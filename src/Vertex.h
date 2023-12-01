/**
 * @file Vertex.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Contains the Vertex struct and neccessary functions to create Vulkan compatible binding and attribute descriptions.
 * @version 0.1
 * @date 2020-05-03
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef VERTEX_H
#define VERTEX_H

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

#include <array>

 /**
  * @brief Simple Vertex struct which contains the vertex position, normal and texture coordinate.
  *
  * Using only a struct, because no class functionality is needed.
  */
struct Vertex {
	glm::vec3 position;
	glm::vec2 textureCoordinate;
	unsigned char normalID;
	unsigned char textureID;
	unsigned char ambientOcclusionValue;
	unsigned char lightLevel;

	/**
	 * @brief Gets the vertex input binding description.
	 *
	 * @return VkVertexInputBindingDescription The binding description for Vertex.
	 */
	static VkVertexInputBindingDescription getVertexInputBindingDescription() {
		VkVertexInputBindingDescription vertexInputBindingDescription{};
		vertexInputBindingDescription.binding = 0;
		vertexInputBindingDescription.stride = sizeof(Vertex);
		vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return vertexInputBindingDescription;
	}

	/**
	 * @brief Gets the vertex input attribute descriptions.
	 *
	 * @return std::array<VkVertexInputAttributeDescription, 3> Array containing the attribute descriptions of the vertex struct.
	 */
	static std::array<VkVertexInputAttributeDescription, 6> getVertexInputAttributeDescription() {
		std::array<VkVertexInputAttributeDescription, 6> vertexInputAttributeDescriptions;

		vertexInputAttributeDescriptions[0].binding = 0;
		vertexInputAttributeDescriptions[0].location = 0;
		vertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescriptions[0].offset = offsetof(Vertex, position);

		vertexInputAttributeDescriptions[1].binding = 0;
		vertexInputAttributeDescriptions[1].location = 1;
		vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		vertexInputAttributeDescriptions[1].offset = offsetof(Vertex, textureCoordinate);

		vertexInputAttributeDescriptions[2].binding = 0;
		vertexInputAttributeDescriptions[2].location = 2;
		vertexInputAttributeDescriptions[2].format = VK_FORMAT_R8_UINT;
		vertexInputAttributeDescriptions[2].offset = offsetof(Vertex, normalID);

		vertexInputAttributeDescriptions[3].binding = 0;
		vertexInputAttributeDescriptions[3].location = 3;
		vertexInputAttributeDescriptions[3].format = VK_FORMAT_R8_UINT;
		vertexInputAttributeDescriptions[3].offset = offsetof(Vertex, textureID);

		vertexInputAttributeDescriptions[4].binding = 0;
		vertexInputAttributeDescriptions[4].location = 4;
		vertexInputAttributeDescriptions[4].format = VK_FORMAT_R8_UINT;
		vertexInputAttributeDescriptions[4].offset = offsetof(Vertex, ambientOcclusionValue);

		vertexInputAttributeDescriptions[5].binding = 0;
		vertexInputAttributeDescriptions[5].location = 5;
		vertexInputAttributeDescriptions[5].format = VK_FORMAT_R8_UINT;
		vertexInputAttributeDescriptions[5].offset = offsetof(Vertex, lightLevel);

		return vertexInputAttributeDescriptions;
	}
};

inline bool operator== (Vertex const &a, Vertex const &b) {
	return abs(glm::length(a.position - b.position)) < 1e-6;
}

#endif // VERTEX_H