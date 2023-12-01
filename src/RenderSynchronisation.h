/**
 * @file RenderSynchronisation.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Contains class with all needed synchronisation objects for rendering.
 * @version 0.1
 * @date 2020-05-05
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef RENDERSYNCHRONISATION_H
#define RENDERSYNCHRONISATION_H

#include "vulkan/vulkan.h"

#include <vector>

 /**
  * @brief This class contains all needed synchronisation objects needed for rendering.
  */
class RenderSynchronisation {
public:

	/**
	 * @brief Vector containing enough semaphores to check if an image is ready to become "in flight".
	 */
	std::vector<VkSemaphore> imageAvailableSemaphores;

	/**
	 * @brief Vector containing enough semaphores to check if an image is ready for presentation.
	 */
	std::vector<VkSemaphore> renderFinishedSemaphores;

	/**
	 * @brief Vector of fences to check if an InFlight spot is available.
	 */
	std::vector<VkFence> inFlightFences;

	/**
	 * @brief Vector with the size of swapchain image count used to check if a particular swapchain image can be used.
	 */
	std::vector<VkFence> imagesInFlight;

	RenderSynchronisation();

	/**
	 * @brief Constructs a new RenderSynchronisation object.
	 *
	 * Creates all necessary semaphores and fences.
	 *
	 * @param device Reference to the device, whis is need for some vulkan function calls.
	 * @param swapChainImageCount Count of the swapchain images needed for the fences array securing the swapchain images.
	 */
	RenderSynchronisation(VkDevice const &device, uint32_t const swapChainImageCount);

	~RenderSynchronisation();

	VkFence computeFence;

private:

	/**
	* @brief Reference to the device, whis is need for some vulkan function calls.
	*/
	VkDevice device;

	/**
	 * @brief Create a Render Synchronisation Objects object
	 *
	 * @param device
	 * @param swapChainImageCount
	 */
	void createRenderSynchronisationObjects(VkDevice const &device, uint32_t const swapChainImageCount);
};

#endif // !RENDERSYNCHRONISATION_H