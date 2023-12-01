#include "RenderSynchronisation.h"
#include "Settings.h"

#include <stdexcept>

RenderSynchronisation::RenderSynchronisation() {}

RenderSynchronisation::RenderSynchronisation(VkDevice const &device, uint32_t const swapChainImageCount)
	:device(device) {
	createRenderSynchronisationObjects(device, swapChainImageCount);
}

RenderSynchronisation::~RenderSynchronisation() {
	for (size_t i = 0; i < Settings::MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);

		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);

		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	vkDestroyFence(device, computeFence, nullptr);
}

void RenderSynchronisation::createRenderSynchronisationObjects(VkDevice const &device, uint32_t const swapChainImageCount) {
	//Resizing the vectors to be able to contain the needed amount of semaphores/fences
	imageAvailableSemaphores.resize(Settings::MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(Settings::MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(Settings::MAX_FRAMES_IN_FLIGHT);

	//Since this will be used to check if a given swapchain image is still in usage, we need to resize the array according to the swapchain image count
	imagesInFlight.resize(swapChainImageCount, VK_NULL_HANDLE);

	//Setting up the Semaphores
	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	//Setting up the fences
	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//Creating the Semaphores and the fences needed for rendering. This shouldn't set any fences of the images in flight fences, because those are set by the rendering function with references to the inFlightFences fences
	for (size_t i = 0; i < Settings::MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create semaphore");
		}

		if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create semaphore");
		}

		if (vkCreateFence(device, &fenceCreateInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create fence");
		}
	}

	if (vkCreateFence(device, &fenceCreateInfo, nullptr, &computeFence) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create compute fence");
	}
}
