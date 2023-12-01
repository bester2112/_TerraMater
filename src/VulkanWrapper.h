/**
 * @file VulkanWrapper.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Includes the VulkanWrapper class, which combines all Vulkan parts to render a frame.
 * @version 0.1
 * @date 2020-05-03
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef VULKANWRAPPER_H
#define VULKANWRAPPER_H

#include "QueueFamilyIndices.h"
#include "Queues.h"
#include "CommandWrapper.h"
#include "BufferCreator.h"
#include "ImageCreator.h"
#include "SwapchainWrapper.h"
#include "DescriptorWrapper.h"
#include "TextureArray.h"
#include "RenderSynchronisation.h"
#include "ComputeWrapper.h"
#include "SkyBox.h"
#include "SkyWrapper.h"

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include <vector>

 /**
  * @brief Handles all Vulkan related things and has the ability to render a frame with the render function.
  */
class VulkanWrapper {
public:

	/**
	* @brief Used to indicate, that the glfw window was resized.
	*/
	bool frameBufferResized = false;

	VulkanWrapper();
	~VulkanWrapper();

	/**
	 * @brief Initializes all components which are needed to use Vulkan.
	 *
	 * @param window Handle to the used GLFW window, will be stored as a private reference so it shouldn't change as long as the VulkanWrapper is used.
	 */
	void initialize(GLFWwindow *window);

	/**
	 * @brief Creates vulkan objects for a given data of a chunk.
	 *
	 * @param vertices Vertices of the chunk.
	 * @param indices Indices of the chunk.
	 * @param vertexBuffer Handle in which the generated vertex buffer will be stored.
	 * @param vertexBufferMemory Handle in which the generated vertex buffer memory will be stored.
	 * @param indexBuffer Handle in which the generated index buffer will be stored.
	 * @param indexBufferMemory Hanlde in which the generated index buffer memory will be stored.
	 */
	void createVulkanLoadedChunk(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, VkBuffer &vertexBuffer, VkDeviceMemory &vertexBufferMemory, VkBuffer &indexBuffer, VkDeviceMemory &indexBufferMemory);

	void createVulkanObjChunk(std::vector<BigVertex> &vertices, std::vector<uint32_t> &indices, VkBuffer &vertexBuffer, VkDeviceMemory &vertexBufferMemory, VkBuffer &indexBuffer, VkDeviceMemory &indexBufferMemory);

	/**
	 * @brief Calls the vulkan destroy functions for the vulkan objects of a chunk.
	 *
	 * @param vertexBuffer The vertex buffer, which should be destroyed.
	 * @param vertexBufferMemory The vertex buffer memory, which should be freed.
	 * @param indexBuffer The index buffer, which should be destroyed.
	 * @param indexBufferMemory The index buffer memory, which should be freed.
	 */
	void deleteVulkanLoadedChunk(VkBuffer &vertexBuffer, VkDeviceMemory &vertexBufferMemory, VkBuffer &indexBuffer, VkDeviceMemory &indexBufferMemory);

	/**
	 * @brief Tries to acquire an image and sets up everything to record render commands.
	 *
	 * @param view View transformation matrix.
	 * @param projection Projection transformation matrix.
	 * @param imageIndex Variable in which the index of the acquired image will be stored.
	 * @return true If an image could be acquired.
	 * @return false If no image could be acquired.
	 */
	bool startRenderRecording(glm::mat4 const &view, glm::mat4 const &projection, uint32_t &imageIndex);

	/**
	 * @brief Adds the data of a given chunk to be rendered.
	 *
	 * @param imageIndex Index of the image into which the chunk should be rendered.
	 * @param vertexBuffer Vertex buffer containing the vertices of the chunk.
	 * @param indexBuffer Index buffer containing the indices of the chunk.
	 * @param indexCount Count of the indices.
	 */
	void addChunkToRender(uint32_t const imageIndex, VkBuffer vertexBuffer, VkBuffer indexBuffer, uint32_t const indexCount);

	void changeToObjPipeline(uint32_t const imageIndex);

	void addObjChunkToRender(uint32_t const imageIndex, VkBuffer vertexBuffer, VkBuffer indexBuffer, uint32_t const indexCount);

	void changeToSkyPipeline(uint32_t const imageIndex);

	void addSkyToRender(uint32_t const imageIndex);

	/**
	 * @brief Enqueues the rendering of given image index.
	 *
	 * @param imageIndex Index of the image which will be submitted to be rendered.
	 */
	void SubmitRender(uint32_t const imageIndex);

	void renderPhotomode(glm::vec3 const &cameraPosition, glm::vec3 const &cameraDirection, glm::vec3 const &up, glm::vec4 const &iData);

	/**
	 * @brief Waits until the vulkan device is idle.
	 */
	void waitForDeviceIdle();

	/**
	 * @brief Callback function, called if glfw window was resized.
	 *
	 * @param window Instance of the glfw window, which was resized.
	 * @param width New width of the window.
	 * @param height New height of the window.
	 */
	static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

	void loadComputeBoxes(std::vector<BoxData> const &boxData, std::vector<BoxData> const &emissiveBoxData, std::vector<PointLight> const &pointLights);

private:

	/**
	 * @brief Handle for the GLFWwindow object, used for surface creation.
	 */
	GLFWwindow *window;

	/**
	* @brief Handle of the vulkan instance.
	*/
	VkInstance instance;

	/**
	* @brief Handle of the vulkan surface.
	*/
	VkSurfaceKHR surface;

	/**
	* @brief Handle for the physical device.
	*/
	VkPhysicalDevice physicalDevice;

	/**
	* @brief Handle for the logical device.
	*/
	VkDevice device;

	/**
	* @brief Stores the indices of the queue families to use.
	*/
	QueueFamilyIndices queueFamilyIndices;

	/**
	* @brief Struct which stores handles for all queues.
	*/
	Queues queues;

	/**
	* @brief Contains the vulkan command pools and all needed functions to control them.
	*/
	CommandWrapper *commandWrapper;

	/**
	* @brief Bundles all vulkan buffer creation.
	*/
	BufferCreator bufferCreator;

	/**
	* @brief Bundles all vulkan image creation.
	*/
	ImageCreator imageCreator;

	/**
	* @brief Contains the vulkan swapchain and all needed swapchain attributes.
	*/
	SwapchainWrapper *swapchainWrapper;

	/**
	* @brief Handle for the used renderpass.
	*/
	VkRenderPass renderPass;

	/**
	* @brief Contains the vulkan descriptor objects, such as the layout, pool, etc.
	*/
	DescriptorWrapper *descriptorWrapper;

	/**
	* @brief Handle for the graphics pipeline layout. (Contains information about the structure of the used uniforms)
	*/
	VkPipelineLayout pipelineLayout;

	/**
	* @brief Handle for the graphics pipeline.
	*/
	VkPipeline pipeline;

	VkPipelineLayout objPipelineLayout;
	VkPipeline objPipeline;

	VkPipeline skyPipeline;
	VkPipelineLayout skyPipelineLayout;

	/**
	* @brief Current frame, needed to get the right semaphore pairs from the synchronisation object.
	*/
	size_t currentFrame = 0;

	/**
	* @brief Contains all needed semaphores and fences to synchronize the rendering.
	*/
	RenderSynchronisation *renderSynchronisation;

	/**
	* @brief Creates the vulkan instance.
	*/
	void createInstance();

	/**
	 * @brief Returns an Vector cointaining the names of the required vulkan extensions for GLFW.
	 *
	 * @return std::vector<char const *> Contains the names of the required vulkan extensions.
	 */
	std::vector<char const *> getGLFWRequiredInstanceExtensions();

	/**
	* @brief Creates the vulkan surfaces handle.
	*/
	void createSurface();

	/**
	* @brief Sets the physical device and the corresponding queue family indices for the given physical device.
	*/
	void setPhysicalDevice();

	/**
	* @brief Creates the logical device.
	*/
	void createDevice();

	/**
	* @brief Gets the queues of the current device and stores them in the queues struct.
	*/
	void getQueues();

	/**
	* @brief Creates the command wrapper.
	*/
	void createCommandWrapper();

	/**
	* @brief Creates the buffer creator.
	*/
	void createBufferCreator();

	/**
	* @brief Creates the image creator.
	*/
	void createImageCreator();

	/**
	* @brief Creates the swapchainWrapper, which creates the swapchain and all neccessary swapchain attributes.
	*/
	void createSwapchainWrapper();

	/**
	* @brief Creates the render pass, which will later then be used as "layout" inside the commandbuffers.
	*/
	void createRenderPass();

	/**
	* @brief creates the descriptorWrapper and the descriptorSetLayout.
	*/
	void createDescriptorWrapper();

	/**
	* @brief Creates the graphics pipeline and the pipelinelayout.
	*/
	void createPipeline();

	/**
	* @brief Creates all neccessary synchronisation objects inside the renderSynchronisation object.
	*/
	void createRenderSynchronisation();

	/**
	 * @brief Updates an uniform buffer object corresponding to the given imageIndex.
	 *
	 * @param imageIndex Image index, to which the uniform buffer is belonging.
	 * @param model Model transformation matrix.
	 * @param view View transformation matrix.
	 * @param projection Projection transformation matrix.
	 */
	void updateUniformBufferObject(uint32_t const imageIndex, glm::mat4 const &model, glm::mat4 const &view, glm::mat4 const &projection);

	void updateSkyUniformBufferObject(uint32_t const imageIndex);

	/**
	 * @brief Destroys the used SwapchainWrapper and cleansUp all things depending on the swapchain attributes.
	 *
	 */
	void cleanUpSwapchain();

	/**
	 * @brief Creates a new SwapchainWrapper with updated attributes, also recreates everything which depends on the swapchain attributes.
	 */
	void recreateSwapchain();

	/**
	 * @brief Handle to our Texture.
	 */
	TextureArray *textureArray;

	SkyBox *skyBox;

	SkyWrapper *skyWrapper;

	ComputeWrapper *computeWrapper;
	void createComputeWrapper(char const *computeShaderPath);

	//Debug functions

	/**
	 * @brief Debug callback function used for the validation layers.
	 */
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT *pCallBackData,
		void *pUserData);

	/**
	 * @brief Prints out all available instance extensions.
	 */
	void printAllAvailableInstanceExtensions();
};

#endif // !VULKANWRAPPER_H
