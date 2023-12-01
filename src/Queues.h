/**
 * @file Queues.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief File contains a class which gathers all queue handles.
 * @version 0.1
 * @date 2020-05-03
 *
 * @copyright Copyright (c) 2020
 *
 */
#ifndef QUEUES_H
#define QUEUES_H

#include "QueueFamilyIndices.h"

#include "vulkan/vulkan.h"

 /**
  * @brief Contains all needed device queues.
  */
class Queues {
public:
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue transferQueue;
	VkQueue computeQueue;

	Queues();

	/**
	 * @brief Constructs a new Queues object.
	 *
	 * Calls the vkGetDeviceQueue function to initialize the queues.
	 *
	 * @param device Handle of the vulkan device, which is needed for the vkGetDeviceQueue calls.
	 * @param queueFamilyIndices Contains the indices of the device queues.
	 */
	Queues(VkDevice const device, QueueFamilyIndices const queueFamilyIndices);

	~Queues();
};

#endif // !QUEUES_H