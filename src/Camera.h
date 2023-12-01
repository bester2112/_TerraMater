/**
 * @file InputHandler.h
 * @author Thomas Rzepka (uqdll@student.kit.edu)
 * @brief Includes the Camera class which manages cameras ystem in for the player.
 * @version 0.1
 * @date 2020-05-03
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef CAMERA_H
#define CAMERA_H

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "Input.h"
#include "Settings.h"
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

 /*
  * @brief instance that is needed to save values from a callback function
  */
class Camera {
public:
	Camera();
	~Camera();

	/*
	 * @brief initialise the camera
	 */
	void initialize(GLFWwindow *window, INPUT *input);

	/*
	 * @brief calles all necessary methods to update the camera
	 */
	void update(GLFWwindow *window, INPUT *input);

	/*
	 * @brief returns the view matrix
	 */
	glm::mat4 getView();

	/*
	 * @brief returns the projection matrix
	 */
	glm::mat4 getProjection();
	
	/*
	 * @brief returns the camera position
	 */
	glm::vec3 getCameraPosition();

	/*
	 * front front facing camera
	 */
	glm::vec3 cameraFront;

	/*
	 * @brief vector that shows in the y direction
	 */
	glm::vec3 cameraUp;

	/*
	 * @brief field of view
	 */
	float fov;

private:

	/*
	 * @brief how fast the camera should move
	 */
	//const float cameraSpeed = 0.15f; // adjust accordingly
	const float cameraSpeed = 0.5f; // adjust accordingly

	/*
	 * @brief view matrix
	 */
	glm::mat4 view;

	/* 
	 * @brief projection matrix
	 */
	glm::mat4 projection;

	/* 
	 * @brief position of the camera
	 */
	glm::vec3 cameraPosition;

	/*
	 * @brief boolean that saves if the method was called the first time.
	 */
	bool firstTime;

	double yaw;
	double pitch;

	/*
	 * @brief x-position from last frame
	 */
	double lastX;
	
	/*
	 * @brief y position from last frame
	 */
	double lastY;

	/*
	 * @brief moves the camera with the interacting inputs
	 */
	void moveCamera(GLFWwindow *window, INPUT *input);

	/*
	 * @brief changes the camera with the input of the cursor
	 */
	void cursorMovement(GLFWwindow *window, INPUT *input);
};

#endif // CAMERA_H