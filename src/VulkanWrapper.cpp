#include "VulkanWrapper.h"
#include "Settings.h"
#include "DeviceFinder.h"
#include "RenderPassCreator.h"
#include "PipelineCreator.h"
#include "ComputeSettings.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cstring>

VulkanWrapper::VulkanWrapper()
	:window(nullptr), instance(VK_NULL_HANDLE), surface(VK_NULL_HANDLE), physicalDevice(VK_NULL_HANDLE), device(VK_NULL_HANDLE), renderPass(VK_NULL_HANDLE) {}

VulkanWrapper::~VulkanWrapper() {
	vkDeviceWaitIdle(device);

	computeWrapper->~ComputeWrapper();

	cleanUpSwapchain();

	textureArray->~TextureArray();
	skyBox->~SkyBox();
	skyWrapper->~SkyWrapper();
	renderSynchronisation->~RenderSynchronisation();
	commandWrapper->~CommandWrapper();

	vkDestroyDevice(device, nullptr);

	vkDestroySurfaceKHR(instance, surface, nullptr);

	//Need to destroy all vulkan objects before calling this
	vkDestroyInstance(instance, nullptr);
}

void VulkanWrapper::initialize(GLFWwindow *window) {
	this->window = window;
	glfwSetWindowUserPointer(window, this); //Storing a reference to the VulkanWrapper instance within our window object, later used to gain access to the VulkanWrapper if for example we need to call a function
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

	createInstance();

	createSurface();

	//Using DeviceFinder
	setPhysicalDevice();
	createDevice();

	getQueues();

	createCommandWrapper();

	createBufferCreator();
	createImageCreator();

	createSwapchainWrapper();

	createRenderPass();
	swapchainWrapper->createSwapchainFramebuffers(renderPass);
	createRenderSynchronisation();

	createDescriptorWrapper();
	textureArray = new TextureArray(device, imageCreator);
	skyBox = new SkyBox(device, imageCreator);
	skyWrapper = new SkyWrapper(device, bufferCreator, imageCreator, swapchainWrapper->swapchainImages.size());
	descriptorWrapper->createDescriptorSets(swapchainWrapper->uniformBufferObjects, *textureArray);
	descriptorWrapper->createObjDescriptorSets(swapchainWrapper->uniformBufferObjects, *textureArray);
	descriptorWrapper->createSkyDescriptorSets(swapchainWrapper->uniformBufferObjects, skyWrapper->skyUniformBufferObjects, *skyWrapper->cloudTexture);

	commandWrapper->createCommandBuffers(swapchainWrapper->swapchainImages.size());

	createPipeline();

	createComputeWrapper(ComputeSettings::COMPUTE_SHADER_PATH);
	descriptorWrapper->createQuadDescriptorSets(swapchainWrapper->uniformBufferObjects, computeWrapper->computeTextureImageView, computeWrapper->computeTextureSampler);
	commandWrapper->createQuadCommandBuffers(swapchainWrapper->swapchainImages.size());
	commandWrapper->createComputeCommandBuffer();
}

void VulkanWrapper::createVulkanLoadedChunk(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, VkBuffer &vertexBuffer, VkDeviceMemory &vertexBufferMemory, VkBuffer &indexBuffer, VkDeviceMemory &indexBufferMemory) {
	bufferCreator.createVertexBuffer(vertices, vertexBuffer, vertexBufferMemory);
	bufferCreator.createIndexBuffer(indices, indexBuffer, indexBufferMemory);
}

void VulkanWrapper::createVulkanObjChunk(std::vector<BigVertex> &vertices, std::vector<uint32_t> &indices, VkBuffer &vertexBuffer, VkDeviceMemory &vertexBufferMemory, VkBuffer &indexBuffer, VkDeviceMemory &indexBufferMemory) {
	bufferCreator.createBigVertexBuffer(vertices, vertexBuffer, vertexBufferMemory);
	bufferCreator.createIndexBuffer(indices, indexBuffer, indexBufferMemory);
}

void VulkanWrapper::deleteVulkanLoadedChunk(VkBuffer &vertexBuffer, VkDeviceMemory &vertexBufferMemory, VkBuffer &indexBuffer, VkDeviceMemory &indexBufferMemory) {
	{
		std::lock_guard<std::mutex> lockGuard(CommandWrapper::mutexCommandPool);
		vkDestroyBuffer(device, vertexBuffer, nullptr);
		vkFreeMemory(device, vertexBufferMemory, nullptr);
		vkDestroyBuffer(device, indexBuffer, nullptr);
		vkFreeMemory(device, indexBufferMemory, nullptr);
	}
}

bool VulkanWrapper::startRenderRecording(glm::mat4 const &view, glm::mat4 const &projection, uint32_t &imageIndex) {
	//Waiting till the InFlightSpot is ready
	vkWaitForFences(device, 1, &renderSynchronisation->inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	//Try's to acquire the next image out of the swapchain
	VkResult result = vkAcquireNextImageKHR(device, swapchainWrapper->swapchain, UINT64_MAX, renderSynchronisation->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	//Recreate swapchain if incompatible to surface
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapchain();
		return false;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) { //Suboptimal means, the swapchain still can be represented in a not perfect way on the surface
		throw std::runtime_error("Failed to acquire swapchain image");
	}

	//Wait if image is still in use
	if (renderSynchronisation->imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(device, 1, &renderSynchronisation->imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	renderSynchronisation->imagesInFlight[imageIndex] = renderSynchronisation->inFlightFences[currentFrame];

	//Update the uniform buffer used by the shaders
	updateUniformBufferObject(imageIndex, glm::mat4(1), view, projection);
	updateSkyUniformBufferObject(imageIndex);

	//Starts recording
	commandWrapper->startRecordingCommandBuffer(imageIndex, renderPass, swapchainWrapper->swapchainFramebuffers[imageIndex], swapchainWrapper->extent, pipeline, pipelineLayout, descriptorWrapper->descriptorSets[imageIndex]);

	//Signal that everything went ok and we can now record draw calls
	return true;
}

void VulkanWrapper::addChunkToRender(uint32_t const imageIndex, VkBuffer vertexBuffer, VkBuffer indexBuffer, uint32_t const indexCount) {
	commandWrapper->recordChunk(imageIndex, vertexBuffer, indexBuffer, indexCount);
}

void VulkanWrapper::changeToObjPipeline(uint32_t const imageIndex) {
	commandWrapper->changeShader(imageIndex, objPipeline, objPipelineLayout, descriptorWrapper->objDescriptorSets[imageIndex]);
}

void VulkanWrapper::addObjChunkToRender(uint32_t const imageIndex, VkBuffer vertexBuffer, VkBuffer indexBuffer, uint32_t const indexCount) {
	commandWrapper->recordChunk(imageIndex, vertexBuffer, indexBuffer, indexCount);
}

void VulkanWrapper::changeToSkyPipeline(uint32_t const imageIndex) {
	commandWrapper->changeShader(imageIndex, skyPipeline, skyPipelineLayout, descriptorWrapper->skyDescriptorSets[imageIndex]);
}

void VulkanWrapper::addSkyToRender(uint32_t const imageIndex) {
	commandWrapper->recordChunk(imageIndex, skyWrapper->boxVertices, skyWrapper->boxIndices, 36);
}

void VulkanWrapper::SubmitRender(uint32_t const imageIndex) {
	//Ends the recording
	commandWrapper->endRecordingCommandBuffer(imageIndex);

	//Setting up the submit of the coommand buffer
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { renderSynchronisation->imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandWrapper->commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { renderSynchronisation->renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device, 1, &renderSynchronisation->inFlightFences[currentFrame]);

	//Submit the commands
	if (vkQueueSubmit(queues.graphicsQueue, 1, &submitInfo, renderSynchronisation->inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapchainWrapper->swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	//Enqueue the presentation of the image
	VkResult result = vkQueuePresentKHR(queues.presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frameBufferResized) {
		frameBufferResized = false;
		recreateSwapchain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to present swapchain image");
	}

	//Setting our current frame to ne next one if necessary wrap around to 0 again
	currentFrame = (currentFrame + 1) % Settings::MAX_FRAMES_IN_FLIGHT;
}

void VulkanWrapper::renderPhotomode(glm::vec3 const &cameraPosition, glm::vec3 const &cameraDirection, glm::vec3 const &up, glm::vec4 const &iData) {
	uint32_t imageIndex = 0;

	//Waiting till the InFlightSpot is ready
	vkWaitForFences(device, 1, &renderSynchronisation->inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	//Try's to acquire the next image out of the swapchain
	VkResult result = vkAcquireNextImageKHR(device, swapchainWrapper->swapchain, UINT64_MAX, renderSynchronisation->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	//Recreate swapchain if incompatible to surface
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapchain();
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) { //Suboptimal means, the swapchain still can be represented in a not perfect way on the surface
		throw std::runtime_error("Failed to acquire swapchain image");
	}

	//Wait if image is still in use
	if (renderSynchronisation->imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(device, 1, &renderSynchronisation->imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	renderSynchronisation->imagesInFlight[imageIndex] = renderSynchronisation->inFlightFences[currentFrame];

	//Starts recording
	commandWrapper->recordQuadCommandBuffers(imageIndex, renderPass, swapchainWrapper->swapchainFramebuffers[imageIndex], swapchainWrapper->extent, computeWrapper->quadPipeline, computeWrapper->quadPipelineLayout, descriptorWrapper->quadDescriptorSets[imageIndex], computeWrapper->computeTextureImage, computeWrapper->quadVertices, computeWrapper->quadIndexBuffer);

	//Setting up the submit of the coommand buffer
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { renderSynchronisation->imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandWrapper->quadCommandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { renderSynchronisation->renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device, 1, &renderSynchronisation->inFlightFences[currentFrame]);

	//Submit the commands
	if (vkQueueSubmit(queues.graphicsQueue, 1, &submitInfo, renderSynchronisation->inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapchainWrapper->swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	//Enqueue the presentation of the image
	result = vkQueuePresentKHR(queues.presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || frameBufferResized) {
		frameBufferResized = false;
		recreateSwapchain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to present swapchain image");
	}

	//Setting our current frame to ne next one if necessary wrap around to 0 again
	currentFrame = (currentFrame + 1) % Settings::MAX_FRAMES_IN_FLIGHT;

	//Running compute here
	vkWaitForFences(device, 1, &renderSynchronisation->computeFence, VK_TRUE, UINT64_MAX);
	vkResetFences(device, 1, &renderSynchronisation->computeFence);

	computeWrapper->updateCameraData(imageIndex, cameraPosition, cameraDirection, up, iData);

	commandWrapper->recordComputeCommandBuffer(computeWrapper->computePipeline, computeWrapper->computePipelineLayout, descriptorWrapper->computeDescriptorSets[imageIndex]);

	VkSubmitInfo computeSubmitInfo{};
	computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	computeSubmitInfo.commandBufferCount = 1;
	computeSubmitInfo.pCommandBuffers = &commandWrapper->computeCommandBuffer;

	if (vkQueueSubmit(queues.computeQueue, 1, &computeSubmitInfo, renderSynchronisation->computeFence) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit compute command buffer");
	}

	vkWaitForFences(device, 1, &renderSynchronisation->computeFence, VK_TRUE, UINT64_MAX);
	computeWrapper->storeAndResetWriteBackData();
}

void VulkanWrapper::waitForDeviceIdle() {
	vkDeviceWaitIdle(device);
}

void VulkanWrapper::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
	VulkanWrapper *vulkanWrapper = reinterpret_cast<VulkanWrapper *>(glfwGetWindowUserPointer(window)); //Because we can't use this reference
	vulkanWrapper->frameBufferResized = true;

#ifdef _DEBUG
	//Debug utility printing out if glfw window was resized
	std::cout << "GLFW window was resized: " << width << " x " << height << std::endl;
#endif // _DEBUG

}

void VulkanWrapper::loadComputeBoxes(std::vector<BoxData> const &boxData, std::vector<BoxData> const &emissiveBoxData, std::vector<PointLight> const &pointLights) {
	if (computeWrapper->boxesGenerated) {
		vkDestroyBuffer(device, computeWrapper->boxDataBuffer, nullptr);
		vkFreeMemory(device, computeWrapper->boxDataBufferMemory, nullptr);
	}

	computeWrapper->createDataBuffers(boxData, emissiveBoxData, pointLights);
	descriptorWrapper->createComputeDescriptorSets(computeWrapper->cameraDataBuffers, computeWrapper->computeTextureImageView, computeWrapper->computeTextureSampler, *textureArray, *skyBox, computeWrapper->boxDataBuffer, boxData, computeWrapper->emissiveBoxDataBuffer, emissiveBoxData, computeWrapper->pointLightBuffer, pointLights, computeWrapper->writeBackDataBuffer, computeWrapper->allocated);
}

void VulkanWrapper::createInstance() {

	//Creating the application info struct, which is needed for the instance create info
	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = Settings::WINDOW_NAME;
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = Settings::ENGINE_NAME;
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_2;

	//Creating the instance create info struct, which will be passed as argument to the createInstance call
	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &applicationInfo;

#ifdef _DEBUG
	//Debug utility printing out all available extensions
	printAllAvailableInstanceExtensions();
#endif // _DEBUG

	std::vector<char const *> requiredInstanceExtensions = getGLFWRequiredInstanceExtensions();

	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredInstanceExtensions.size());
	instanceCreateInfo.ppEnabledExtensionNames = requiredInstanceExtensions.data();

#ifdef _DEBUG
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	const std::vector<const char *> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size()); //Setzt die Anzahl der aktiven ValidationLayers
	instanceCreateInfo.ppEnabledLayerNames = validationLayers.data(); //Setzt die Namen der aktiven ValidationLayers

	//instanceCreateInfo(debugCreateInfo); //f�llt das debugCreateInfo Struct
	debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT; //OPtional VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
	debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugCreateInfo.pfnUserCallback = debugCallback;
	debugCreateInfo.pUserData = nullptr; //Kann als Pointer zu NutzerKlassen wie der Vulkan Boilerplate Klasse oder �hnlichen genutzt werden


	instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo; //Setzt das Debug als Next handle von dem instance Handle
#endif // _DEBUG


	if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create instance");
	}
}

std::vector<char const *> VulkanWrapper::getGLFWRequiredInstanceExtensions() {
	uint32_t glfwInstanceExtensionCount = 0;
	const char **glfwInstanceExtensions;

	glfwInstanceExtensions = glfwGetRequiredInstanceExtensions(&glfwInstanceExtensionCount); //Returns an array with the names of all required Extensions to create a surface for glfw windows

	std::vector<char const *> requiredInstanceExtension(glfwInstanceExtensions, glfwInstanceExtensions + glfwInstanceExtensionCount);

#ifdef _DEBUG
	requiredInstanceExtension.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // _DEBUG



	return requiredInstanceExtension;
}

void VulkanWrapper::createSurface() {
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) { //Creates a vulkan surface for the specified glfw window
		throw std::runtime_error("Failed to create the window surface");
	}
}

void VulkanWrapper::setPhysicalDevice() {
	DeviceFinder deviceFinder = DeviceFinder();

#ifdef _DEBUG
	//Debug utility printing out all physical devices
	deviceFinder.printAllPhysicalDevices(instance);
#endif // _DEBUG

	if (deviceFinder.findPhysicalDevice(instance, surface, physicalDevice, queueFamilyIndices) != true) {
		throw std::runtime_error("Failed to find a vulkan compatible gpu");
	}

#ifdef _DEBUG
	//Debug utility printing out all used queue family indices
	std::cout << "Used queue family indices:" << std::endl;
	std::cout << "\t" << "Graphics family index: " << queueFamilyIndices.graphicsFamilyIndex << std::endl;
	std::cout << "\t" << "Present family index: " << queueFamilyIndices.presentFamilyIndex << std::endl;
	std::cout << "\t" << "Transfer family index: " << queueFamilyIndices.transferFamilyIndex << std::endl;
#endif // _DEBUG

}

void VulkanWrapper::createDevice() {
	DeviceFinder deviceFinder = DeviceFinder();
	deviceFinder.createDevice(physicalDevice, queueFamilyIndices, device);
}

void VulkanWrapper::getQueues() {
	queues = Queues(device, queueFamilyIndices);
}

void VulkanWrapper::createCommandWrapper() {
	commandWrapper = new CommandWrapper(device, queueFamilyIndices, queues);
}

void VulkanWrapper::createBufferCreator() {
	bufferCreator = BufferCreator(physicalDevice, device, queueFamilyIndices, *commandWrapper);
}

void VulkanWrapper::createImageCreator() {
	imageCreator = ImageCreator(physicalDevice, device, queueFamilyIndices, *commandWrapper, bufferCreator);
}

void VulkanWrapper::createSwapchainWrapper() {
	DeviceFinder deviceFinder = DeviceFinder();

	swapchainWrapper = new SwapchainWrapper(window, surface, device, deviceFinder.getPhysicalDeviceSurfaceInfo(physicalDevice, surface), queueFamilyIndices, imageCreator, bufferCreator);
}

void VulkanWrapper::createRenderPass() {
	RenderPassCreator::createRenderPass(device, swapchainWrapper->swapchainImageFormat, swapchainWrapper->depthBufferFormat, renderPass);
}

void VulkanWrapper::createDescriptorWrapper() {
	descriptorWrapper = new DescriptorWrapper(device, swapchainWrapper->swapchainImages.size());
}

void VulkanWrapper::createPipeline() {
	PipelineCreator::createPipeLine(device, swapchainWrapper->extent, descriptorWrapper->descriptorSetLayout, renderPass, Settings::VERTEX_SHADER_PATH, Settings::FRAGMENT_SHADER_PATH, false, false, pipelineLayout, pipeline);
	
	PipelineCreator::createPipeLine(device, swapchainWrapper->extent, descriptorWrapper->objDescriptorSetLayout, renderPass, Settings::OBJ_VERTEX_SHADER_PATH, Settings::OBJ_FRAGMENT_SHADER_PATH, false, true, objPipelineLayout, objPipeline);

	PipelineCreator::createPipeLine(device, swapchainWrapper->extent, descriptorWrapper->skyDescriptorSetLayout, renderPass, Settings::SKY_VERTEX_SHADER_PATH, Settings::SKY_FRAGMENT_SHADER_PATH, false, false, skyPipelineLayout, skyPipeline);
}

void VulkanWrapper::createRenderSynchronisation() {
	renderSynchronisation = new RenderSynchronisation(device, static_cast<uint32_t>(swapchainWrapper->swapchainImages.size()));
}

void VulkanWrapper::updateUniformBufferObject(uint32_t const imageIndex, glm::mat4 const &model, glm::mat4 const &view, glm::mat4 const &projection) {
	UniformBufferObject uniformBufferObject{};

	uniformBufferObject.model = model;
	uniformBufferObject.view = view;
	uniformBufferObject.projection = projection;
	uniformBufferObject.projection[1][1] *= -1.0f;
	uniformBufferObject.sunDirection = Settings::skyUbo.lightDirection;

	void *data;
	vkMapMemory(device, swapchainWrapper->uniformBufferObjectsMemory[imageIndex], 0, sizeof(uniformBufferObject), 0, &data);
	memcpy(data, &uniformBufferObject, sizeof(uniformBufferObject));
	vkUnmapMemory(device, swapchainWrapper->uniformBufferObjectsMemory[imageIndex]);
}

void VulkanWrapper::updateSkyUniformBufferObject(uint32_t const imageIndex) {
	SkyUBO skyUBO = SkyUBO();

	skyUBO = Settings::skyUbo;

	void *data;
	vkMapMemory(device, skyWrapper->skyUniformBufferObjectsMemory[imageIndex], 0, sizeof(skyUBO), 0, &data);
	memcpy(data, &skyUBO, sizeof(skyUBO));
	vkUnmapMemory(device, skyWrapper->skyUniformBufferObjectsMemory[imageIndex]);
}

void VulkanWrapper::cleanUpSwapchain() {

	vkDestroyPipeline(device, pipeline, nullptr);

	commandWrapper->freeCommandBuffers();

	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

	vkDestroyRenderPass(device, renderPass, nullptr);

	swapchainWrapper->~SwapchainWrapper();

	descriptorWrapper->~DescriptorWrapper();
}

void VulkanWrapper::recreateSwapchain() {
	glfwGetFramebufferSize(window, &Settings::WINDOW_WIDTH, &Settings::WINDOW_HEIGHT);

	while (Settings::WINDOW_WIDTH == 0 || Settings::WINDOW_HEIGHT == 0) {
		glfwGetFramebufferSize(window, &Settings::WINDOW_WIDTH, &Settings::WINDOW_HEIGHT);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(device);

	cleanUpSwapchain();

	createSwapchainWrapper();
	createRenderPass();
	swapchainWrapper->createSwapchainFramebuffers(renderPass);
	createDescriptorWrapper();
	descriptorWrapper->createDescriptorSets(swapchainWrapper->uniformBufferObjects, *textureArray);
	descriptorWrapper->createObjDescriptorSets(swapchainWrapper->uniformBufferObjects, *textureArray);
	descriptorWrapper->createSkyDescriptorSets(swapchainWrapper->uniformBufferObjects, skyWrapper->skyUniformBufferObjects, *skyWrapper->cloudTexture);
	commandWrapper->createCommandBuffers(swapchainWrapper->swapchainImages.size());
	createPipeline();
}

void VulkanWrapper::printAllAvailableInstanceExtensions() {
	uint32_t propertyCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr);

	std::vector<VkExtensionProperties> extensionPropterties(propertyCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, extensionPropterties.data());

	std::cout << "Available Extensions:" << std::endl;
	for (VkExtensionProperties const &extensionProperty : extensionPropterties) {
		std::cout << "\t" << extensionProperty.extensionName << std::endl;
	}
}

void VulkanWrapper::createComputeWrapper(char const *computeShaderPath) {
	computeWrapper = new ComputeWrapper(device, imageCreator, bufferCreator, swapchainWrapper->extent, swapchainWrapper->swapchainImages.size(), descriptorWrapper->quadDescriptorSetLayout, renderPass, descriptorWrapper->computeDescriptorSetLayout, computeShaderPath);
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanWrapper::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallBackData, void *pUserData) {
	std::cerr << "validation Layer: " << pCallBackData->pMessage << std::endl;

	return VK_FALSE;
}
