/**
 * @file SwapchainWrapper.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Contains a wrapper class for all vulkan swapchain things.
 * @version 0.1
 * @date 2020-05-04
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef SWAPCHAINWRAPPER_H
#define SWAPCHAINWRAPPER_H

#include "PhysicalDeviceSurfaceInfo.h"
#include "QueueFamilyIndices.h"
#include "ImageCreator.h"
#include "BufferCreator.h"
#include "UniformBufferObject.h"

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include <vector>

 /**
 * @brief Contains the vulkan swapchain and all neccessary types, which are needed to actually use the swapchain (like imageViews).
 */
class SwapchainWrapper {
public:
	/**
	* @brief Reference to the device, whis is need for some vulkan function calls.
	*/
	VkDevice device;

	/**
	* @brief Handle for the swapchain object.
	*/
	VkSwapchainKHR swapchain;

	/**
	* @brief Stores the dimensions of the swapchain.
	*/
	VkExtent2D extent;

	/**
	* @brief Used format for the swapchain images.
	*/
	VkFormat swapchainImageFormat;

	/**
	* @brief List of swapchain images used for rendering.
	*/
	std::vector<VkImage> swapchainImages;

	/**
	* @brief Handles for the swapchain image views.
	*/
	std::vector<VkImageView> swapchainImageViews;

	/**
	* @brief Vector containing the uniform buffer objects, for each image in the swapchain it contains one unique uniform buffer object.
	*/
	std::vector<VkBuffer> uniformBufferObjects;

	/**
	* @brief Vector for the DeviceMemory handles of the uniform buffer objects memory.
	*/
	std::vector<VkDeviceMemory> uniformBufferObjectsMemory;

	/**
	* @brief Handles for the swapchain framebuffers.
	*/
	std::vector<VkFramebuffer> swapchainFramebuffers;

	/**
	* @brief Handle of the depth buffer image.
	*/
	VkImage depthBufferImage;

	/**
	* @brief Handle of the depth buffer image view.
	*/
	VkImageView depthBufferImageView;

	/**
	* @brief Handle of the depth buffer image memory.
	*/
	VkDeviceMemory depthBufferImageMemory;

	/**
	* @brief Handle of the depth buffer.
	*/
	VkFormat depthBufferFormat;

	SwapchainWrapper();

	/**
	 * @brief Constructs a new swapchain wrapper object.
	 *
	 * Calls the create functions for the swapchain and depeding attributes.
	 *
	 * @param window Handle to the GLFW window, used to determine the size of the swapchain images.
	 * @param surface Handle to the vulkan surface, this surface is going to be used by the swapchain.
	 * @param device Handle to the vulkan device, needed for the createSwapchain call.
	 * @param physicalDeviceSurfaceInfo Contains surface information regarding the physical device and the surface.
	 * @param queueFamilyIndices Queue family indices needed to determine the access scheme of the swapchain images.
	 * @param imageCreator Used to create the swapchain image views.
	 * @param bufferCreator Used to create the uniform buffers.
	 */
	SwapchainWrapper(GLFWwindow *window, VkSurfaceKHR const &surface, VkDevice &device, PhysicalDeviceSurfaceInfo const &physicalDeviceSurfaceInfo, QueueFamilyIndices const &queueFamilyIndices, ImageCreator const &imageCreator, BufferCreator const &bufferCreator);

	~SwapchainWrapper();

	/**
	 * @brief Creates the framebuffers for the swapchain.
	 *
	 * @param renderPass Specifies the used renderpass.
	 */
	void createSwapchainFramebuffers(VkRenderPass const &renderPass);

private:
	/**
	 * @brief Creates the swapchain and sets the swapchainImageFormat and swapchainImages.
	 *
	 * @param window Handle to the GLFW window, used to determine the size of the swapchain images.
	 * @param surface Handle to the vulkan surface, this surface is going to be used by the swapchain.
	 * @param physicalDeviceSurfaceInfo Contains surface information regarding the physical device and the surface.
	 * @param queueFamilyIndices  Queue family indices needed to determine the access scheme of the swapchain images.
	 */
	void createSwapchain(GLFWwindow *window, VkSurfaceKHR const &surface, PhysicalDeviceSurfaceInfo const &physicalDeviceSurfaceInfo, QueueFamilyIndices const &queueFamilyIndices);

	/**
	 * @brief Finds the best available swapchain format, given a list of available formats.
	 *
	 * @param availableFormats Vector containing the available swap chain formats.
	 * @return VkSurfaceFormatKHR The best available swapchain format.
	 */
	VkSurfaceFormatKHR findSwapchainSurfaceFormat(std::vector<VkSurfaceFormatKHR> const &availableFormats);

	/**
	 * @brief Finds the best available swapchain present mode, given a list of available present modes.
	 *
	 * @param availablePresentModes Vector containing the available swap chain present modes.
	 * @return VkPresentModeKHR The best available swapchain presen mode.
	 */
	VkPresentModeKHR findSwapchainPresentMode(std::vector<VkPresentModeKHR> const &availablePresentModes);

	/**
	 * @brief Sets the swapchain extent depending on the restrictions given by the window manager.
	 *
	 * @param window Handle of the GLFW window, used to determine swapchain extend dimensions.
	 * @param capabilities Capabilities are used to check if differing from the current extend is allowed.
	 */
	void setExtent(GLFWwindow *window, VkSurfaceCapabilitiesKHR const &capabilities);

	/**
	* @brief Creates the handles for the swapchain image views.
	*
	* @param imageCreator Reference to the image creator which will be used to create the swapchain image views.
	*/
	void createSwapchainImageViews(ImageCreator const &imageCreator);

	/**
	* @brief Creates the handles for the depth image attributes.
	*
	* @param imageCreator Reference to the image creator which will be used to create the depth image attributes.
	*/
	void createDepthImage(ImageCreator const &imageCreator);

	/**
	 * @brief Creates the uniform buffer objects, for each image in the swap chain one uniform buffer objects gets created.
	 *
	 * @param bufferCreator Reference to the buffer creator which will be used to create the uniform buffers.
	 */
	void createUniformBuffers(BufferCreator const &bufferCreator);

	//Debug functions

	/**
	 * @brief Prints out all available surface formats.
	 *
	 * @param availableFormats Vector containing the available swap chain formats.
	 */
	void printAllAvailableSwapchainSurfaceFormats(std::vector<VkSurfaceFormatKHR> const &availableFormats);

	/**
	 * @brief Prints out all available present modes.
	 *
	 * @param availablePresentModes Vector containing the available swap chain present modes.
	 */
	void printAllAvailableSwapchainPresentModes(std::vector<VkPresentModeKHR> const &availablePresentModes);

	/**
	* @brief Prints out the set swapchain extent.
	*/
	void printExtent();
};

#endif // !SWAPCHAINWRAPPER_H