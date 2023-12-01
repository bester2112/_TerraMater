#include "PipelineCreator.h"
#include "Shader.h"
#include "Settings.h"
#include "Vertex.h"
#include "BigVertex.h"

#include <stdexcept>

PipelineCreator::PipelineCreator() {}

PipelineCreator::~PipelineCreator() {}

void PipelineCreator::createPipeLine(VkDevice const &device, VkExtent2D const &extent, VkDescriptorSetLayout const &descriptorSetLayout, VkRenderPass const &renderPass, char const *vertexShaderPath, char const *fragmentShaderPath, bool const wireframe, bool const bigVertex, VkPipelineLayout &pipelineLayout, VkPipeline &pipeline) {
	//Loading the shaders and creating there shader modules
	Shader vertexShader = Shader(device, vertexShaderPath);
	Shader fragmentShader = Shader(device, fragmentShaderPath);

	//Creating the vertex shader stage create info struct, which is needed for the pipeline create info
	VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo{};
	vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexShaderStageCreateInfo.module = vertexShader.shaderModule;
	vertexShaderStageCreateInfo.pName = "main";

	//Creating the fragment shader stage create info struct, which is needed for the pipeline create info
	VkPipelineShaderStageCreateInfo fragmentShaderStagerCreateInfo{};
	fragmentShaderStagerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderStagerCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentShaderStagerCreateInfo.module = fragmentShader.shaderModule;
	fragmentShaderStagerCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfos[] = { vertexShaderStageCreateInfo , fragmentShaderStagerCreateInfo };

	//Creating the vertex input state create info struct, which is needed for the pipeline create info
	VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
	pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	if (!bigVertex) {
		VkVertexInputBindingDescription vertexInputBindingDescription = Vertex::getVertexInputBindingDescription();
		pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
		pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;

		std::array<VkVertexInputAttributeDescription, 6> vertexInputAttributeDescriptions = Vertex::getVertexInputAttributeDescription();
		pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescriptions.size());
		pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();
	} else {
		VkVertexInputBindingDescription vertexInputBindingDescription = BigVertex::getVertexInputBindingDescription();
		pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
		pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;

		std::array<VkVertexInputAttributeDescription, 4> vertexInputAttributeDescriptions = BigVertex::getVertexInputAttributeDescription();
		pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescriptions.size());
		pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();
	}

	//Creating the input assembly state create info struct, which is needed for the pipeline create info
	VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
	pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	//Setting up the viewport
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)extent.width;
	viewport.height = (float)extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	//Setting up the scissor
	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = extent;

	//Creating the viewport state create info struct, which is needed for the pipeline create info
	//Combines viewport and scissor, multiple instances can be used (maybe useful for an debug view for culling later)
	VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
	pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	pipelineViewportStateCreateInfo.viewportCount = 1;
	pipelineViewportStateCreateInfo.pViewports = &viewport;
	pipelineViewportStateCreateInfo.scissorCount = 1;
	pipelineViewportStateCreateInfo.pScissors = &scissor;

	//Creating the rasterization state create info struct, which is needed for the pipeline create info
	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
	pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE; //Discard Fragments outside of the clipping plane, instead of clamping them onto the clipping plane
	pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE; //If enabled fragments would be discarded and nothing would be stored to the framebuffer

	if (wireframe) {
		pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_LINE;
	}
	else {
		pipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	}

	pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
	pipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	pipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; //Using counter clockwise because projection matrix will multiply with -1, because vulkan flips the y axis in comparision to OpenGL
	pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;

	//Creating the multisample state create info struct, which is needed for the pipeline create info
	VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
	pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	//Creating the depth & stencil state create info struct, which is needed for the pipeline create info
	VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};
	pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
	pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	pipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;

	//Setting up the color blend attachment needed for the color blending create info
	VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
	pipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;

	//Creating the color blending state create info struct, which is needed for the pipeline create info
	VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
	pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pipelineColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	pipelineColorBlendStateCreateInfo.attachmentCount = 1;
	pipelineColorBlendStateCreateInfo.pAttachments = &pipelineColorBlendAttachmentState;

	//Creating the pipeline layout create info struct, which will be passed as argument to the createPipelineLayout call
	//Used for the layout of the uniforms in the shaders
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

	if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline layout");
	}

	//Creating the graphics pipeline create info struct, which will be passed as argument to the cCreateGraphicsPipelines call
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.stageCount = 2;
	graphicsPipelineCreateInfo.pStages = pipelineShaderStageCreateInfos;
	graphicsPipelineCreateInfo.pVertexInputState = &pipelineVertexInputStateCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo;
	graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
	graphicsPipelineCreateInfo.pRasterizationState = &pipelineRasterizationStateCreateInfo;
	graphicsPipelineCreateInfo.pMultisampleState = &pipelineMultisampleStateCreateInfo;
	graphicsPipelineCreateInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
	graphicsPipelineCreateInfo.pColorBlendState = &pipelineColorBlendStateCreateInfo;
	graphicsPipelineCreateInfo.pDynamicState = nullptr;
	graphicsPipelineCreateInfo.layout = pipelineLayout;
	graphicsPipelineCreateInfo.renderPass = renderPass;
	graphicsPipelineCreateInfo.subpass = 0;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline");
	}
}

void PipelineCreator::createComputePipeLine(VkDevice const &device, VkDescriptorSetLayout const &descriptorSetLayout, char const *computeShaderPath, VkPipelineLayout &computePipelineLayout, VkPipeline &computePipeline) {
	Shader computeShader = Shader(device, computeShaderPath);

	//Creating the compute shader stage create info struct, which is needed for the compute pipeline create info
	VkPipelineShaderStageCreateInfo computeShaderStageCreateInfo{};
	computeShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	computeShaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	computeShaderStageCreateInfo.module = computeShader.shaderModule;
	computeShaderStageCreateInfo.pName = "main";
	
	//Creating the pipeline layout create info struct, which will be passed as argument to the createPipelineLayout call
	//Used for the layout of the uniforms in the shaders
	VkPipelineLayoutCreateInfo computePipelineLayoutCreateInfo{};
	computePipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	computePipelineLayoutCreateInfo.setLayoutCount = 1;
	computePipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;

	if (vkCreatePipelineLayout(device, &computePipelineLayoutCreateInfo, nullptr, &computePipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to compute create pipeline layout");
	}

	VkComputePipelineCreateInfo computePipelineCreateInfo{};
	computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	computePipelineCreateInfo.layout = computePipelineLayout;
	computePipelineCreateInfo.stage = computeShaderStageCreateInfo;

	if (vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &computePipeline) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create compute pipeline");
	}
}
