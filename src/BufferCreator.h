/**
 * @file BufferCreator.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Contains a helper class to create vulkan buffers.
 * @version 0.1
 * @date 2020-05-05
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef BUFFERCREATOR_H
#define BUFFERCREATOR_H

#include "QueueFamilyIndices.h"
#include "Vertex.h"
#include "CommandWrapper.h"
#include "UniformBufferObject.h"
#include "BoxData.h"
#include "PointLight.h"
#include "BigVertex.h"
#include "WriteBackData.h"

#include "vulkan/vulkan.h"

#include <vector>

 /**
  * @brief Helper class to create needed vulkan buffers, like for example an vertex buffer.
  */
class BufferCreator {
public:

	static std::mutex mutexBuffer;

	BufferCreator();

	/**
	 * @brief Constructs a new buffer creator object.
	 *
	 * @param physicalDevice Handle of the used physical device, which is need for infos about the available memory types.
	 * @param device Handle to the vulkan device, needed for the createBuffer call.
	 * @param queueFamilyIndices Reference to the queueFamilyIndices, needed to determine which queues have access to the created buffers later.
	 * @param commandWrapper Reference to the commandWrapper, needed to actually submit the copying of the staging buffers into the result buffers.
	 */
	BufferCreator(VkPhysicalDevice const &physicalDevice, VkDevice const &device, QueueFamilyIndices const &queueFamilyIndices, CommandWrapper &commandWrapper);

	~BufferCreator();

	/**
	 * @brief Creates a vertex buffer and allocates the neccessary memory.
	 *
	 * @param vertices Vertices which should be used for the vertex buffer.
	 * @param vertexBuffer Handle for the created vertex buffer.
	 * @param vertexBufferMemory Handle for the allocated vertex buffer memory.
	 */
	void createVertexBuffer(std::vector<Vertex> const &vertices, VkBuffer &vertexBuffer, VkDeviceMemory &vertexBufferMemory) const;

	void createBigVertexBuffer(std::vector<BigVertex> const &bigVertices, VkBuffer &bigVertexBuffer, VkDeviceMemory &bigVertexBufferMemory) const;

	/**
	 * @brief Creates a index buffer and allocates the neccessary memory.
	 *
	 * @param indices Indices which should be used for the index buffer.
	 * @param indexBuffer Handle for the created index buffer.
	 * @param indexBufferMemory Handle for the allocated index buffer memory.
	 */
	void createIndexBuffer(std::vector<uint32_t> const &indices, VkBuffer &indexBuffer, VkDeviceMemory &indexBufferMemory) const;

	/**
	 * @brief Creates a uniform buffer object and allocates the neccessary memory.
	 *
	 * @param uniformBuffer Handle for the created uniform buffer.
	 * @param uniformBufferMemory Handle for the allocated uniform buffer memory.
	 */
	void createUniformBuffer(VkBuffer &uniformBuffer, VkDeviceMemory &uniformBufferMemory) const;

	void createSkyUniformBuffer(VkBuffer &uniformBuffer, VkDeviceMemory &uniformBufferMemory) const;

	void createCameraDataBuffer(VkBuffer &cameraDataBuffer, VkDeviceMemory &cameraDataBufferMemory) const;

	void createWriteBackDataBuffer(VkBuffer &writeBackDataBuffer, VkDeviceMemory &writeBackDataBufferMemory) const;

	void createBoxDataBuffer(VkBuffer &boxDataBuffer, VkDeviceMemory &boxDataBufferMemory, std::vector<BoxData> const &boxData) const;

	void createPointLightBuffer(VkBuffer &pointLightBuffer, VkDeviceMemory &pointLightBufferMemory, std::vector<PointLight> const &pointLights) const;

	/**
	 * @brief Creates a buffer for a given texture.
	 *
	 * @param filePath Path to the texture.
	 * @param stagingBuffer Handle in which the generated buffer will be stored.
	 * @param stagingBufferMemory Handle in which the allocated memory will be stored.
	 * @param textureWidth Width of the loaded texture.
	 * @param textureHeight Height of the laoded texture.
	 */
	void createTextureBuffer(char const *filePath, VkBuffer &stagingBuffer, VkDeviceMemory &stagingBufferMemory, uint32_t &textureWidth, uint32_t &textureHeight) const;

	void create3DTextureBuffer(glm::vec4 values[], VkDeviceSize const &bufferSize, VkBuffer &stagingBuffer, VkDeviceMemory &stagingBufferMemory) const;

private:

	/**
	 * @brief Reference to the physical device, which is need for infos about the available memory types.
	 */
	VkPhysicalDevice physicalDevice;

	/**
	 * @brief Reference to the device, whis is need for some vulkan function calls.
	 */
	VkDevice device;

	/**
	 * @brief Needed to determine which queues will have acces to the generated buffers.
	 */
	QueueFamilyIndices queueFamilyIndices;

	/**
	 * @brief Reference to an command wrapper to submit the transfer of staging buffers into the real buffer.
	 */
	CommandWrapper *commandWrapper;

	/**
	 * @brief Creates a vulkan buffer and allocates the buffer memory.
	 *
	 * @param bufferSize Size of the created buffer.
	 * @param bufferUsageFlags Flags determining the later usage of the created buffer.
	 * @param memoryPropertyFlags Flags determining the needed memory properties of the created buffer.
	 * @param buffer Handle for the created buffer.
	 * @param bufferMemory Handle for the allocated buffer memory.
	 */
	void createBuffer(VkDeviceSize const &bufferSize, VkBufferUsageFlags const &bufferUsageFlags, VkMemoryPropertyFlags const &memoryPropertyFlags, VkBuffer &buffer, VkDeviceMemory &bufferMemory) const;

	/**
	 * @brief Copys one buffer into another, using the commandWrapper.
	 *
	 * @param sourceBuffer Source buffer which will be copied.
	 * @param destinationBuffer Destination buffer into which the source buffer will be copied.
	 * @param size size of the buffer which will be copied.
	 */
	void copyBuffer(VkBuffer sourceBuffer, VkBuffer destinationBuffer, VkDeviceSize size) const;
};

#endif // !BUFFERCREATOR_H
