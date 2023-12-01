/**
 * @file CommandWrapper.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Contains vulkan command(pool/buffer) wrapper class.
 * @version 0.1
 * @date 2020-05-05
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef COMMANDWRAPPER_H
#define COMMANDWRAPPER_H

#include "QueueFamilyIndices.h"
#include "Queues.h"

#include "vulkan/vulkan.h"

#include <vector>
#include <thread>
#include <condition_variable>

 /**
  * @brief Wrapper class which includes all needed vulkan command(pool/buffer) functionality.
  */
class CommandWrapper {
public:

	static std::mutex mutexCommandPool;

	static std::mutex mutexQueueSubmit;

	/**
	 * @brief Contains the command buffers for rendering.
	 */
	std::vector<VkCommandBuffer> commandBuffers;

	CommandWrapper();

	/**
	 * @brief Constructs a new command wrapper object.
	 *
	 * Creates the normal command pool and the transfer command pool.
	 *
	 * @param device Handle to the vulkan device, needed for the createCommandPool call.
	 * @param queueFamilyIndices Needed to determine to queueFamiliyIndex of the commandPool and the transferCommandPool.
	 * @param queues Needed for submitting the transfer command buffers.
	 */
	CommandWrapper(VkDevice const &device, QueueFamilyIndices const &queueFamilyIndices, Queues queues);

	~CommandWrapper();

	/**
	 * @brief Creates a transfer command buffer for one time use, and starts recording.
	 *
	 * @return VkCommandBuffer The created command buffer in which will be used for recording.
	 */
	VkCommandBuffer beginRecordingSingleUseTransferCommandBuffer() const;

	/**
	 * @brief Ends the recording for a given transfer command buffer, submits the commands, wait until the transfer is over and releases the buffer.
	 *
	 * @param commandBuffer Buffer wich will be submitted and released.
	 */
	void endRecordingSingleUseTransferCommandBuffer(VkCommandBuffer &commandBuffer);

	/**
	 * @brief Creates the command buffers for rendering.
	 *
	 * @param commandBufferCount Count of the command buffers to create.
	 */
	void createCommandBuffers(size_t const commandBufferCount);

	/**
	 * @brief Starts recording into the command buffer used for rendering.
	 *
	 * @param commandBufferIndex Index of the command buffer which should be recorded in.
	 * @param renderPass Renderpass used in the recording to start a render pass.
	 * @param framebuffer Framebuffer of the used swapchain image.
	 * @param extent Swapchain extent.
	 * @param pipeline Used pipeline for the rendering.
	 * @param pipelineLayout Layout of the used pipeline.
	 * @param descriptorSet Descriptor set to use for this render pass.
	 */
	void startRecordingCommandBuffer(size_t const commandBufferIndex, VkRenderPass const &renderPass, VkFramebuffer const &framebuffer, VkExtent2D const &extent, VkPipeline const &pipeline, VkPipelineLayout const &pipelineLayout, VkDescriptorSet const &descriptorSet);

	/**
	 * @brief Records the drawIndexed commands for a chunk with the given data.
	 *
	 * @param commandBufferIndex Index of the command buffer which should be recorded in.
	 * @param vertexBuffer Contains the vertices which are used in the draw command.
	 * @param indexBuffer Contains the indices for the triangle creation.
	 * @param indexCount Number of indices in the indexBuffer.
	 */
	void recordChunk(size_t const commandBufferIndex, VkBuffer const &vertexBuffer, VkBuffer const &indexBuffer, uint32_t const indexCount);

	void changeShader(size_t const commandBufferIndex, VkPipeline const &pipeline, VkPipelineLayout const &pipelineLayout, VkDescriptorSet const &descriptorSet);

	//void recordObjChunk();

	/**
	 * @brief Ends the recording in the specified commandBuffer.
	 *
	 * @param commandBufferIndex Index of the command buffer which should be recorded in.
	 */
	void endRecordingCommandBuffer(size_t const commandBufferIndex);


	/**
	* @brief Function to free the commandbuffers and reset the commandPool, with the free resources flag.
	*/
	void freeCommandBuffers();

	VkCommandBuffer computeCommandBuffer;

	void createComputeCommandBuffer();

	void recordComputeCommandBuffer(VkPipeline const &pipeline, VkPipelineLayout const &pipelineLayout, VkDescriptorSet const &descriptorSet);

	std::vector<VkCommandBuffer> quadCommandBuffers;

	void createQuadCommandBuffers(size_t const commandBufferCount);

	void recordQuadCommandBuffers(size_t const commandBufferIndex, VkRenderPass const &renderPass, VkFramebuffer const &framebuffer, VkExtent2D const &extent, VkPipeline const &pipeline, VkPipelineLayout const &pipelineLayout, VkDescriptorSet const &descriptorSet, VkImage const &image, VkBuffer const &vertexBuffer, VkBuffer const &indexBuffer);

private:

	/**
	 * @brief Reference to the device, whis is need for some vulkan function calls.
	 */
	VkDevice device;

	/**
	 * @brief Reference to the queues, needed for submitting the command buffers into the right queue.
	 */
	Queues queues;

	/**
	 * @brief Hanndle for the command pool.
	 */
	VkCommandPool commandPool;

	/**
	 * @brief Handle for the transfer command pool.
	 */
	VkCommandPool transferCommandPool;

	VkCommandPool computeCommandPool;

	VkCommandPool quadCommandPool;

	/**
	 * @brief Creates a command pool.
	 *
	 * @param queueFamilyIndex Specifies for which queue family the pool will be generated.
	 * @param commandPoolCreateFlags Specifies if the command pool should use some non default settings, like the transient bit.
	 * @param commandPool Handle of the created command pool.
	 */
	void createCommandPool(uint32_t const queueFamilyIndex, VkCommandPoolCreateFlags const commandPoolCreateFlags, VkCommandPool &commandPool);

	void createCommandBuffers(size_t const commandBufferCount, VkCommandPool const &commandPool, std::vector<VkCommandBuffer> &commandBuffers);
};

#endif // !COMMANDWRAPPER_H