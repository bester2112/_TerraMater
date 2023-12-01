#ifndef SKYWRAPPER_H
#define SKYWRAPPER_H

#include "BufferCreator.h"
#include "Vertex.h"
#include "SkyUBO.h"
#include "CloudTexture.h"

class SkyWrapper {
public:
	SkyWrapper(VkDevice &device, BufferCreator const &bufferCreator, ImageCreator const &imageCreator, size_t const bufferCount);
	~SkyWrapper();

	VkBuffer boxVertices;
	VkDeviceMemory boxVerticesMemory;

	VkBuffer boxIndices;
	VkDeviceMemory boxIndicesMemory;

	std::vector<VkBuffer> skyUniformBufferObjects;
	std::vector<VkDeviceMemory> skyUniformBufferObjectsMemory;

	size_t indexCount;

	CloudTexture *cloudTexture;

private:

	VkDevice device;

	BufferCreator bufferCreator;

	size_t bufferCount;

	void createBox();

	void createSkyUniformBuffers();

	void createCloudTexture(ImageCreator const &imageCreator);

};

#endif // !SKYWRAPPER_H
