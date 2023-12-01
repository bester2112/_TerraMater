#include "Cube.h"

Cube::Cube() : cubeType(CubeType::AIR) {}

Cube::Cube(CubeType cubeType)
	: cubeType(cubeType) {}

Cube::~Cube() {}

void Cube::getVertices(std::vector<Vertex> &vertices) {
	unsigned char top = 0;

	if (cubeType == CubeType::GRASS_BLOCK ||
		cubeType == CubeType::ACACIA_LOG ||
		cubeType == CubeType::BIRCH_LOG ||
		cubeType == CubeType::CACTUS ||
		cubeType == CubeType::DARK_OAK_LOG ||
		cubeType == CubeType::OAK_LOG ||
		cubeType == CubeType::SPRUCE_LOG ||
		cubeType == CubeType::DARK_GRASS_BLOCK
		) {
		top = 1;
	}

	vertices = {
		{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}, 2, (unsigned char)(cubeType + top), 1},	//0
		{{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, 2, (unsigned char)(cubeType + top), 1},		//1
		{{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, 2, (unsigned char)(cubeType + top), 1},	//2
		{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f}, 2, (unsigned char)(cubeType + top), 1},	//3

		{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}, 4, cubeType, 1},	//7
		{{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}, 4, cubeType, 1},	//6
		{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, 4, cubeType, 1},		//4
		{{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, 4, cubeType, 1},	//5

		{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}, 1, cubeType, 1},	//11
		{{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}, 1, cubeType, 1},	//10
		{{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}, 1, cubeType, 1},	//8
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, 1, cubeType, 1},	//9

		{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f}, 3, cubeType, 1},	//15
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, 3, cubeType, 1},	//14
		{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, 3, cubeType, 1},	//12
		{{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, 3, cubeType, 1},	//13

		{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}, 0, cubeType, 1},		//19
		{{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, 0, cubeType, 1},	//18
		{{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, 0, cubeType, 1},	//16
		{{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}, 0, cubeType, 1},	//17

		{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}, 5, cubeType, 1},	//23
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, 5, cubeType, 1},	//22
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}, 5, cubeType, 1},	//20
		{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, 5, cubeType, 1},	//21

	};
}

void Cube::getIndices(std::vector<uint32_t> &indices) {
	indices = {
		0,1,2,
		3,1,0,
		7,6,4,
		5,6,7,
		11,10,8,
		9,10,11,
		13,12,15,
		14,12,13,
		19,18,16,
		17,18,19,
		23,22,20,
		21,22,23
	};
}
