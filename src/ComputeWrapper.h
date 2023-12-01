#ifndef COMPUTEWRAPPER_H
#define COMPUTEWRAPPER_H

#include "ImageCreator.h"
#include "BufferCreator.h"
#include "BoxData.h"
#include "PointLight.h"
#include "WriteBackData.h"

class ComputeWrapper {
public:
	ComputeWrapper(VkDevice &device, ImageCreator const &imageCreator, BufferCreator const &bufferCreator, VkExtent2D const extent, size_t const bufferCount, VkDescriptorSetLayout const &quadDescriptorSetLayout, VkRenderPass const &renderPass, VkDescriptorSetLayout const &computeDescriptorSetLayout, char const *computeShaderPath);
	~ComputeWrapper();

	VkBuffer quadVertices;
	VkDeviceMemory quadVerticesMemory;

	VkBuffer quadIndexBuffer;
	VkDeviceMemory quadIndexBufferMemory;

	VkPipelineLayout quadPipelineLayout;
	VkPipeline quadPipeline;

	VkPipelineLayout computePipelineLayout;
	VkPipeline computePipeline;

	VkImage computeTextureImage;
	VkDeviceMemory computeTextureImageMemory;
	VkImageView computeTextureImageView;
	VkSampler computeTextureSampler;

	std::vector<VkBuffer> cameraDataBuffers;
	std::vector<VkDeviceMemory> cameraDataBuffersMemory;

	VkBuffer writeBackDataBuffer;
	VkDeviceMemory writeBackDataBufferMemory;

	void storeAndResetWriteBackData();

	void updateCameraData(uint32_t const imageIndex, glm::vec3 const &cameraPosition, glm::vec3 const &cameraDirection, glm::vec3 const &up, glm::vec4 const &iData);

	VkBuffer boxDataBuffer;
	VkDeviceMemory boxDataBufferMemory;
	VkBuffer emissiveBoxDataBuffer;
	VkDeviceMemory emissiveBoxDataBufferMemory;
	VkBuffer pointLightBuffer;
	VkDeviceMemory pointLightBufferMemory;
	bool boxesGenerated = false;
	bool allocated = false;

	void createDataBuffers(std::vector<BoxData> const &boxData, std::vector<BoxData> const &emissiveBoxData, std::vector<PointLight> const &pointLights);

private:
	VkDevice device;

	BufferCreator bufferCreator;

	glm::vec3 oldCameraDirection;
	glm::vec3 oldCameraPosition;
	float counter = 0.0f;

	void createQuad();
	void createTexture(ImageCreator const &imageCreator);
	void createCameraData(size_t bufferCount);
	void createQuadPipeline(VkExtent2D const extent, VkDescriptorSetLayout const &quadDescriptorSetLayout, VkRenderPass const &renderPass);
	void createComputePipeline(VkDescriptorSetLayout const &computeDescriptorSetLayout, char const *computeShaderPath);

	void createWriteBackDataBuffer();
};

#endif // !COMPUTEWRAPPER_H
