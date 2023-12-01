#include "Application.h"
#include "Settings.h"
#include "Cube.h"
#include "Chunk.h"
#include "Frustum.h"
#include "ComputeSettings.h"
#include "Profiler.h"

#include "vulkan/vulkan.h"
#include "glm/gtx/rotate_vector.hpp"

#include <vector>
#include <chrono>
#include <thread>

Application::Application()
	:isRunning(true), window(nullptr) {}

Application::~Application() {

	//Waiting till the device is idle, so we can destroy everything
	vulkanWrapper->waitForDeviceIdle();

	loadedChunks->~LoadedChunks();

	vulkanWrapper->~VulkanWrapper();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Application::start() {
	initialize();

	mainLoop();
}

void Application::mainLoop() {
	//loadedChunks->loadMap();

	ThreadPool::getInstance().submit([this] {
		loadedChunks->loadMap();
		});

	ThreadPool::getInstance().submit([this] {
		while (isRunning) {
			loadedChunks->updateMiddle(camera.getCameraPosition());
		}
		});

	ThreadPool::getInstance().submit([this] {
		bool changed = true;

		Profiler profiler = Profiler();
		
		Frustum frustum = Frustum(10000.0f, 0.1f, camera.cameraUp);
		frustum.updateFrustum(camera.getCameraPosition(), camera.cameraFront, camera.fov);

		while (isRunning) {

			if (!Settings::IN_PHOTO_MODE) {
				changed = true;

				Settings::skyUbo.cameraPosition = glm::vec4(camera.getCameraPosition(), 0.0f);

				if (Settings::UPDATE_FRUSTUM) {
					frustum.updateFrustum(camera.getCameraPosition(), camera.cameraFront, camera.fov);
				}

				uint32_t imageIndex = 0;
				bool result = vulkanWrapper->startRenderRecording(camera.getView(), camera.getProjection(), imageIndex);

				if (result) {
					for (auto iterator = loadedChunks->loadedChunkStacks.begin(); iterator != loadedChunks->loadedChunkStacks.end(); iterator++) {
						if (iterator->second->willBeRemoved) {
							iterator->second->lifeCounter--;
						}
						else {
							if (iterator->second->chunkStackReady && frustum.isInside(iterator->second->aabb)) {
								for (size_t y = 0; y < iterator->second->chunkStack.stack.size(); y++) {
									if (iterator->second->chunkIndexCount[y] != 0) {
										vulkanWrapper->addChunkToRender(imageIndex, iterator->second->chunkVertexBuffer[y], iterator->second->chunkIndexBuffer[y], iterator->second->chunkIndexCount[y]);
									}
								}
							}
						}
					}

					vulkanWrapper->changeToObjPipeline(imageIndex);

					for (auto iterator = loadedChunks->loadedChunkStacks.begin(); iterator != loadedChunks->loadedChunkStacks.end(); iterator++) {
						if (!iterator->second->willBeRemoved) {
							if (iterator->second->chunkStackReady && frustum.isInside(iterator->second->aabb)) {
								for (size_t y = 0; y < iterator->second->chunkStack.stack.size(); y++) {
									if (iterator->second->objIndexCount[y] != 0) {
										vulkanWrapper->addObjChunkToRender(imageIndex, iterator->second->objVertexBuffer[y], iterator->second->objIndexBuffer[y], iterator->second->objIndexCount[y]);
									}
								}
							}
						}
					}

					vulkanWrapper->changeToSkyPipeline(imageIndex);

					//Render sky
					vulkanWrapper->addSkyToRender(imageIndex);


					vulkanWrapper->SubmitRender(imageIndex);
				}
			}
			else {
				if (changed) {
					std::vector<BoxData> boxData = {};
					std::vector<BoxData> emissiveBoxData = {};
					std::vector<PointLight> pointLights = {};
					pointLights.push_back({ glm::vec4(50.f, 100.f, 50.f, 0.0f), glm::vec4(300.f)});
					loadedChunks->generateBoxData(boxData, emissiveBoxData);
					vulkanWrapper->loadComputeBoxes(boxData, emissiveBoxData, pointLights);
					changed = false;
				}

				glm::vec3 cameraPosition = glm::vec3(camera.getCameraPosition());
				glm::vec3 cameraDirection = glm::vec3(camera.cameraFront);
				glm::vec3 cameraUp = glm::vec3(camera.cameraUp);


				glm::ivec4 iData = ComputeSettings::iData;

				if (ComputeSettings::benchmarkStatus != BenchmarkStatus::OFF) {
					profiler.benchmarkSetUpData(cameraPosition, cameraDirection, cameraUp, iData);
				} else {
					profiler.oldBenchmarkStatus = BenchmarkStatus::OFF;
				}

				profiler.profilerSetUpData();

				vulkanWrapper->renderPhotomode(cameraPosition, cameraDirection, cameraUp, iData);

				profiler.profilerCollectData();

				if (profiler.oldBenchmarkStatus != BenchmarkStatus::OFF) {
					profiler.benchmarkCollectData();
				}
			}
		}
		});

	static auto last = std::chrono::high_resolution_clock::now();

	while (isRunning) {
		auto current = std::chrono::high_resolution_clock::now();
		float elapsed = std::chrono::duration<float, std::chrono::milliseconds::period>(current - last).count();

		//Aims for 144fps
		if (elapsed > 6.9f) {
			glfwPollEvents();

			inputHandler.update(window, &input);
			camera.update(window, &input);

			isRunning = !glfwWindowShouldClose(window);

			//For debug fps use this
			//std::cout << "Fps: " << 1000.f / elapsed << std::endl;

			//float rotation = (elapsed / 1.2e6f) * 2.0f * M_PI; //20 minutes day/night cycle
			float rotation = (elapsed / 60000.0f) * 2.0f * M_PI; //1 minute day/night cycle

			Settings::skyUbo.lightDirection = glm::rotateZ(Settings::skyUbo.lightDirection, rotation);
			Settings::skyUbo.time += elapsed;

			last = current;
		}
	}
}

void Application::initialize() {
	initializeGLFWWindow();
	inputHandler.initialize(window, &input);
	camera.initialize(window, &input);
	vulkanWrapper = new VulkanWrapper();
	vulkanWrapper->initialize(window);
	loadedChunks = new LoadedChunks(*vulkanWrapper);
}

void Application::initializeGLFWWindow() {
	// needs to be called before the initialization, which notifies if an error during or after the initialization happens
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) { //Initializes the GLFW library
		exit(EXIT_FAILURE); // Fails if an error ocurrs
	}
	//glfwInit(); //Initializes the GLFW library
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //Telling glfw to not create a OpenGL context, so we can create a vulkan one

	window = glfwCreateWindow(Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, Settings::WINDOW_NAME, nullptr, nullptr); //Creating the actual window, using the specified settings
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
}
