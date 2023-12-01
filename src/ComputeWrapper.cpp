#include "ComputeWrapper.h"
#include "ComputeSettings.h"
#include "Vertex.h"
#include "PipelineCreator.h"
#include "CameraData.h"

#include <chrono>
#include <vector>
#include <iostream>

ComputeWrapper::ComputeWrapper(VkDevice &device, ImageCreator const &imageCreator, BufferCreator const &bufferCreator, VkExtent2D const extent, size_t const bufferCount, VkDescriptorSetLayout const &quadDescriptorSetLayout, VkRenderPass const &renderPass, VkDescriptorSetLayout const &computeDescriptorSetLayout, char const *computeShaderPath)
	: device(device), bufferCreator(bufferCreator) {
	createQuad();
	createTexture(imageCreator);
	createCameraData(bufferCount);
	createQuadPipeline(extent, quadDescriptorSetLayout, renderPass);
	createComputePipeline(computeDescriptorSetLayout, computeShaderPath);
	createWriteBackDataBuffer();
}

ComputeWrapper::~ComputeWrapper() {
	vkDestroyPipeline(device, quadPipeline, nullptr);
	vkDestroyPipeline(device, computePipeline, nullptr);

	vkDestroyPipelineLayout(device, quadPipelineLayout, nullptr);
	vkDestroyPipelineLayout(device, computePipelineLayout, nullptr);

	vkDestroySampler(device, computeTextureSampler, nullptr);
	vkDestroyImageView(device, computeTextureImageView, nullptr);
	vkDestroyImage(device, computeTextureImage, nullptr);
	vkFreeMemory(device, computeTextureImageMemory, nullptr);

	vkDestroyBuffer(device, quadVertices, nullptr);
	vkFreeMemory(device, quadVerticesMemory, nullptr);

	vkDestroyBuffer(device, quadIndexBuffer, nullptr);
	vkFreeMemory(device, quadIndexBufferMemory, nullptr);

	for (size_t i = 0; i < cameraDataBuffers.size(); i++) {
		vkDestroyBuffer(device, cameraDataBuffers[i], nullptr);
		vkFreeMemory(device, cameraDataBuffersMemory[i], nullptr);
	}

	vkDestroyBuffer(device, writeBackDataBuffer, nullptr);
	vkFreeMemory(device, writeBackDataBufferMemory, nullptr);
}

void ComputeWrapper::storeAndResetWriteBackData() {
	WriteBackData resetWriteBackBuffer = {glm::ivec4(0)};

	void *data;
	vkMapMemory(device, writeBackDataBufferMemory, 0, sizeof(WriteBackData), 0, &data);

	//Store
	memcpy(&ComputeSettings::writeBackData, data, sizeof(WriteBackData));

	//Reset
	memcpy(data, &resetWriteBackBuffer, sizeof(WriteBackData));


	vkUnmapMemory(device, writeBackDataBufferMemory);
}

void ComputeWrapper::updateCameraData(uint32_t const imageIndex, glm::vec3 const &cameraPosition, glm::vec3 const &cameraDirection, glm::vec3 const &up, glm::vec4 const &iData) {
	CameraData cameraData{};

	cameraData.iData = iData;

	if (ComputeSettings::oldIntegrator != iData.x) {
		counter = 0.0f;
		ComputeSettings::oldIntegrator = iData.x;
	}

	if (ComputeSettings::oldScaling != iData.z) {
		counter = 0.0f;
		ComputeSettings::oldScaling = iData.z;
	}

	if (ComputeSettings::oldCamera != iData.y) {
		counter = 0.0f;
		ComputeSettings::oldCamera = iData.y;
	}


	cameraData.position = glm::vec4(cameraPosition, 0.0f);
	cameraData.direction = glm::vec4(normalize(cameraDirection), 0.0f);
	cameraData.up = glm::vec4(up, 0.0f);
	cameraData.sensorData = ComputeSettings::sensorData;
	cameraData.sensorData.w = counter + 1.0f;

	if (glm::length(cameraDirection - oldCameraDirection) > 1e-6f || glm::length(cameraPosition - oldCameraPosition) > 1e-6f) {
		counter = 0.0f;
	}
	else {
		counter += 1.0f;
	}
	oldCameraDirection = cameraDirection;
	oldCameraPosition = cameraPosition;

	cameraData.data.x = counter;

	void *data;
	vkMapMemory(device, cameraDataBuffersMemory[imageIndex], 0, sizeof(CameraData), 0, &data);
	memcpy(data, &cameraData, sizeof(CameraData));
	vkUnmapMemory(device, cameraDataBuffersMemory[imageIndex]);
}

void ComputeWrapper::createDataBuffers(std::vector<BoxData> const &boxData, std::vector<BoxData> const &emissiveBoxData, std::vector<PointLight> const &pointLights) {
	bufferCreator.createBoxDataBuffer(boxDataBuffer, boxDataBufferMemory, boxData);
	if (emissiveBoxData.size() > 0) {
		bufferCreator.createBoxDataBuffer(emissiveBoxDataBuffer, emissiveBoxDataBufferMemory, emissiveBoxData);
	}
	
	if (pointLights.size() > 0) {
		bufferCreator.createPointLightBuffer(pointLightBuffer, pointLightBufferMemory, pointLights);
	}
}

void ComputeWrapper::createQuad() {
	std::vector<Vertex> vertices = {
		{{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, 0, 0, 0},
		{{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, 0, 0, 0},
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, 0, 0, 0},
		{{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, 0, 0, 0}
	};

	std::vector<uint32_t> indices = {
		2, 1, 0,
		0, 3, 2
	};

	bufferCreator.createVertexBuffer(vertices, quadVertices, quadVerticesMemory);
	bufferCreator.createIndexBuffer(indices, quadIndexBuffer, quadIndexBufferMemory);
}

void ComputeWrapper::createTexture(ImageCreator const &imageCreator) {
	imageCreator.createComputTexture(ComputeSettings::COMPUTE_WIDTH, ComputeSettings::COMPUTE_HEIGHT, computeTextureImage, computeTextureImageMemory);
	imageCreator.createComputeTextureView(computeTextureImage, computeTextureImageView);
	imageCreator.createComputeTextureSampler(computeTextureSampler);

}

void ComputeWrapper::createCameraData(size_t bufferCount) {
	cameraDataBuffers.resize(bufferCount);
	cameraDataBuffersMemory.resize(bufferCount);

	for (size_t i = 0; i < bufferCount; i++) {
		bufferCreator.createCameraDataBuffer(cameraDataBuffers[i], cameraDataBuffersMemory[i]);
	}
}

void ComputeWrapper::createQuadPipeline(VkExtent2D const extent, VkDescriptorSetLayout const &quadDescriptorSetLayout, VkRenderPass const &renderPass) {
	PipelineCreator::createPipeLine(device, extent, quadDescriptorSetLayout, renderPass, ComputeSettings::COMPUTE_VERTEX_SHADER_PATH, ComputeSettings::COMPUTE_FRAGMENT_SHADER_PATH, false, false, quadPipelineLayout, quadPipeline);
}

void ComputeWrapper::createComputePipeline(VkDescriptorSetLayout const &computeDescriptorSetLayout, char const *computeShaderPath) {
	PipelineCreator::createComputePipeLine(device, computeDescriptorSetLayout, computeShaderPath, computePipelineLayout, computePipeline);
}

void ComputeWrapper::createWriteBackDataBuffer() {
	bufferCreator.createWriteBackDataBuffer(writeBackDataBuffer, writeBackDataBufferMemory);
}
