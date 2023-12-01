/**
 * @file DeviceFinder.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Holds the DeviceFinder class, which is a small helper class with functions to pick/create a physical/logical device.
 * @version 0.1
 * @date 2020-05-03
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef DEVICEFINDER_H
#define DEVICEFINDER_H

#include "QueueFamilyIndices.h"
#include "PhysicalDeviceSurfaceInfo.h"

#include "vulkan/vulkan.h"

#include <vector>
#include <string>

 /**
 * @brief Helper class to pick a physical device, find its queue family indices and to create a logical device.
 */
class DeviceFinder {
public:
	DeviceFinder();
	~DeviceFinder();

	/**
	 * @brief Finds the best physical device to use, finds also the corresponding queue family indices.
	 * 
	 * @param instance Handle of the vulkan instance, needed to enumerate all available physical devices.
	 * @param surface Handle of the vulkan surface, needed for access to the queue family indices.
	 * @param physicalDevice Handle in which the found physical device gets stored.
	 * @param queueFamilyIndices Pointer in which the queue family indices of the corresponding physical device get stored.
	 * @return true if a physical device supporting the needed features was found.
	 * @return false if no physical device supporting the needed features was found.
	 */
	bool findPhysicalDevice(VkInstance const instance, VkSurfaceKHR const &surface, VkPhysicalDevice &physicalDevice, QueueFamilyIndices &queueFamilyIndices) const;

	/**
	 * @brief Creates a logical device for a given physical device.
	 * 
	 * @param physicalDevice Handle of the physical device, for which an logical device should be created.
	 * @param queueFamilyIndices Handle of the queue family indices which should be used within the created logical device.
	 * @param device Handle in which the created logical device gets stored.
	 */
	void createDevice(VkPhysicalDevice const &physicalDevice, QueueFamilyIndices const &queueFamilyIndices, VkDevice &device) const;

	/**
	 * @brief Returns surface infos for a given physical device and surface.
	 * 
	 * @param physicalDevice Handle of the physical device, for which the information should be determined.
	 * @param surface Handle of the vulkan surface, for which the information should be determined.
	 * @return PhysicalDeviceSurfaceInfo with the information, depending on the physical device and surface.
	 */
	PhysicalDeviceSurfaceInfo getPhysicalDeviceSurfaceInfo(VkPhysicalDevice const &physicalDevice, VkSurfaceKHR const &surface) const;

	//Debug functions

	/**
	 * @brief Prints out all available physical devices.
	 * 
	 * @param instance Handle of the vulkan instance, needed to enumerate all available physical devices.
	 */
	void printAllPhysicalDevices(VkInstance const instance) const;

private:

	/**
	* @brief List with the names of the enabled vulkan extensions, which the program needs to run.
	*/
	const std::vector<char const *> enabledExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	/**
	 * @brief Returns a list of all available physical devices.
	 * 
	 * @param instance Handle of the vulkan instance, needed to enumerate all available physical devices.
	 * @return std::vector<VkPhysicalDevice> Vector containing all found physical devices.
	 */
	std::vector<VkPhysicalDevice> listAllPhysicalDevices(VkInstance const instance) const;

	/**
	 * @brief Rates a given physical device for its capability/performance, for example if it's a dedicated gpu.
	 * 
	 * @param surface Handle of the vulkan surface, needed for access to the queue family indices.
	 * @param physicalDevice Handle of the physical device, for which the rating should be calculated.
	 * @param queueFamilyIndices Pointer in which the queue family indices of the corresponding physical device get stored.
	 * @return uint32_t Rating for the given physical device.
	 */
	uint32_t ratePhysicalDevice(VkSurfaceKHR const &surface, VkPhysicalDevice const &physicalDevice, QueueFamilyIndices &queueFamilyIndices) const;

	/**
	 * @brief Checks if all enabled extension are available on a given physical device.
	 * 
	 * @param physicalDevice Handle of the physical device, whose available extension are going to be checked for completeness.
	 * @return true If all enabled extensions are available on the given physical device.
	 * @return false If the physical device, doesn't support all enabled extensions.
	 */
	bool checkForEnabledExtensions(VkPhysicalDevice const &physicalDevice) const;

	/**
	 * @brief Returns the queue family indices, for a given physical device and surface, packaged in the corresponding data class.
	 * 
	 * @param surface Handle of the vulkan surface, needed to check for present support.
	 * @param physicalDevice Handle of the physical device, whose queue family indices are going to be determined.
	 * @return QueueFamilyIndices Struct containing information, if the needed queue family were found and if so there indices.
	 */
	QueueFamilyIndices findQueueFamilyIndices(VkSurfaceKHR const &surface, VkPhysicalDevice const &physicalDevice) const;
};

#endif // !DEVICEFINDER_H