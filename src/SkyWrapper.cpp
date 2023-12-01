#include "SkyWrapper.h"
#include "Settings.h"
#include "Obj.h"

SkyWrapper::SkyWrapper(VkDevice &device, BufferCreator const &bufferCreator, ImageCreator const &imageCreator, size_t const bufferCount)
	: device(device), bufferCreator(bufferCreator), bufferCount(bufferCount) {
	createBox();
	createSkyUniformBuffers();
	createCloudTexture(imageCreator);
}

SkyWrapper::~SkyWrapper() {
	vkDestroyBuffer(device, boxVertices, nullptr);
	vkFreeMemory(device, boxVerticesMemory, nullptr);

	vkDestroyBuffer(device, boxIndices, nullptr);
	vkFreeMemory(device, boxIndicesMemory, nullptr);

	cloudTexture->~CloudTexture();
}

void SkyWrapper::createBox() {
	std::vector<Vertex> vertices = {
		{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}, 2, 0, 1},	//0
		{{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, 2, 0, 1},	//1
		{{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, 2, 0, 1},	//2
		{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, 2, 0, 1},	//3

		{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}, 4, 0, 1},	//7
		{{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}, 4, 0, 1},	//6
		{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, 4, 0, 1},	//4
		{{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, 4, 0, 1},	//5

		{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}, 1, 0, 1},	//11
		{{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}, 1, 0, 1},	//10
		{{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, 1, 0, 1},	//8
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, 1, 0, 1},	//9

		{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, 3, 0, 1},	//15
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, 3, 0, 1},	//14
		{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, 3, 0, 1},	//12
		{{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, 3, 0, 1},	//13

		{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}, 0, 0, 1},	//19
		{{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, 0, 0, 1},	//18
		{{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, 0, 0, 1},	//16
		{{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, 0, 0, 1},	//17

		{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}, 5, 0, 1},	//23
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, 5, 0, 1},	//22
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, 5, 0, 1},	//20
		{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, 5, 0, 1},	//21

	};

	for (int i = 0; i < 24; i++) {
		vertices[i].position.y *= 512.f;
	}

	std::vector<uint32_t> indices = {
		2,1,0,
		0,1,3,
		4,6,7,
		7,6,5,
		8,10,11,
		11,10,9,
		15,12,13,
		13,12,14,
		16,18,19,
		19,18,17,
		20,22,23,
		23,22,21
	};

	bufferCreator.createVertexBuffer(vertices, boxVertices, boxVerticesMemory);
	bufferCreator.createIndexBuffer(indices, boxIndices, boxIndicesMemory);
}

void SkyWrapper::createSkyUniformBuffers() {
	skyUniformBufferObjects.resize(bufferCount);
	skyUniformBufferObjectsMemory.resize(bufferCount);

	for (size_t i = 0; i < bufferCount; i++) {
		bufferCreator.createSkyUniformBuffer(skyUniformBufferObjects[i], skyUniformBufferObjectsMemory[i]);
	}
}

void SkyWrapper::createCloudTexture(ImageCreator const &imageCreator) {
	cloudTexture = new CloudTexture(device, imageCreator);
}
