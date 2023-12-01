#include "Queues.h"

Queues::Queues()
	:graphicsQueue(VK_NULL_HANDLE), presentQueue(VK_NULL_HANDLE), transferQueue(VK_NULL_HANDLE) {}

Queues::Queues(VkDevice const device, QueueFamilyIndices const queueFamilyIndices) {
	vkGetDeviceQueue(device, queueFamilyIndices.graphicsFamilyIndex, 0, &graphicsQueue);
	vkGetDeviceQueue(device, queueFamilyIndices.presentFamilyIndex, 0, &presentQueue);
	vkGetDeviceQueue(device, queueFamilyIndices.transferFamilyIndex, 0, &transferQueue);
	vkGetDeviceQueue(device, queueFamilyIndices.computeFamilyIndex, 0, &computeQueue);
}

Queues::~Queues() {}
