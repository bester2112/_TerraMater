/**
 * @file RenderPassCreator.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief File contains a helper class to construct vulkan render pass objects.
 * @version 0.1
 * @date 2020-05-04
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef RENDERPASSCREATOR_H
#define RENDERPASSCREATOR_H

#include "vulkan/vulkan.h"

 /**
  * @brief Helper class to create vulkan render pass objects.
  */
class RenderPassCreator {
public:
	RenderPassCreator();
	~RenderPassCreator();

	/**
	 * @brief Creates a vulkan render pass object.
	 *
	 * @param device Handle of the vulkan device used to create the render pass.
	 * @param colorAttachmentFormat Format which should be used in the color attachment of the renderpass, should be the same as the swapchain image format.
	 * @param depthAttachmentFormat Format which should be used in the depth attachment of the renderpass, should be the same as the depth image format.
	 * @param renderPass Handle for the created renderpass.
	 */
	static void createRenderPass(VkDevice const &device, VkFormat const &colorAttachmentFormat, VkFormat const &depthAttachmentFormat, VkRenderPass &renderPass);
};

#endif // !RENDERPASSCREATOR_H
