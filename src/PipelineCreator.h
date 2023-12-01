/**
 * @file PipelineCreator.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Contains a Helper class, to create a vulkan graphics pipeline.
 * @version 0.1
 * @date 2020-05-04
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef PIPELINECREATOR_H
#define PIPELINECREATOR_H

#include "vulkan/vulkan.h"

 /**
  * @brief Helper class to create a vulkan graphics pipeline.
  */
class PipelineCreator {
public:
	PipelineCreator();
	~PipelineCreator();

	/**
	 * @brief Creates a vulkan graphics pipeline.
	 *
	 * @param device Handle to the vulkan device, needed for the createGraphicsPipeline call.
	 * @param extent Extent of the swapchain needed for viewport and scissor creation.
	 * @param descriptorSetLayout Descriptor set layout used for pipelineLayout creation.
	 * @param renderPass Renderpass needed for pipeline creation.
	 * @param wireframe If the rasterizer should "draw" in wireframe moder or not
	 * @param pipelineLayout Handle of the created pipeline layout.
	 * @param pipeline Handle of the created pipeline.
	 */
	static void createPipeLine(VkDevice const &device, VkExtent2D const &extent, VkDescriptorSetLayout const &descriptorSetLayout, VkRenderPass const &renderPass, char const *vertexShaderPath, char const *fragmentShaderPath, bool const wireframe, bool const bigVertex, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline);

	static void createComputePipeLine(VkDevice const &device, VkDescriptorSetLayout const &descriptorSetLayout, char const *computeShaderPath, VkPipelineLayout &computePipelineLayout, VkPipeline &computePipeline);
};

#endif // !PIPELINECREATOR_H