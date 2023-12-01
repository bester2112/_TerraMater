#include "DeviceFinder.h"

#include <vector>
#include <set>
#include <iostream>

DeviceFinder::DeviceFinder() {}

DeviceFinder::~DeviceFinder() {}

bool DeviceFinder::findPhysicalDevice(VkInstance const instance, VkSurfaceKHR const &surface, VkPhysicalDevice &physicalDevice, QueueFamilyIndices &queueFamilyIndices) const {
	std::vector<VkPhysicalDevice> physicalDevices = listAllPhysicalDevices(instance);

	if (physicalDevices.size() == 0) {
		return false;
	}

	uint32_t bestRating = 0;

	//Calculates rating for every physical device to choose from, and sets the best one as the physical device to use
	for (VkPhysicalDevice const &physicalDeviceToRate : physicalDevices) {
		QueueFamilyIndices foundQueueFamilyIndices;
		uint32_t rating = ratePhysicalDevice(surface, physicalDeviceToRate, foundQueueFamilyIndices);

		if (rating > bestRating) {
			bestRating = rating;
			physicalDevice = physicalDeviceToRate;
			queueFamilyIndices = foundQueueFamilyIndices;
		}
	}

	if (bestRating > 0) {
		return true;
	}

	return false;
}

void DeviceFinder::createDevice(VkPhysicalDevice const &physicalDevice, QueueFamilyIndices const &queueFamilyIndices, VkDevice &device) const {
	std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
	std::set<uint32_t> uniqueQUeueFamilyIndices{ queueFamilyIndices.graphicsFamilyIndex, queueFamilyIndices.presentFamilyIndex, queueFamilyIndices.transferFamilyIndex };

	float queuePriority = 1.0f;

	//Generating the device queue create infos for each unique queue index (for example if graphics and present have the same index only device queue gets created)
	for (uint32_t const uniqueQUeueFamilyIndex : uniqueQUeueFamilyIndices) {
		VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
		deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueCreateInfo.queueFamilyIndex = uniqueQUeueFamilyIndex;
		deviceQueueCreateInfo.queueCount = 1;
		deviceQueueCreateInfo.pQueuePriorities = &queuePriority;

		deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
	}

	//Setting up the used physical device features
	VkPhysicalDeviceFeatures physicalDeviceFeatures{};
	physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;

	//Creating the device create info struct, used to generate the device
	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensionNames.size());
	deviceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();

	if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create device");
	}
}

void DeviceFinder::printAllPhysicalDevices(VkInstance const instance) const {
	std::vector<VkPhysicalDevice> physicalDevices = listAllPhysicalDevices(instance);

	VkPhysicalDeviceProperties physicalDeviceProperties;

	std::cout << "Physical devices:" << std::endl;
	for (VkPhysicalDevice const &physicalDevice : physicalDevices) {
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

		std::cout << "\t" << physicalDeviceProperties.deviceName << std::endl;
	}
}

std::vector<VkPhysicalDevice> DeviceFinder::listAllPhysicalDevices(VkInstance const instance) const {
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

	return physicalDevices;
}

uint32_t DeviceFinder::ratePhysicalDevice(VkSurfaceKHR const &surface, VkPhysicalDevice const &physicalDevice, QueueFamilyIndices &queueFamilyIndices) const {
	//Different attributes of the physical device can add to the score, the scheme of the point distribution should provide discrete gpu's a better rating over integrated ones.
	uint32_t rating = 0;

	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

	//Rating for device properties
	if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		rating += 10000;
	}

	rating += physicalDeviceProperties.limits.maxImageDimension2D; //If multiple physical device are available, this gives the one with the biggest supported image size a bonus

	//Rating for device features
	if (!physicalDeviceFeatures.geometryShader) {
		return 0;
	}

	if (!physicalDeviceFeatures.samplerAnisotropy) {
		return 0;
	}

	//Rating for supported extensions
	if (!checkForEnabledExtensions(physicalDevice)) {
		return 0;
	}

	//Rating for support of needed queue families
	queueFamilyIndices = findQueueFamilyIndices(surface, physicalDevice);
	if (!queueFamilyIndices.hasAllIndices()) {
		return 0;
	}

	//Rating for the surface capabilites
	//No need to check at this point if surface related KHR extension are available, since we did this already 6 lines above
	PhysicalDeviceSurfaceInfo physicalDeviceSurfaceInfo = getPhysicalDeviceSurfaceInfo(physicalDevice, surface);
	if (physicalDeviceSurfaceInfo.surfaceFormats.empty() || physicalDeviceSurfaceInfo.presentModes.empty()) {
		return 0;
	}

	return rating;
}

bool DeviceFinder::checkForEnabledExtensions(VkPhysicalDevice const &physicalDevice) const {
	uint32_t availableExtensionsCount = 0;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionsCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &availableExtensionsCount, availableExtensions.data());

	uint32_t foundExtensionsCount = 0;

	//Loops through both the enabled extensions and the available extensions, if an enabled extension is available it is being counted as found
	for (std::string const &enabledExtensionName : enabledExtensionNames) {
		for (VkExtensionProperties const &availableExtension : availableExtensions) {
			if (enabledExtensionName.compare(availableExtension.extensionName) == 0) {
				foundExtensionsCount++;
			}
		}
	}

	if (foundExtensionsCount == enabledExtensionNames.size()) {
		return true;
	}

	return false;
}

QueueFamilyIndices DeviceFinder::findQueueFamilyIndices(VkSurfaceKHR const &surface, VkPhysicalDevice const &physicalDevice) const {
	QueueFamilyIndices queueFamilyIndices;

	uint32_t queueFamilyPropertyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyPropertyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilies.data());

#ifdef _DEBUG
	//Debug utility printing out all queue families
	std::cout << "Number of queue families: " << queueFamilies.size() << std::endl;
#endif // _DEBUG

	uint32_t index = 0;

	for (VkQueueFamilyProperties const &queueFamily : queueFamilies) {

#ifdef _DEBUG
		//Debug utility printing out all queue families
		std::cout << "\t" << "Queue family " << index << ":" << std::endl;
		std::cout << "\t" << "\t" << "queue count: " << queueFamily.queueCount << std::endl;
		std::cout << "\t" << "\t" << "queue flags:" << queueFamily.queueFlags << std::endl;
#endif // _DEBUG

		//Setting graphics family index
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queueFamilyIndices.graphicsFamilyIndex = index;
			queueFamilyIndices.hasGraphicsFamily = true;
		}

		//Setting present family index
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, surface, &presentSupport);
		if (presentSupport) {
			queueFamilyIndices.presentFamilyIndex = index;
			queueFamilyIndices.hasPresentFamily = true;
		}

		//Setting transfer family index
		if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT && !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
			queueFamilyIndices.transferFamilyIndex = index;
			queueFamilyIndices.hasTransferFamily = true;
		}

		//Setting compute family index
		if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			queueFamilyIndices.computeFamilyIndex = index;
			queueFamilyIndices.hasComputeFamily = true;
		}

		if (queueFamilyIndices.hasAllIndices()) {
			break;
		}

		index++;
	}

	return queueFamilyIndices;
}

PhysicalDeviceSurfaceInfo DeviceFinder::getPhysicalDeviceSurfaceInfo(VkPhysicalDevice const &physicalDevice, VkSurfaceKHR const &surface) const {
	PhysicalDeviceSurfaceInfo physicalDeviceSurfaceInfo;

	//Storing the surface capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &physicalDeviceSurfaceInfo.surfaceCapabilities);

	//Storing the supported formats
	uint32_t surfaceFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);

	if (surfaceFormatCount != 0) {
		physicalDeviceSurfaceInfo.surfaceFormats.resize(surfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, physicalDeviceSurfaceInfo.surfaceFormats.data());
	}

	//Storing the supported present modes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		physicalDeviceSurfaceInfo.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, physicalDeviceSurfaceInfo.presentModes.data());
	}

	return physicalDeviceSurfaceInfo;
}
