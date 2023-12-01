/**
 * @file Application.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief This is the main class which manages the whole game, including the main loop, graphics, input handling and physics.
 * @version 0.1
 * @date 2020-05-03
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "GLFW/glfw3.h"
#include "VulkanWrapper.h"
#include "InputHandler.h"
#include "Input.h"
#include "Camera.h"
#include "LoadedChunks.h"
#include "MapGenerator.h"
#include "Map.h"

 /**
  * @brief The Application class houses all relevant Wrapper/Handler classes and executes the main game loop.
  */
class Application {
public:
	Application();
	~Application();

	/**
	 * @brief Initializes all neccessary components and launches the main game loop.
	 */
	void start();

private:

	/**
	 * @brief Determines if the main game loop should still be running or being stopped.
	 */
	bool isRunning;

	/**
	 * @brief Handle for the GLFWwindow object, used for window creation and input handling.
	 */
	GLFWwindow *window;

	/**
	 * @brief Wrapper for all neccessary vulkan things, rendering a frame is enqueued through this wrapper.
	 */
	VulkanWrapper *vulkanWrapper;

	/**
	 * @brief Wrapper for all neccessary GLFW things, input from mouse and keyboard is in this wrapper.
	 */
	InputHandler inputHandler;

	/**
	 * @brief saves all input.
	 */
	INPUT input;

	/**
	 * @brief Creates a camera that handley the view for the player.
	 */
	Camera camera;

	/**
	 * @brief Handles all currently loaded chunks of the game.
	 */
	LoadedChunks *loadedChunks;

	/**
	 * @brief in case a GLFW function fails, an error is reported to the GLFW error callback.
	 */
	static void error_callback(int error, const char *description) {
		fprintf(stderr, "Error in InputHandler : %s\n", description);
	}
	/**
	 * @brief Initializes all relevant components, needs to be called before mainLoop function.
	 */
	void initialize();

	/**
	 * @brief Initializes the GLFWwindow which will be used to output the rendered frames.
	 */
	void initializeGLFWWindow();

	/**
	 * @brief The main loop of the whole game, calls every function which needs to be executed every game tick.
	 */
	void mainLoop();
};

#endif // APPLICATION_H