#include "BufferCreator.h"
#include "MemoryHelper.h"
#include "CameraData.h"
#include "BoxData.h"
#include "SkyUBO.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdexcept>

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

BufferCreator::BufferCreator() {}

BufferCreator::BufferCreator(VkPhysicalDevice const &physicalDevice, VkDevice const &device, QueueFamilyIndices const &queueFamilyIndices, CommandWrapper &commandWrapper)
	: physicalDevice(physicalDevice), device(device), queueFamilyIndices(queueFamilyIndices), commandWrapper(&commandWrapper) {}

BufferCreator::~BufferCreator() {}

void BufferCreator::createVertexBuffer(std::vector<Vertex> const &vertices, VkBuffer &vertexBuffer, VkDeviceMemory &vertexBufferMemory) const {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	//Creating a staging buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//Copying the vertex data into the staging buffer
	void *data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	//Creating the real vertex buffer
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	//Copying from the staging buffer into the real vertex buffer
	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	//Releasing the staging buffer
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void BufferCreator::createBigVertexBuffer(std::vector<BigVertex> const &bigVertices, VkBuffer &bigVertexBuffer, VkDeviceMemory &bigVertexBufferMemory) const {
	VkDeviceSize bufferSize = sizeof(bigVertices[0]) * bigVertices.size();

	//Creating a staging buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//Copying the vertex data into the staging buffer
	void *data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, bigVertices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	//Creating the real vertex buffer
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bigVertexBuffer, bigVertexBufferMemory);

	//Copying from the staging buffer into the real vertex buffer
	copyBuffer(stagingBuffer, bigVertexBuffer, bufferSize);

	//Releasing the staging buffer
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void BufferCreator::createIndexBuffer(std::vector<uint32_t> const &indices, VkBuffer &indexBuffer, VkDeviceMemory &indexBufferMemory) const {
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	//Creating the staging buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//Copying the index data into the staging buffer
	void *data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	//Creating the real index buffer
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	//Copying from the stagin buffer into the real index buffer
	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	//Releasing the staging buffer
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void BufferCreator::createUniformBuffer(VkBuffer &uniformBuffer, VkDeviceMemory &uniformBufferMemory) const {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer, uniformBufferMemory);
}

void BufferCreator::createSkyUniformBuffer(VkBuffer &uniformBuffer, VkDeviceMemory &uniformBufferMemory) const {
	VkDeviceSize bufferSize = sizeof(SkyUBO);

	createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer, uniformBufferMemory);
}

void BufferCreator::createCameraDataBuffer(VkBuffer &cameraDataBuffer, VkDeviceMemory &cameraDataBufferMemory) const {
	VkDeviceSize bufferSize = sizeof(CameraData);

	createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, cameraDataBuffer, cameraDataBufferMemory);
}

void BufferCreator::createWriteBackDataBuffer(VkBuffer &writeBackDataBuffer, VkDeviceMemory &writeBackDataBufferMemory) const {
	VkDeviceSize bufferSize = sizeof(WriteBackData);

	createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, writeBackDataBuffer, writeBackDataBufferMemory);
}

void BufferCreator::createBoxDataBuffer(VkBuffer &boxDataBuffer, VkDeviceMemory &boxDataBufferMemory, std::vector<BoxData> const &boxData) const {
	VkDeviceSize bufferSize = sizeof(BoxData) * boxData.size();

	//Creating the staging buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//Copying the index data into the staging buffer
	void *data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, boxData.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	//Creating the real index buffer
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, boxDataBuffer, boxDataBufferMemory);

	//Copying from the stagin buffer into the real index buffer
	copyBuffer(stagingBuffer, boxDataBuffer, bufferSize);

	//Releasing the staging buffer
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void BufferCreator::createPointLightBuffer(VkBuffer &pointLightBuffer, VkDeviceMemory &pointLightBufferMemory, std::vector<PointLight> const &pointLights) const {
	VkDeviceSize bufferSize = sizeof(PointLight) * pointLights.size();

	//Creating the staging buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//Copying the index data into the staging buffer
	void *data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, pointLights.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	//Creating the real index buffer
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, pointLightBuffer, pointLightBufferMemory);

	//Copying from the stagin buffer into the real index buffer
	copyBuffer(stagingBuffer, pointLightBuffer, bufferSize);

	//Releasing the staging buffer
	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void BufferCreator::createTextureBuffer(char const *filePath, VkBuffer &stagingBuffer, VkDeviceMemory &stagingBufferMemory, uint32_t &textureWidth, uint32_t &textureHeight) const {
	int textureWidthINT;
	int textureHeightINT;
	int textureChannels;

	//Loading the image using the stb library
	stbi_uc *pixels = stbi_load(filePath, &textureWidthINT, &textureHeightINT, &textureChannels, STBI_rgb_alpha);

	textureWidth = static_cast<uint32_t>(textureWidthINT);
	textureHeight = static_cast<uint32_t>(textureHeightINT);

	if (!pixels) {
		throw std::runtime_error("Failed to load texture image");
	}

	VkDeviceSize imageSize = textureWidth * textureHeight * 4;

	//Creating the staging buffer
	createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//Copying the image into the staging buffer
	void *data;
	vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device, stagingBufferMemory);

	//Freeing the memory used by stb
	stbi_image_free(pixels);

#ifdef _DEBUG
	//Debug utility printing out texture infos
	std::cout << "Texture loaded:" << std::endl;
	std::cout << "\t" << "File path: " << filePath << std::endl;
	std::cout << "\t" << "Texture dimensions: " << textureWidth << " x " << textureHeight << std::endl;
#endif // _DEBUG
}

void BufferCreator::create3DTextureBuffer(glm::vec4 values[], VkDeviceSize const &bufferSize, VkBuffer &stagingBuffer, VkDeviceMemory &stagingBufferMemory) const {
	//Creating the staging buffer
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//Copying the image into the staging buffer
	void *data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, values, static_cast<size_t>(bufferSize));
	vkUnmapMemory(device, stagingBufferMemory);
}

void BufferCreator::createBuffer(VkDeviceSize const &bufferSize, VkBufferUsageFlags const &bufferUsageFlags, VkMemoryPropertyFlags const &memoryPropertyFlags, VkBuffer &buffer, VkDeviceMemory &bufferMemory) const {

	//Creating the buffer create info struct, which will be passed as argument to the createBuffer call
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = bufferSize;
	bufferCreateInfo.usage = bufferUsageFlags;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT; //If performance is bad, change with exclusive but needs memory barrier for synchronisation

	uint32_t queueFamilyIndicesArray[] = { queueFamilyIndices.graphicsFamilyIndex, queueFamilyIndices.transferFamilyIndex };

	bufferCreateInfo.queueFamilyIndexCount = 2;
	bufferCreateInfo.pQueueFamilyIndices = queueFamilyIndicesArray;

	if (vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create buffer");
	}

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

	//Creating the memory allocate info struct, which will be passed as argument to the allocateMemory call
	VkMemoryAllocateInfo memoryAllocateInfo{};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = MemoryHelper::findMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, memoryPropertyFlags);

	if (vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create buffer memory");
	}

	//Binding the buffer and the buffer memory
	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void BufferCreator::copyBuffer(VkBuffer sourceBuffer, VkBuffer destinationBuffer, VkDeviceSize size) const {
	VkCommandBuffer commandBuffer = commandWrapper->beginRecordingSingleUseTransferCommandBuffer();

	//Setting up the copy informations
	VkBufferCopy bufferCopy{};
	bufferCopy.srcOffset = 0;
	bufferCopy.dstOffset = 0;
	bufferCopy.size = size;

	{
		std::lock_guard<std::mutex> lockGuard(CommandWrapper::mutexCommandPool);
		vkCmdCopyBuffer(commandBuffer, sourceBuffer, destinationBuffer, 1, &bufferCopy);
	}

	commandWrapper->endRecordingSingleUseTransferCommandBuffer(commandBuffer);
}
