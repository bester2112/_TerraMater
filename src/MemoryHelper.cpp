#include "MemoryHelper.h"

#include <stdexcept>

MemoryHelper::MemoryHelper() {}

MemoryHelper::~MemoryHelper() {}

uint32_t MemoryHelper::findMemoryType(VkPhysicalDevice const &physicalDevice, uint32_t const typeFilter, VkMemoryPropertyFlags const memoryPropertyFlags) {
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

	for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++) {
		if (typeFilter & (1 << i) && (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags) {
			return i;
		}
	}

	//No suitable memory found, so we need to stop
	throw std::runtime_error("Failed to find suitable memory type");
}