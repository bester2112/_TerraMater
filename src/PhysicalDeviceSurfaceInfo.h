/**
 * @file PhysicalDeviceSurfaceInfo.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Contains a small helper struct to store relevant surface attributes for a given physical device and surface.
 * @version 0.1
 * @date 2020-05-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef PHYSICALDEVICESURFACEINFO_H
#define PHYSICALDEVICESURFACEINFO_H

#include "vulkan/vulkan.h"

#include <vector>

/**
 * @brief Contains relevant surface infos for a given physical device and surface.
 */
struct PhysicalDeviceSurfaceInfo {
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> surfaceFormats;
	std::vector<VkPresentModeKHR> presentModes;
};

#endif // !PHYSICALDEVICESURFACEINFO_H