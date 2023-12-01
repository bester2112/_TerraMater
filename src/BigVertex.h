#ifndef BIGVERTEX_H
#define BIGVERTEX_H

#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

#include <array>

struct BigVertex {
	glm::vec3 position;
	glm::vec2 textureCoordinate;
	glm::vec3 normal;
	unsigned char textureID;

	static VkVertexInputBindingDescription getVertexInputBindingDescription() {
		VkVertexInputBindingDescription vertexInputBindingDescription{};
		vertexInputBindingDescription.binding = 0;
		vertexInputBindingDescription.stride = sizeof(BigVertex);
		vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return vertexInputBindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 4> getVertexInputAttributeDescription() {
		std::array<VkVertexInputAttributeDescription, 4> vertexInputAttributeDescription{};
		
		vertexInputAttributeDescription[0].binding = 0;
		vertexInputAttributeDescription[0].location = 0;
		vertexInputAttributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescription[0].offset = offsetof(BigVertex, position);

		vertexInputAttributeDescription[1].binding = 0;
		vertexInputAttributeDescription[1].location = 1;
		vertexInputAttributeDescription[1].format = VK_FORMAT_R32G32_SFLOAT;
		vertexInputAttributeDescription[1].offset = offsetof(BigVertex, textureCoordinate);

		vertexInputAttributeDescription[2].binding = 0;
		vertexInputAttributeDescription[2].location = 2;
		vertexInputAttributeDescription[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttributeDescription[2].offset = offsetof(BigVertex, normal);

		vertexInputAttributeDescription[3].binding = 0;
		vertexInputAttributeDescription[3].location = 3;
		vertexInputAttributeDescription[3].format = VK_FORMAT_R8_UINT;
		vertexInputAttributeDescription[3].offset = offsetof(BigVertex, textureID);

		return vertexInputAttributeDescription;
	}
};

#endif // !BIGVERTEX_H
