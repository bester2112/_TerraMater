
#include "CommandWrapper.h"
#include "ComputeSettings.h"

#include <stdexcept>

std::mutex CommandWrapper::mutexCommandPool = std::mutex();
std::mutex CommandWrapper::mutexQueueSubmit = std::mutex();

CommandWrapper::CommandWrapper() {}

CommandWrapper::CommandWrapper(VkDevice const &device, QueueFamilyIndices const &queueFamilyIndices, Queues queues)
	: device(device), queues(queues) {
	createCommandPool(queueFamilyIndices.graphicsFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, commandPool);
	createCommandPool(queueFamilyIndices.transferFamilyIndex, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, transferCommandPool);
	createCommandPool(queueFamilyIndices.computeFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, computeCommandPool);
	createCommandPool(queueFamilyIndices.graphicsFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, quadCommandPool);
}

CommandWrapper::~CommandWrapper() {
	vkDestroyCommandPool(device, commandPool, nullptr);
	vkDestroyCommandPool(device, transferCommandPool, nullptr);
	vkDestroyCommandPool(device, computeCommandPool, nullptr);
	vkDestroyCommandPool(device, quadCommandPool, nullptr);
}

VkCommandBuffer CommandWrapper::beginRecordingSingleUseTransferCommandBuffer() const {

	//Creating the command buffer allocate info struct, which will be passed as argument to the allocateCommandBuffers call
	VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1;
	commandBufferAllocateInfo.commandPool = transferCommandPool;

	//Allocating the command buffer
	VkCommandBuffer thread_local commandBuffer;
	//VkCommandBuffer commandBuffer;

	{
		std::lock_guard<std::mutex> lockGuard(CommandWrapper::mutexCommandPool);
		vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
	}

	//Setting up the information needed to start recording into the command buffer
	VkCommandBufferBeginInfo commandBufferBeginInfo{};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	{
		std::lock_guard<std::mutex> lockGuard(CommandWrapper::mutexCommandPool);
		//Starts recording into the command buffer
		vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
	}

	return commandBuffer;
}

void CommandWrapper::endRecordingSingleUseTransferCommandBuffer(VkCommandBuffer &commandBuffer) {

	//Ends recording into the command buffer
	{
		std::lock_guard<std::mutex> lockGuard(CommandWrapper::mutexCommandPool);
		vkEndCommandBuffer(commandBuffer);
	}

	//Setting up the information to submit the recorded commands
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	{
		std::lock_guard<std::mutex> lockGuard(mutexQueueSubmit);
		vkQueueSubmit(queues.transferQueue, 1, &submitInfo, nullptr);
		vkQueueWaitIdle(queues.transferQueue);
	}
	//Submitting the commands to the transfer queue and waiting until the transfer is over
	//vkQueueSubmit(queues.transferQueue, 1, &submitInfo, nullptr);
	//vkQueueWaitIdle(queues.transferQueue);

	//Freeing the used command buffer
	{
		std::lock_guard<std::mutex> lockGuard(CommandWrapper::mutexCommandPool);
		vkFreeCommandBuffers(device, transferCommandPool, 1, &commandBuffer);
	}
}

void CommandWrapper::createCommandBuffers(size_t const commandBufferCount) {
	createCommandBuffers(commandBufferCount, commandPool, commandBuffers);
}

void CommandWrapper::startRecordingCommandBuffer(size_t const commandBufferIndex, VkRenderPass const &renderPass, VkFramebuffer const &framebuffer, VkExtent2D const &extent, VkPipeline const &pipeline, VkPipelineLayout const &pipelineLayout, VkDescriptorSet const &descriptorSet) {
	//Setting up the information needed to start recording into the command buffer
	VkCommandBufferBeginInfo commandBufferBeginInfo{};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	//Starts recording into the command buffer
	if (vkBeginCommandBuffer(commandBuffers[commandBufferIndex], &commandBufferBeginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer");
	}

	//Setting up the neccessary render pass info
	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.framebuffer = framebuffer;
	renderPassBeginInfo.renderArea.offset = { 0,0 };
	renderPassBeginInfo.renderArea.extent = extent;

	//Setting up a clear value
	VkClearValue clearValue[] = { { 0.0f, 0.0f, 0.0f, 1.0f }, {1.0f, 0} };
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValue;

	//Starts a new render pass
	vkCmdBeginRenderPass(commandBuffers[commandBufferIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffers[commandBufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	vkCmdBindDescriptorSets(commandBuffers[commandBufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
}

void CommandWrapper::recordChunk(size_t const commandBufferIndex, VkBuffer const &vertexBuffer, VkBuffer const &indexBuffer, uint32_t const indexCount) {
	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffers[commandBufferIndex], 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(commandBuffers[commandBufferIndex], indexBuffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(commandBuffers[commandBufferIndex], indexCount, 1, 0, 0, 0);
}

void CommandWrapper::changeShader(size_t const commandBufferIndex, VkPipeline const &pipeline, VkPipelineLayout const &pipelineLayout, VkDescriptorSet const &descriptorSet) {
	vkCmdBindPipeline(commandBuffers[commandBufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	vkCmdBindDescriptorSets(commandBuffers[commandBufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
}

void CommandWrapper::endRecordingCommandBuffer(size_t const commandBufferIndex) {
	vkCmdEndRenderPass(commandBuffers[commandBufferIndex]);

	//Ends recording into the command buffer
	if (vkEndCommandBuffer(commandBuffers[commandBufferIndex]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer");
	}
}

void CommandWrapper::freeCommandBuffers() {
	{
		//std::lock_guard<std::mutex> lockGuard(CommandWrapper::mutexCommandPool);
		vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		vkResetCommandPool(device, commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
	}
}

void CommandWrapper::createComputeCommandBuffer() {
	//Creating the command buffer allocate info struct, which will be passed as argument to the allocateCommandBuffers call
	VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = computeCommandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1;

	{
		std::lock_guard<std::mutex> lockGuard(CommandWrapper::mutexCommandPool);
		if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &computeCommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate compute command buffers");
		}
	}
}

void CommandWrapper::recordComputeCommandBuffer(VkPipeline const &pipeline, VkPipelineLayout const &pipelineLayout, VkDescriptorSet const &descriptorSet) {
	VkCommandBufferBeginInfo computeCommandBufferBeginInfo{};
	computeCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	{
		std::lock_guard<std::mutex> lockGuard(CommandWrapper::mutexCommandPool);
		//Starts recording into the compute command buffer
		vkBeginCommandBuffer(computeCommandBuffer, &computeCommandBufferBeginInfo);
	}

	vkCmdBindPipeline(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
	vkCmdBindDescriptorSets(computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, 0);

	vkCmdDispatch(computeCommandBuffer, ComputeSettings::COMPUTE_WIDTH / ComputeSettings::groupSizeX, ComputeSettings::COMPUTE_HEIGHT / ComputeSettings::groupSizeY, 1);

	vkEndCommandBuffer(computeCommandBuffer);
}

void CommandWrapper::createQuadCommandBuffers(size_t const commandBufferCount) {
	createCommandBuffers(commandBufferCount, quadCommandPool, quadCommandBuffers);
}

void CommandWrapper::recordQuadCommandBuffers(size_t const commandBufferIndex, VkRenderPass const &renderPass, VkFramebuffer const &framebuffer, VkExtent2D const &extent, VkPipeline const &pipeline, VkPipelineLayout const &pipelineLayout, VkDescriptorSet const &descriptorSet, VkImage const &image, VkBuffer const &vertexBuffer, VkBuffer const &indexBuffer) {
	//Setting up the information needed to start recording into the command buffer
	VkCommandBufferBeginInfo commandBufferBeginInfo{};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	//Starts recording into the command buffer
	if (vkBeginCommandBuffer(quadCommandBuffers[commandBufferIndex], &commandBufferBeginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording quad command buffer");
	}

	//Setting up the neccessary render pass info
	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.framebuffer = framebuffer;
	renderPassBeginInfo.renderArea.offset = { 0,0 };
	renderPassBeginInfo.renderArea.extent = extent;

	//Setting up a clear value
	VkClearValue clearValue[] = { { 0.0f, 0.0f, 0.0f, 1.0f }, {1.0f, 0} };
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValue;

	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(quadCommandBuffers[commandBufferIndex], VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

	//Starts a new render pass
	vkCmdBeginRenderPass(quadCommandBuffers[commandBufferIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(quadCommandBuffers[commandBufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	vkCmdBindDescriptorSets(quadCommandBuffers[commandBufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(quadCommandBuffers[commandBufferIndex], 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(quadCommandBuffers[commandBufferIndex], indexBuffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(quadCommandBuffers[commandBufferIndex], 6, 1, 0, 0, 0);

	vkCmdEndRenderPass(quadCommandBuffers[commandBufferIndex]);

	//Ends recording into the command buffer
	if (vkEndCommandBuffer(quadCommandBuffers[commandBufferIndex]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to record quad command buffer");
	}
}

void CommandWrapper::createCommandPool(uint32_t const queueFamilyIndex, VkCommandPoolCreateFlags const commandPoolCreateFlags, VkCommandPool &commandPool) {

	//Creating the command pool create info struct, which will be passed as argument to the createCommandPool call
	VkCommandPoolCreateInfo commandPoolCreateInfo{};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
	commandPoolCreateInfo.flags = commandPoolCreateFlags;

	if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command pool");
	}
}

void CommandWrapper::createCommandBuffers(size_t const commandBufferCount, VkCommandPool const &commandPool, std::vector<VkCommandBuffer> &commandBuffers) {
	commandBuffers.resize(commandBufferCount);

	//Creating the command buffer allocate info struct, which will be passed as argument to the allocateCommandBuffers call
	VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(commandBufferCount);

	{
		//std::lock_guard<std::mutex> lockGuard(CommandWrapper::mutexCommandPool);
		if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}
}
