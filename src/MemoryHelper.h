/**
 * @file MemoryHelper.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Contains a helper class to find a suitable memory type in a physical device.
 * @version 0.1
 * @date 2020-05-04
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef MEMORYHELPER_H
#define MEMORYHELPER_H

#include "vulkan/vulkan.h"

 /**
  * @brief Helper class to find a suitable memory type on a physical device.
  */
class MemoryHelper {
public:
	MemoryHelper();
	~MemoryHelper();

	/**
	 * @brief Finds a suitable memory type on a physical device.
	 *
	 * @param physicalDevice Handle of the physical device for which the memory types should be enumerated.
	 * @param typeFilter Filter for the type of the wanted memory.
	 * @param memoryPropertyFlags Flags which the memory type need to fulfill.
	 * @return uint32_t The index of the found suitable memory type.
	 */
	static uint32_t findMemoryType(VkPhysicalDevice const &physicalDevice, uint32_t const typeFilter, VkMemoryPropertyFlags const memoryPropertyFlags);

private:

};

#endif // !MEMORYHELPER_H
