#include "SwapchainWrapper.h"

#include <algorithm>
#include <iostream>

SwapchainWrapper::SwapchainWrapper()
	: device(VK_NULL_HANDLE), swapchain(VK_NULL_HANDLE), extent({}), swapchainImageFormat(VK_FORMAT_UNDEFINED) {}

SwapchainWrapper::SwapchainWrapper(GLFWwindow *window, VkSurfaceKHR const &surface, VkDevice &device, PhysicalDeviceSurfaceInfo const &physicalDeviceSurfaceInfo, QueueFamilyIndices const &queueFamilyIndices, ImageCreator const &imageCreator, BufferCreator const &bufferCreator)
	: device(device), swapchain(VK_NULL_HANDLE) {
	createSwapchain(window, surface, physicalDeviceSurfaceInfo, queueFamilyIndices);
	createSwapchainImageViews(imageCreator);
	createDepthImage(imageCreator);
	createUniformBuffers(bufferCreator);
}

SwapchainWrapper::~SwapchainWrapper() {

	for (auto framebuffer : swapchainFramebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	vkDestroyImageView(device, depthBufferImageView, nullptr);
	vkDestroyImage(device, depthBufferImage, nullptr);
	vkFreeMemory(device, depthBufferImageMemory, nullptr);

	for (size_t i = 0; i < swapchainImages.size(); i++) {
		vkDestroyBuffer(device, uniformBufferObjects[i], nullptr);
		vkFreeMemory(device, uniformBufferObjectsMemory[i], nullptr);
	}

	for (VkImageView imageView : swapchainImageViews) {
		vkDestroyImageView(device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(device, swapchain, nullptr);
}

void SwapchainWrapper::createSwapchainFramebuffers(VkRenderPass const &renderPass) {
	swapchainFramebuffers.resize(swapchainImageViews.size());

	//Creating a framebuffer for each image in the swapchain
	for (size_t i = 0; i < swapchainImageViews.size(); i++) {
		VkImageView attachments[] = { swapchainImageViews[i], depthBufferImageView };

		//Creating the framebuffer create info struct, which will be passed as argument to the createFramebuffer call
		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = renderPass;
		framebufferCreateInfo.attachmentCount = 2;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = extent.width;
		framebufferCreateInfo.height = extent.height;
		framebufferCreateInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create framebuffer");
		}
	}
}

void SwapchainWrapper::createSwapchain(GLFWwindow *window, VkSurfaceKHR const &surface, PhysicalDeviceSurfaceInfo const &physicalDeviceSurfaceInfo, QueueFamilyIndices const &queueFamilyIndices) {
	VkSurfaceFormatKHR surfaceFormat = findSwapchainSurfaceFormat(physicalDeviceSurfaceInfo.surfaceFormats);
	VkPresentModeKHR presentMode = findSwapchainPresentMode(physicalDeviceSurfaceInfo.presentModes);
	setExtent(window, physicalDeviceSurfaceInfo.surfaceCapabilities);

#ifdef _DEBUG
	//Debug utility printing out all swapchain surface infos
	printAllAvailableSwapchainSurfaceFormats(physicalDeviceSurfaceInfo.surfaceFormats);
	printAllAvailableSwapchainPresentModes(physicalDeviceSurfaceInfo.presentModes);
	printExtent();
#endif // _DEBUG

	uint32_t minImageCount = physicalDeviceSurfaceInfo.surfaceCapabilities.minImageCount + 1; //Adding one image more to the minimum, to have a buffer if the driver somehow still does things and we don't have to wait in the meantime

	//Clamping if the +1 is greater than the maximum
	if (physicalDeviceSurfaceInfo.surfaceCapabilities.maxImageCount > 0 && minImageCount > physicalDeviceSurfaceInfo.surfaceCapabilities.maxImageCount) { //Need to check if maximum > 0, because 0 here means no limit
		minImageCount = physicalDeviceSurfaceInfo.surfaceCapabilities.maxImageCount;
	}

	//Creating the swapchain create info struct, which will be passed as argument to the createSwapchain call
	VkSwapchainCreateInfoKHR swapchainCreateInfo{};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = surface;
	swapchainCreateInfo.minImageCount = minImageCount;
	swapchainCreateInfo.imageFormat = surfaceFormat.format;
	swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapchainCreateInfo.imageExtent = extent;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndicesArray[] = { queueFamilyIndices.graphicsFamilyIndex, queueFamilyIndices.presentFamilyIndex }; //Needs to be out of the else scope
	//Setting the image sharing mode (either exclusive, or concurrent)
	if (queueFamilyIndices.graphicsFamilyIndex == queueFamilyIndices.presentFamilyIndex) {
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	else {
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchainCreateInfo.queueFamilyIndexCount = 2;
		swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndicesArray;
	}

	swapchainCreateInfo.preTransform = physicalDeviceSurfaceInfo.surfaceCapabilities.currentTransform;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = presentMode;
	swapchainCreateInfo.clipped = VK_TRUE; //If color of pixels behind windows are clipped or not
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE; //Since we don't have an old one (atleast one we still need to do things with)

	if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create swapchain");
	}

	//Setting swapchainwrapper attributes we need later
	vkGetSwapchainImagesKHR(device, swapchain, &minImageCount, nullptr);
	swapchainImages.resize(minImageCount);
	vkGetSwapchainImagesKHR(device, swapchain, &minImageCount, swapchainImages.data());

	swapchainImageFormat = surfaceFormat.format;
}

VkSurfaceFormatKHR SwapchainWrapper::findSwapchainSurfaceFormat(std::vector<VkSurfaceFormatKHR> const &availableFormats) {
	for (VkSurfaceFormatKHR const &availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	//Fallback using the first available format, this list isn't empty because we checked before using it
	return availableFormats[0];
}

VkPresentModeKHR SwapchainWrapper::findSwapchainPresentMode(std::vector<VkPresentModeKHR> const &availablePresentModes) {
	for (VkPresentModeKHR const &availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) { //We want to use mailbox mode, only if the device doesn't support it we use a fallback
			return availablePresentMode;
		}
	}

	//Fallback since this is always supported
	return VK_PRESENT_MODE_FIFO_KHR;
}

void SwapchainWrapper::setExtent(GLFWwindow *window, VkSurfaceCapabilitiesKHR const &capabilities) {
	if (capabilities.currentExtent.width != UINT32_MAX) { //If this value is not UINT32_MAX, the used window manager doesn't allow any difference between its resolution and the resolution of our swapchain
		extent = capabilities.currentExtent;
	}
	else { //We are allowed to set a different Resolution
		int width;
		int height;
		glfwGetFramebufferSize(window, &width, &height);

		extent.width = std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		extent.height = std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
	}
}

void SwapchainWrapper::createSwapchainImageViews(ImageCreator const &imageCreator) {
	swapchainImageViews.resize(swapchainImages.size());

	for (size_t i = 0; i < swapchainImages.size(); i++) {
		imageCreator.createImageView(swapchainImages[i], swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, swapchainImageViews[i]);
	}
}

void SwapchainWrapper::createDepthImage(ImageCreator const &imageCreator) {
	depthBufferFormat = imageCreator.findDepthFormat();
	imageCreator.createImage(extent.width, extent.height, depthBufferFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthBufferImage, depthBufferImageMemory);
	imageCreator.createImageView(depthBufferImage, depthBufferFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D, depthBufferImageView);
}

void SwapchainWrapper::createUniformBuffers(BufferCreator const &bufferCreator) {
	uniformBufferObjects.resize(swapchainImages.size());
	uniformBufferObjectsMemory.resize(swapchainImages.size());

	for (size_t i = 0; i < swapchainImages.size(); i++) {
		bufferCreator.createUniformBuffer(uniformBufferObjects[i], uniformBufferObjectsMemory[i]);
	}
}

void SwapchainWrapper::printAllAvailableSwapchainSurfaceFormats(std::vector<VkSurfaceFormatKHR> const &availableFormats) {
	std::cout << "Available swapchain surface formats: " << availableFormats.size() << std::endl;

	int index = 0;
	for (VkSurfaceFormatKHR const &availableFormat : availableFormats) {
		std::cout << "\t" << "Swapchain surface format " << index << ":" << std::endl;
		std::cout << "\t" << "\t" << "Format " << availableFormat.format << " and colorspace " << availableFormat.colorSpace << std::endl;
		index++;
	}
}

void SwapchainWrapper::printAllAvailableSwapchainPresentModes(std::vector<VkPresentModeKHR> const &availablePresentModes) {
	std::cout << "Available swapchain present modes: " << availablePresentModes.size() << std::endl;

	int index = 0;
	for (VkPresentModeKHR const &availablePresentMode : availablePresentModes) {
		std::cout << "\t" << "Swapchain present mode " << index << ":" << std::endl;
		std::cout << "\t" << "\t" << "Present mode: " << availablePresentMode << std::endl;
		index++;
	}
}

void SwapchainWrapper::printExtent() {
	std::cout << "Swapchain extent " << extent.width << " x " << extent.height << std::endl;
}
