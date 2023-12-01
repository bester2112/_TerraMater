#include "LoadedChunkStack.h"
#include "BigVertex.h"

#include "glm/gtx/rotate_vector.hpp"

#include <iostream>
#include <queue>

LoadedChunkStack::LoadedChunkStack(VulkanWrapper &vulkanWrapper, ObjArray *objArray)
	: vulkanWrapper(&vulkanWrapper), objArray(objArray) {
	chunkStack = ChunkStack();
	updateHeight();
}

LoadedChunkStack::~LoadedChunkStack() {}

void LoadedChunkStack::generateVulkanChunks() {
	updateHeight();
	for (int y = 0; y < chunkStack.stack.size(); y++) {
		generateVulkanChunk(y);
	}

	glm::vec3 minB = glm::vec3(chunkStack.coordinates.x * Settings::CHUNK_SIZE, 0.0f, chunkStack.coordinates.z * Settings::CHUNK_SIZE);
	glm::vec3 maxB = minB + glm::vec3(Settings::CHUNK_SIZE, chunkStack.stack.size() * Settings::CHUNK_SIZE, Settings::CHUNK_SIZE);

	aabb = AABB(minB, maxB);

	chunkStackReady = true;
}

void LoadedChunkStack::deleteVulkanChunks() {
	chunkStackReady = false;
	for (size_t y = 0; y < chunkStack.stack.size(); y++) {
		deleteVulkanChunk(y);
	}
}

void LoadedChunkStack::generateBoxData(std::vector<BoxData> &boxData, std::vector<BoxData> &emissiveBoxData) {
	int size = boxData.size();

	for (int y = 0; y < chunkStack.stack.size(); y++) {
		for (int u = 0; u < Settings::CHUNK_SIZE; u++) {
			for (int w = 0; w < Settings::CHUNK_SIZE; w++) {
				for (int v = 0; v < Settings::CHUNK_SIZE; v++) {

					//If cube is visible from atleast one side add its data
					if (!cullCube(u, w, v, y)) {
						Cube cube = chunkStack.stack[y].cubes[u][w][v];
						float xPosition = chunkStack.coordinates.x;
						float zPosition = chunkStack.coordinates.z;

						glm::vec3 position = glm::vec3(u + xPosition * Settings::CHUNK_SIZE, v + y * Settings::CHUNK_SIZE, w + zPosition * Settings::CHUNK_SIZE);

						boxData.push_back({ position, cube.cubeType });

						if (cube.cubeType == CubeType::ACACIA_LOG ||
							cube.cubeType == CubeType::BIRCH_LOG ||
							cube.cubeType == CubeType::CACTUS ||
							cube.cubeType == CubeType::DARK_OAK_LOG ||
							cube.cubeType == CubeType::OAK_LOG ||
							cube.cubeType == CubeType::SPRUCE_LOG) {
							emissiveBoxData.push_back({ position, cube.cubeType });
						}
					}
				}
			}
		}
	}
}

void LoadedChunkStack::updateHeight() {
	size_t size = chunkStack.stack.size();

	chunkVertexBuffer.resize(size);
	chunkVertexBufferMemory.resize(size);
	chunkIndexBuffer.resize(size);
	chunkIndexBufferMemory.resize(size);
	chunkIndexCount.resize(size);

	objVertexBuffer.resize(size);
	objVertexBufferMemory.resize(size);
	objIndexBuffer.resize(size);
	objIndexBufferMemory.resize(size);
	objIndexCount.resize(size);
}

void LoadedChunkStack::generateVulkanChunk(int const y) {
	calculateLightLevels();

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	std::vector<BigVertex> objVertices;
	std::vector<uint32_t> objIndices;

	std::vector<Quad> cubeSideQuads[6];

	size_t sideCounter = 0;

	int cou = 0;

	//Iterates over all cubes
	for (int u = 0; u < Settings::CHUNK_SIZE; u++) {
		for (int w = 0; w < Settings::CHUNK_SIZE; w++) {
			for (int v = 0; v < Settings::CHUNK_SIZE; v++) {

				//If cube is visible from atleast one side add its data
				if (!cullCube(u, w, v, y)) {
					Cube cube = chunkStack.stack[y].cubes[u][w][v];
					float xPosition = chunkStack.coordinates.x;
					float zPosition = chunkStack.coordinates.z;

					std::vector<Vertex> cubeVertices;
					cube.getVertices(cubeVertices);

					for (int side = 0; side < 6; side++) {
						if (!cullSide(u, w, v, y, side)) {
							cubeSideQuads[side].push_back({});
							Vertex *sideVertices = cubeSideQuads[side][cubeSideQuads[side].size() - 1].vertices;
							uint32_t *sideIndices = cubeSideQuads[side][cubeSideQuads[side].size() - 1].indices;

							int8_t lightLevel = getLightLevel(u, w, v, y, side);

							for (int i = 0; i < 4; i++) {
								sideVertices[i] = cubeVertices[i + side * 4];

								sideVertices[i].position += glm::vec3(u + xPosition * Settings::CHUNK_SIZE, v + y * Settings::CHUNK_SIZE, w + zPosition * Settings::CHUNK_SIZE);

								sideVertices[i].ambientOcclusionValue = calculateAmbientOcclusionValue(u, w, v, y, i + side * 4);

								sideVertices[i].lightLevel = lightLevel;
							}

							bool flipped = flippedTriangles(sideVertices[0].ambientOcclusionValue, sideVertices[2].ambientOcclusionValue, sideVertices[1].ambientOcclusionValue, sideVertices[3].ambientOcclusionValue);

							if (flipped) {
								sideIndices[0] = 3;

								sideIndices[3] = 0;
								sideIndices[4] = 3;
								sideIndices[5] = 2;
							}
						}
					}
				}

				ObjData objData = chunkStack.stack[y].objData[u][w][v];
				if (objData.objType != ObjType::EMPTY) {
					cou++;
					float xPosition = chunkStack.coordinates.x;
					float zPosition = chunkStack.coordinates.z;

					std::vector<BigVertex> objLocalVertices;
					objArray->objs[objData.objType].getVertices(objLocalVertices);

					glm::vec3 offset = glm::vec3(u + xPosition * Settings::CHUNK_SIZE + objData.xOffset, v + y * Settings::CHUNK_SIZE, w + zPosition * Settings::CHUNK_SIZE + objData.zOffset);
					for (int i = 0; i < objLocalVertices.size(); i++) {
						objLocalVertices[i].position = glm::rotateY(objLocalVertices[i].position, objData.yRotation);
						objLocalVertices[i].normal = glm::rotateY(objLocalVertices[i].normal, objData.yRotation);

						objLocalVertices[i].position += offset;

						objLocalVertices[i].textureID = objData.objType;
					}

					objVertices.insert(objVertices.end(), objLocalVertices.begin(), objLocalVertices.end());

					std::vector<uint32_t> objLocalIndices;
					objArray->objs[objData.objType].getIndices(objLocalIndices);

					uint32_t localSize = objIndices.size();

					for (int i = 0; i < objLocalIndices.size(); i++) {
						objLocalIndices[i] += localSize;
					}

					objIndices.insert(objIndices.end(), objLocalIndices.begin(), objLocalIndices.end());
				}

			}
		}
	}

	for (int side = 0; side < 6; side++) {
		greedyMesh2D(cubeSideQuads[side], side);

		for (int i = 0; i < cubeSideQuads[side].size(); i++) {
			Quad *quad = &cubeSideQuads[side][i];

			vertices.insert(vertices.end(), quad->vertices, quad->vertices + 4);

			for (int j = 0; j < 6; j++) {
				quad->indices[j] += sideCounter * 4;
			}

			indices.insert(indices.end(), quad->indices, quad->indices + 6);

			sideCounter++;
		}
	}

	chunkIndexCount[y] = static_cast<uint32_t>(indices.size());

	//if (vertices.size() == 0) {
	//	return;
	//}

	////Generates vulkan usable data
	//vulkanWrapper->createVulkanLoadedChunk(vertices, indices, chunkVertexBuffer[y], chunkVertexBufferMemory[y], chunkIndexBuffer[y], chunkIndexBufferMemory[y]);

	if (vertices.size() != 0) {
		//Generates vulkan usable data
		vulkanWrapper->createVulkanLoadedChunk(vertices, indices, chunkVertexBuffer[y], chunkVertexBufferMemory[y], chunkIndexBuffer[y], chunkIndexBufferMemory[y]);
	}

	objIndexCount[y] = static_cast<uint32_t>(objIndices.size());

	if (objVertices.size() != 0) {
		vulkanWrapper->createVulkanObjChunk(objVertices, objIndices, objVertexBuffer[y], objVertexBufferMemory[y], objIndexBuffer[y], objIndexBufferMemory[y]);
	}

	std::cout << cou << std::endl;
}

void LoadedChunkStack::deleteVulkanChunk(int const y) {
	vulkanWrapper->deleteVulkanLoadedChunk(chunkVertexBuffer[y], chunkVertexBufferMemory[y], chunkIndexBuffer[y], chunkIndexBufferMemory[y]);
	vulkanWrapper->deleteVulkanLoadedChunk(objVertexBuffer[y], objVertexBufferMemory[y], objIndexBuffer[y], objIndexBufferMemory[y]);
}

bool LoadedChunkStack::cullCube(int const u, int const w, int const v, int const y) {
	Chunk *chunk = &chunkStack.stack[y];
	Cube cube = chunkStack.stack[y].cubes[u][w][v];

	if (cube.cubeType != CubeType::AIR) {// && cube.cubeType != CubeType::WATER) {
		//Left
		if (u - 1 < 0) {
			if (y >= leftStack.stack.size()) {
				return false;
			}
			else {
				if (leftStack.stack[y].cubes[Settings::CHUNK_SIZE - 1][w][v].cubeType == CubeType::AIR || leftStack.stack[y].cubes[Settings::CHUNK_SIZE - 1][w][v].cubeType == CubeType::WATER) {
					return false;
				}
			}
		}
		else {
			if (chunk->cubes[u - 1][w][v].cubeType == CubeType::AIR || chunk->cubes[u - 1][w][v].cubeType == CubeType::WATER) {
				return false;
			}
		}

		//Right
		if (u + 1 >= Settings::CHUNK_SIZE) {
			if (y >= rightStack.stack.size()) {
				return false;
			}
			else {
				if (rightStack.stack[y].cubes[0][w][v].cubeType == CubeType::AIR || rightStack.stack[y].cubes[0][w][v].cubeType == CubeType::WATER) {
					return false;
				}
			}
		}
		else {
			if (chunk->cubes[u + 1][w][v].cubeType == CubeType::AIR || chunk->cubes[u + 1][w][v].cubeType == CubeType::WATER) {
				return false;
			}
		}

		//Front
		if (w - 1 < 0) {
			if (y >= frontStack.stack.size()) {
				return false;
			}
			else {
				if (frontStack.stack[y].cubes[u][Settings::CHUNK_SIZE - 1][v].cubeType == CubeType::AIR || frontStack.stack[y].cubes[u][Settings::CHUNK_SIZE - 1][v].cubeType == CubeType::WATER) {
					return false;
				}
			}
		}
		else {
			if (chunk->cubes[u][w - 1][v].cubeType == CubeType::AIR || chunk->cubes[u][w - 1][v].cubeType == CubeType::WATER) {
				return false;
			}
		}

		//Back
		if (w + 1 >= Settings::CHUNK_SIZE) {
			if (y >= backStack.stack.size()) {
				return false;
			}
			else {
				if (backStack.stack[y].cubes[u][0][v].cubeType == CubeType::AIR || backStack.stack[y].cubes[u][0][v].cubeType == CubeType::WATER) {
					return false;
				}
			}
		}
		else {
			if (chunk->cubes[u][w + 1][v].cubeType == CubeType::AIR || chunk->cubes[u][w + 1][v].cubeType == CubeType::WATER) {
				return false;
			}
		}

		//Bottom
		if (v - 1 < 0) {
			if (y - 1 > 0) {
				if (chunkStack.stack[y - 1].cubes[u][w][Settings::CHUNK_SIZE - 1].cubeType == CubeType::AIR || chunkStack.stack[y - 1].cubes[u][w][Settings::CHUNK_SIZE - 1].cubeType == CubeType::WATER) {
					return false;
				}
			}
		}
		else {
			if (chunk->cubes[u][w][v - 1].cubeType == CubeType::AIR || chunk->cubes[u][w][v - 1].cubeType == CubeType::WATER) {
				return false;
			}
		}

		//Top
		if (v + 1 >= Settings::CHUNK_SIZE) {
			if (y + 1 >= chunkStack.stack.size()) {
				return false;
			}
			else {
				if (chunkStack.stack[y + 1].cubes[u][w][0].cubeType == CubeType::AIR || chunkStack.stack[y + 1].cubes[u][w][0].cubeType == CubeType::WATER) {
					return false;
				}
			}
		}
		else {
			if (chunk->cubes[u][w][v + 1].cubeType == CubeType::AIR || chunk->cubes[u][w][v + 1].cubeType == CubeType::WATER) {
				return false;
			}
		}
	}

	//Cull it
	return true;
}

uint8_t LoadedChunkStack::calculateAmbientOcclusionValue(int const u, int const w, int const v, int const y, int const vertexID) {
	Chunk *chunk = &chunkStack.stack[y];

	Chunk *side1Chunk = chunk;
	Chunk *side2Chunk = chunk;
	Chunk *cornerChunk = chunk;

	CubeType side1 = CubeType::AIR;
	CubeType side2 = CubeType::AIR;
	CubeType corner = CubeType::AIR;

	bool uLeft = false;
	bool uRight = false;
	bool wFront = false;
	bool wBack = false;
	bool vBottom = false;
	bool vTop = false;

	int uP = u + 1;
	int uM = u - 1;
	int wP = w + 1;
	int wM = w - 1;
	int vP = v + 1;
	int vM = v - 1;

	if (u - 1 < 0) {
		uLeft = true;
		uM = Settings::CHUNK_SIZE - 1;
	}
	if (u + 1 >= Settings::CHUNK_SIZE) {
		uRight = true;
		uP = 0;
	}
	if (w - 1 < 0) {
		wFront = true;
		wM = Settings::CHUNK_SIZE - 1;
	}
	if (w + 1 >= Settings::CHUNK_SIZE) {
		wBack = true;
		wP = 0;
	}
	if (v - 1 < 0) {
		vBottom = true;
		vM = Settings::CHUNK_SIZE - 1;
	}
	if (v + 1 >= Settings::CHUNK_SIZE) {
		vTop = true;
		vP = 0;
	}

	switch (vertexID) {
	case 0:
		if (uLeft) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
				cornerChunk = &leftStack.stack[y];
			}
		}

		if (wFront) {
			if (y >= frontStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &frontStack.stack[y];
				cornerChunk = &frontStack.stack[y];
			}
		}

		if (vTop) {
			if (y + 1 >= chunkStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &chunkStack.stack[y + 1];
				side2Chunk = &chunkStack.stack[y + 1];
				cornerChunk = &chunkStack.stack[y + 1];
			}
		}

		if (uLeft && vTop) {
			if (y + 1 >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y + 1];
				cornerChunk = &leftStack.stack[y + 1];
			}

			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
			}
		}

		if (wFront && vTop) {
			if (y + 1 >= frontStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &frontStack.stack[y + 1];
				cornerChunk = &frontStack.stack[y + 1];
			}

			if (y + 1 >= chunkStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &chunkStack.stack[y + 1];
			}
		}

		if (uLeft && wFront) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
			}

			if (y >= frontLeftStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &frontLeftStack.stack[y];
			}

			if (y >= frontStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &frontStack.stack[y];
			}
		}

		side1 = side1Chunk->cubes[uM][w][vP].cubeType;
		side2 = side2Chunk->cubes[u][wM][vP].cubeType;
		corner = cornerChunk->cubes[uM][wM][vP].cubeType;
		break;
	case 1:
		if (uRight) {
			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
				cornerChunk = &rightStack.stack[y];
			}
		}

		if (wBack) {
			if (y >= backStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &backStack.stack[y];
				cornerChunk = &backStack.stack[y];
			}
		}

		if (vTop) {
			if (y + 1 >= chunkStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &chunkStack.stack[y + 1];
				side2Chunk = &chunkStack.stack[y + 1];
				cornerChunk = &chunkStack.stack[y + 1];
			}
		}

		if (uRight && vTop) {
			if (y + 1 >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y + 1];
				cornerChunk = &rightStack.stack[y + 1];
			}

			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
			}
		}

		if (wBack && vTop) {
			if (y + 1 >= backStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &backStack.stack[y + 1];
				cornerChunk = &backStack.stack[y + 1];
			}

			if (y + 1 >= chunkStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &chunkStack.stack[y + 1];
			}
		}

		side1 = side1Chunk->cubes[uP][w][vP].cubeType;
		side2 = side2Chunk->cubes[u][wP][vP].cubeType;
		corner = cornerChunk->cubes[uP][wP][vP].cubeType;
		break;
	case 2:
		if (uRight) {
			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
				cornerChunk = &rightStack.stack[y];
			}
		}

		if (wFront) {
			if (y >= frontStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &frontStack.stack[y];
				cornerChunk = &frontStack.stack[y];
			}
		}

		if (vTop) {
			if (y + 1 >= chunkStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &chunkStack.stack[y + 1];
				side2Chunk = &chunkStack.stack[y + 1];
				cornerChunk = &chunkStack.stack[y + 1];
			}
		}

		if (uRight && vTop) {
			if (y + 1 >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y + 1];
				cornerChunk = &rightStack.stack[y + 1];
			}

			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
			}
		}

		if (wFront && vTop) {
			if (y + 1 >= frontStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &frontStack.stack[y + 1];
				cornerChunk = &frontStack.stack[y + 1];
			}

			if (y + 1 >= chunkStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &chunkStack.stack[y + 1];
			}
		}

		side1 = side1Chunk->cubes[uP][w][vP].cubeType;
		side2 = side2Chunk->cubes[u][wM][vP].cubeType;
		corner = cornerChunk->cubes[uP][wM][vP].cubeType;
		break;
	case 3:
		if (uLeft) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
				cornerChunk = &leftStack.stack[y];
			}
		}

		if (wBack) {
			if (y >= backStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &backStack.stack[y];
				cornerChunk = &backStack.stack[y];
			}
		}

		if (vTop) {
			if (y + 1 >= chunkStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &chunkStack.stack[y + 1];
				side2Chunk = &chunkStack.stack[y + 1];
				cornerChunk = &chunkStack.stack[y + 1];
			}
		}

		if (uLeft && vTop) {
			if (y + 1 >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y + 1];
				cornerChunk = &leftStack.stack[y + 1];
			}

			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
			}
		}

		if (wBack && vTop) {
			if (y + 1 >= backStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &backStack.stack[y + 1];
				cornerChunk = &backStack.stack[y + 1];
			}

			if (y + 1 >= chunkStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &chunkStack.stack[y + 1];
			}
		}

		side1 = side1Chunk->cubes[uM][w][vP].cubeType;
		side2 = side2Chunk->cubes[u][wP][vP].cubeType;
		corner = cornerChunk->cubes[uM][wP][vP].cubeType;
		break;
	case 6:
		if (uRight) {
			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
				cornerChunk = &rightStack.stack[y];
			}
		}

		if (wBack) {
			if (y >= backStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &backStack.stack[y];
				side2Chunk = &backStack.stack[y];
				cornerChunk = &backStack.stack[y];
			}
		}

		if (vTop) {
			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
				cornerChunk = &chunkStack.stack[y + 1];
			}
		}

		if (uRight && vTop) {
			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
			}

			if (y + 1 >= rightStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &rightStack.stack[y + 1];
			}

			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
			}
		}

		if (wBack && vTop) {
			if (y >= backStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &backStack.stack[y];
			}

			if (y + 1 >= backStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &backStack.stack[y + 1];
				cornerChunk = &backStack.stack[y + 1];
			}
		}

		side1 = side1Chunk->cubes[uP][wP][v].cubeType;
		side2 = side2Chunk->cubes[u][wP][vP].cubeType;
		corner = cornerChunk->cubes[uP][wP][vP].cubeType;
		break;
	case 7:
		if (uLeft) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
				cornerChunk = &leftStack.stack[y];
			}
		}

		if (wBack) {
			if (y >= backStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &backStack.stack[y];
				side2Chunk = &backStack.stack[y];
				cornerChunk = &backStack.stack[y];
			}
		}

		if (vBottom) {
			if (y - 1 < 0) {
				return 3;
			}
			side2Chunk = &chunkStack.stack[y - 1];
			cornerChunk = &chunkStack.stack[y - 1];
		}

		if (uLeft && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
			}

			if (y - 1 >= leftStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &leftStack.stack[y - 1];
			}

			side2Chunk = &chunkStack.stack[y - 1];
		}

		if (wBack && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y >= backStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &backStack.stack[y];
			}

			if (y - 1 >= backStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &backStack.stack[y - 1];
				cornerChunk = &backStack.stack[y - 1];
			}
		}

		side1 = side1Chunk->cubes[uM][wP][v].cubeType;
		side2 = side2Chunk->cubes[u][wP][vM].cubeType;
		corner = cornerChunk->cubes[uM][wP][vM].cubeType;
		break;
	case 5:
		if (uRight) {
			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
				cornerChunk = &rightStack.stack[y];
			}
		}

		if (wBack) {
			if (y >= backStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &backStack.stack[y];
				side2Chunk = &backStack.stack[y];
				cornerChunk = &backStack.stack[y];
			}
		}

		if (vBottom) {
			if (y - 1 < 0) {
				return 3;
			}
			side2Chunk = &chunkStack.stack[y - 1];
			cornerChunk = &chunkStack.stack[y - 1];
		}

		if (uRight && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
			}

			if (y - 1 >= rightStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &rightStack.stack[y - 1];
			}

			side2Chunk = &chunkStack.stack[y - 1];
		}

		if (wBack && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y >= backStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &backStack.stack[y];
			}

			if (y - 1 >= backStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &backStack.stack[y - 1];
				cornerChunk = &backStack.stack[y - 1];
			}
		}

		side1 = side1Chunk->cubes[uP][wP][v].cubeType;
		side2 = side2Chunk->cubes[u][wP][vM].cubeType;
		corner = cornerChunk->cubes[uP][wP][vM].cubeType;
		break;
	case 4:
		if (uLeft) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
				cornerChunk = &leftStack.stack[y];
			}
		}

		if (wBack) {
			if (y >= backStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &backStack.stack[y];
				side2Chunk = &backStack.stack[y];
				cornerChunk = &backStack.stack[y];
			}
		}

		if (vTop) {
			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
				cornerChunk = &chunkStack.stack[y + 1];
			}
		}

		if (uLeft && vTop) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
			}

			if (y + 1 >= leftStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &leftStack.stack[y + 1];
			}

			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
			}
		}

		if (wBack && vTop) {
			if (y >= backStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &backStack.stack[y];
			}

			if (y + 1 >= backStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &backStack.stack[y + 1];
				cornerChunk = &backStack.stack[y + 1];
			}
		}

		side1 = side1Chunk->cubes[uM][wP][v].cubeType;
		side2 = side2Chunk->cubes[u][wP][vP].cubeType;
		corner = cornerChunk->cubes[uM][wP][vP].cubeType;
		break;
	case 10:
		if (uLeft) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
				side2Chunk = &leftStack.stack[y];
				cornerChunk = &leftStack.stack[y];
			}
		}

		if (wBack) {
			if (y >= backStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &backStack.stack[y];
				side2Chunk = &backStack.stack[y];
				cornerChunk = &backStack.stack[y];
			}
		}

		if (vTop) {
			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
				cornerChunk = &chunkStack.stack[y + 1];
			}
		}

		if (uLeft && vTop) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
			}

			if (y + 1 >= leftStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &leftStack.stack[y + 1];
				cornerChunk = &leftStack.stack[y + 1];
			}
		}

		if (wBack && vTop) {
			if (y >= backStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &backStack.stack[y];
			}

			if (y + 1 >= backStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &backStack.stack[y + 1];
				cornerChunk = &backStack.stack[y + 1];
			}
		}

		side1 = side1Chunk->cubes[uM][wP][v].cubeType;
		side2 = side2Chunk->cubes[uM][w][vP].cubeType;
		corner = cornerChunk->cubes[uM][wP][vP].cubeType;
		break;
	case 11:
		if (uLeft) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
				side2Chunk = &leftStack.stack[y];
				cornerChunk = &leftStack.stack[y];
			}
		}

		if (wFront) {
			if (y >= frontStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontStack.stack[y];
				cornerChunk = &frontStack.stack[y];
			}
		}

		if (vBottom) {
			if (y - 1 < 0) {
				return 3;
			}
			side2Chunk = &chunkStack.stack[y - 1];
			cornerChunk = &chunkStack.stack[y - 1];
		}

		if (uLeft && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
			}

			if (y - 1 >= leftStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &leftStack.stack[y - 1];
				cornerChunk = &leftStack.stack[y - 1];
			}
		}

		if (wFront && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y >= frontStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontStack.stack[y];
			}

			if (y - 1 >= frontStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &frontStack.stack[y - 1];
			}

			side2Chunk = &chunkStack.stack[y - 1];
		}

		if (uLeft && wFront) {
			if (y >= leftStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &leftStack.stack[y];
			}

			if (y >= frontLeftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontLeftStack.stack[y];
				cornerChunk = &frontLeftStack.stack[y];
			}
		}

		side1 = side1Chunk->cubes[uM][wM][v].cubeType;
		side2 = side2Chunk->cubes[uM][w][vM].cubeType;
		corner = cornerChunk->cubes[uM][wM][vM].cubeType;
		break;
	case 9:
		if (uLeft) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
				side2Chunk = &leftStack.stack[y];
				cornerChunk = &leftStack.stack[y];
			}
		}

		if (wBack) {
			if (y >= backStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &backStack.stack[y];
				cornerChunk = &backStack.stack[y];
			}
		}

		if (vBottom) {
			if (y - 1 < 0) {
				return 3;
			}
			side2Chunk = &chunkStack.stack[y - 1];
			cornerChunk = &chunkStack.stack[y - 1];
		}

		if (uLeft && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
			}

			if (y - 1 >= leftStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &leftStack.stack[y - 1];
				cornerChunk = &leftStack.stack[y - 1];
			}
		}

		if (wBack && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y >= backStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &backStack.stack[y];
			}

			if (y - 1 >= backStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &backStack.stack[y - 1];
			}

			side2Chunk = &chunkStack.stack[y - 1];
		}

		side1 = side1Chunk->cubes[uM][wP][v].cubeType;
		side2 = side2Chunk->cubes[uM][w][vM].cubeType;
		corner = cornerChunk->cubes[uM][wP][vM].cubeType;
		break;
	case 8:
		if (uLeft) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
				side2Chunk = &leftStack.stack[y];
				cornerChunk = &leftStack.stack[y];
			}
		}

		if (wFront) {
			if (y >= frontStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontStack.stack[y];
				cornerChunk = &frontStack.stack[y];
			}
		}

		if (vTop) {
			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
				cornerChunk = &chunkStack.stack[y + 1];
			}
		}

		if (uLeft && vTop) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
			}

			if (y + 1 >= leftStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &leftStack.stack[y + 1];
				cornerChunk = &leftStack.stack[y + 1];
			}
		}

		if (wFront && vTop) {
			if (y >= frontStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontStack.stack[y];
			}

			if (y + 1 >= frontStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &frontStack.stack[y + 1];
			}

			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
			}
		}

		if (uLeft && wFront) {
			if (y >= leftStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &leftStack.stack[y];
			}

			if (y >= frontLeftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontLeftStack.stack[y];
				cornerChunk = &frontLeftStack.stack[y];
			}
		}

		side1 = side1Chunk->cubes[uM][wM][v].cubeType;
		side2 = side2Chunk->cubes[uM][w][vP].cubeType;
		corner = cornerChunk->cubes[uM][wM][vP].cubeType;
		break;
	case 14:
		if (uRight) {
			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
				cornerChunk = &rightStack.stack[y];
			}
		}

		if (wFront) {
			if (y >= frontStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &frontStack.stack[y];
				cornerChunk = &frontStack.stack[y];
			}
		}

		if (vBottom) {
			if (y - 1 < 0) {
				return 3;
			}
			side1Chunk = &chunkStack.stack[y - 1];
			side2Chunk = &chunkStack.stack[y - 1];
			cornerChunk = &chunkStack.stack[y - 1];
		}

		if (uRight && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y - 1 >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y - 1];
				cornerChunk = &rightStack.stack[y - 1];
			}

			side2Chunk = &chunkStack.stack[y - 1];
		}

		if (wFront && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y - 1 >= frontStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &frontStack.stack[y - 1];
				cornerChunk = &frontStack.stack[y - 1];
			}

			side1Chunk = &chunkStack.stack[y - 1];
		}

		side1 = side1Chunk->cubes[uP][w][vM].cubeType;
		side2 = side2Chunk->cubes[u][wM][vM].cubeType;
		corner = cornerChunk->cubes[uP][wM][vM].cubeType;
		break;
	case 15:
		if (uLeft) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
				cornerChunk = &leftStack.stack[y];
			}
		}

		if (wBack) {
			if (y >= backStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &backStack.stack[y];
				cornerChunk = &backStack.stack[y];
			}
		}

		if (vBottom) {
			if (y - 1 < 0) {
				return 3;
			}
			side1Chunk = &chunkStack.stack[y - 1];
			side2Chunk = &chunkStack.stack[y - 1];
			cornerChunk = &chunkStack.stack[y - 1];
		}

		if (uLeft && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y - 1 >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y - 1];
				cornerChunk = &leftStack.stack[y - 1];
			}

			side2Chunk = &chunkStack.stack[y - 1];
		}

		if (wBack && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y - 1 >= backStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &backStack.stack[y - 1];
				cornerChunk = &backStack.stack[y - 1];
			}

			side1Chunk = &chunkStack.stack[y - 1];
		}

		side1 = side1Chunk->cubes[uM][w][vM].cubeType;
		side2 = side2Chunk->cubes[u][wP][vM].cubeType;
		corner = cornerChunk->cubes[uM][wP][vM].cubeType;
		break;
	case 13:
		if (uLeft) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
				cornerChunk = &leftStack.stack[y];
			}
		}

		if (wFront) {
			if (y >= frontStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &frontStack.stack[y];
				cornerChunk = &frontStack.stack[y];
			}
		}

		if (vBottom) {
			if (y - 1 < 0) {
				return 3;
			}
			side1Chunk = &chunkStack.stack[y - 1];
			side2Chunk = &chunkStack.stack[y - 1];
			cornerChunk = &chunkStack.stack[y - 1];
		}

		if (uLeft && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y - 1 >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y - 1];
				cornerChunk = &leftStack.stack[y - 1];
			}

			side2Chunk = &chunkStack.stack[y - 1];
		}

		if (wFront && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y - 1 >= frontStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &frontStack.stack[y - 1];
				cornerChunk = &frontStack.stack[y - 1];
			}

			side1Chunk = &chunkStack.stack[y - 1];
		}

		if (uLeft && wFront) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
			}

			if (y >= frontLeftStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &frontLeftStack.stack[y];
			}

			if (y >= frontStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &frontStack.stack[y];
			}
		}

		side1 = side1Chunk->cubes[uM][w][vM].cubeType;
		side2 = side2Chunk->cubes[u][wM][vM].cubeType;
		corner = cornerChunk->cubes[uM][wM][vM].cubeType;
		break;
	case 12:
		if (uRight) {
			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
				cornerChunk = &rightStack.stack[y];
			}
		}

		if (wBack) {
			if (y >= backStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &backStack.stack[y];
				cornerChunk = &backStack.stack[y];
			}
		}

		if (vBottom) {
			if (y - 1 < 0) {
				return 3;
			}
			side1Chunk = &chunkStack.stack[y - 1];
			side2Chunk = &chunkStack.stack[y - 1];
			cornerChunk = &chunkStack.stack[y - 1];
		}

		if (uRight && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y - 1 >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y - 1];
				cornerChunk = &rightStack.stack[y - 1];
			}

			side2Chunk = &chunkStack.stack[y - 1];
		}

		if (wBack && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y - 1 >= backStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &backStack.stack[y - 1];
				cornerChunk = &backStack.stack[y - 1];
			}

			side1Chunk = &chunkStack.stack[y - 1];
		}

		side1 = side1Chunk->cubes[uP][w][vM].cubeType;
		side2 = side2Chunk->cubes[u][wP][vM].cubeType;
		corner = cornerChunk->cubes[uP][wP][vM].cubeType;
		break;
	case 18:
		if (uRight) {
			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
				side2Chunk = &rightStack.stack[y];
				cornerChunk = &rightStack.stack[y];
			}
		}

		if (wFront) {
			if (y >= frontStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontStack.stack[y];
				cornerChunk = &frontStack.stack[y];
			}
		}

		if (vTop) {
			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
				cornerChunk = &chunkStack.stack[y + 1];
			}
		}

		if (uRight && vTop) {
			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
			}

			if (y + 1 >= rightStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &rightStack.stack[y + 1];
				cornerChunk = &rightStack.stack[y + 1];
			}
		}

		if (wFront && vTop) {
			if (y >= frontStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontStack.stack[y];
			}

			if (y + 1 >= frontStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &frontStack.stack[y + 1];
			}

			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
			}
		}

		side1 = side1Chunk->cubes[uP][wM][v].cubeType;
		side2 = side2Chunk->cubes[uP][w][vP].cubeType;
		corner = cornerChunk->cubes[uP][wM][vP].cubeType;
		break;
	case 19:
		if (uRight) {
			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
				side2Chunk = &rightStack.stack[y];
				cornerChunk = &rightStack.stack[y];
			}
		}

		if (wBack) {
			if (y >= backStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &backStack.stack[y];
				cornerChunk = &backStack.stack[y];
			}
		}

		if (vBottom) {
			if (y - 1 < 0) {
				return 3;
			}
			side2Chunk = &chunkStack.stack[y - 1];
			cornerChunk = &chunkStack.stack[y - 1];
		}

		if (uRight && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
			}

			if (y - 1 >= rightStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &rightStack.stack[y - 1];
				cornerChunk = &rightStack.stack[y - 1];
			}
		}

		if (wBack && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y >= backStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &backStack.stack[y];
			}

			if (y - 1 >= backStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &backStack.stack[y - 1];
			}

			side2Chunk = &chunkStack.stack[y - 1];
		}

		side1 = side1Chunk->cubes[uP][wP][v].cubeType;
		side2 = side2Chunk->cubes[uP][w][vM].cubeType;
		corner = cornerChunk->cubes[uP][wP][vM].cubeType;
		break;
	case 17:
		if (uRight) {
			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
				side2Chunk = &rightStack.stack[y];
				cornerChunk = &rightStack.stack[y];
			}
		}

		if (wFront) {
			if (y >= frontStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontStack.stack[y];
				cornerChunk = &frontStack.stack[y];
			}
		}

		if (vBottom) {
			if (y - 1 < 0) {
				return 3;
			}
			side2Chunk = &chunkStack.stack[y - 1];
			cornerChunk = &chunkStack.stack[y - 1];
		}

		if (uRight && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
			}

			if (y - 1 >= rightStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &rightStack.stack[y - 1];
				cornerChunk = &rightStack.stack[y - 1];
			}
		}

		if (wFront && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y >= frontStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontStack.stack[y];
			}

			if (y - 1 >= frontStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &frontStack.stack[y - 1];
			}

			side2Chunk = &chunkStack.stack[y - 1];
		}

		side1 = side1Chunk->cubes[uP][wM][v].cubeType;
		side2 = side2Chunk->cubes[uP][w][vM].cubeType;
		corner = cornerChunk->cubes[uP][wM][vM].cubeType;
		break;
	case 16:
		if (uRight) {
			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
				side2Chunk = &rightStack.stack[y];
				cornerChunk = &rightStack.stack[y];
			}
		}

		if (wBack) {
			if (y >= backStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &backStack.stack[y];
				cornerChunk = &backStack.stack[y];
			}
		}

		if (vTop) {
			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
				cornerChunk = &chunkStack.stack[y + 1];
			}
		}

		if (uRight && vTop) {
			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
			}

			if (y + 1 >= rightStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &rightStack.stack[y + 1];
				cornerChunk = &rightStack.stack[y + 1];
			}
		}

		if (wBack && vTop) {
			if (y >= backStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &backStack.stack[y];
			}

			if (y + 1 >= backStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &backStack.stack[y + 1];
			}

			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
			}
		}

		side1 = side1Chunk->cubes[uP][wP][v].cubeType;
		side2 = side2Chunk->cubes[uP][w][vP].cubeType;
		corner = cornerChunk->cubes[uP][wP][vP].cubeType;
		break;
	case 22:
		if (uLeft) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
				cornerChunk = &leftStack.stack[y];
			}
		}

		if (wFront) {
			if (y >= frontStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontStack.stack[y];
				side2Chunk = &frontStack.stack[y];
				cornerChunk = &frontStack.stack[y];
			}
		}

		if (vTop) {
			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
				cornerChunk = &chunkStack.stack[y + 1];
			}
		}

		if (uLeft && vTop) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
			}

			if (y + 1 >= leftStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &leftStack.stack[y + 1];
			}

			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
			}
		}

		if (wFront && vTop) {
			if (y >= frontStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontStack.stack[y];
			}

			if (y + 1 >= frontStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &frontStack.stack[y + 1];
				cornerChunk = &frontStack.stack[y + 1];
			}
		}

		if (uLeft && wFront) {
			if (y >= frontLeftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontLeftStack.stack[y];
				cornerChunk = &frontLeftStack.stack[y];
			}

			if (y >= frontStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &frontStack.stack[y];
			}
		}

		side1 = side1Chunk->cubes[uM][wM][v].cubeType;
		side2 = side2Chunk->cubes[u][wM][vP].cubeType;
		corner = cornerChunk->cubes[uM][wM][vP].cubeType;
		break;
	case 23:
		if (uRight) {
			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
				cornerChunk = &rightStack.stack[y];
			}
		}

		if (wFront) {
			if (y >= frontStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontStack.stack[y];
				side2Chunk = &frontStack.stack[y];
				cornerChunk = &frontStack.stack[y];
			}
		}

		if (vBottom) {
			if (y - 1 < 0) {
				return 3;
			}
			side2Chunk = &chunkStack.stack[y - 1];
			cornerChunk = &chunkStack.stack[y - 1];
		}

		if (uRight && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
			}

			if (y - 1 >= rightStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &rightStack.stack[y - 1];
			}

			side2Chunk = &chunkStack.stack[y - 1];
		}

		if (wFront && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y >= frontStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontStack.stack[y];
			}

			if (y - 1 >= frontStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &frontStack.stack[y - 1];
				cornerChunk = &frontStack.stack[y - 1];
			}
		}

		side1 = side1Chunk->cubes[uP][wM][v].cubeType;
		side2 = side2Chunk->cubes[u][wM][vM].cubeType;
		corner = cornerChunk->cubes[uP][wM][vM].cubeType;
		break;
	case 21:
		if (uLeft) {
			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
				cornerChunk = &leftStack.stack[y];
			}
		}

		if (wFront) {
			if (y >= frontStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontStack.stack[y];
				side2Chunk = &frontStack.stack[y];
				cornerChunk = &frontStack.stack[y];
			}
		}

		if (vBottom) {
			if (y - 1 < 0) {
				return 3;
			}
			side2Chunk = &chunkStack.stack[y - 1];
			cornerChunk = &chunkStack.stack[y - 1];
		}

		if (uLeft && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y >= leftStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &leftStack.stack[y];
			}

			if (y - 1 >= leftStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &leftStack.stack[y - 1];
			}

			side2Chunk = &chunkStack.stack[y - 1];
		}

		if (wFront && vBottom) {
			if (y - 1 < 0) {
				return 3;
			}

			if (y >= frontStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontStack.stack[y];
			}

			if (y - 1 >= frontStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &frontStack.stack[y - 1];
				cornerChunk = &frontStack.stack[y - 1];
			}
		}

		if (uLeft && wFront) {
			if (y >= frontLeftStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontLeftStack.stack[y];
				cornerChunk = &frontLeftStack.stack[y];
			}

			if (y >= frontStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &frontStack.stack[y];
			}
		}

		side1 = side1Chunk->cubes[uM][wM][v].cubeType;
		side2 = side2Chunk->cubes[u][wM][vM].cubeType;
		corner = cornerChunk->cubes[uM][wM][vM].cubeType;
		break;
	case 20:
		if (uRight) {
			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
				cornerChunk = &rightStack.stack[y];
			}
		}

		if (wFront) {
			if (y >= frontStack.stack.size()) {
				side1Chunk = &emptyChunk;
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontStack.stack[y];
				side2Chunk = &frontStack.stack[y];
				cornerChunk = &frontStack.stack[y];
			}
		}

		if (vTop) {
			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
				cornerChunk = &chunkStack.stack[y + 1];
			}
		}

		if (uRight && vTop) {
			if (y >= rightStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &rightStack.stack[y];
			}

			if (y + 1 >= rightStack.stack.size()) {
				cornerChunk = &emptyChunk;
			}
			else {
				cornerChunk = &rightStack.stack[y + 1];
			}

			if (y + 1 >= chunkStack.stack.size()) {
				side2Chunk = &emptyChunk;
			}
			else {
				side2Chunk = &chunkStack.stack[y + 1];
			}
		}

		if (wFront && vTop) {
			if (y >= frontStack.stack.size()) {
				side1Chunk = &emptyChunk;
			}
			else {
				side1Chunk = &frontStack.stack[y];
			}

			if (y + 1 >= frontStack.stack.size()) {
				side2Chunk = &emptyChunk;
				cornerChunk = &emptyChunk;
			}
			else {
				side2Chunk = &frontStack.stack[y + 1];
				cornerChunk = &frontStack.stack[y + 1];
			}
		}

		side1 = side1Chunk->cubes[uP][wM][v].cubeType;
		side2 = side2Chunk->cubes[u][wM][vP].cubeType;
		corner = cornerChunk->cubes[uP][wM][vP].cubeType;
		break;
	default:
		break;
	}


	return occlusion(side1, side2, corner);
}

uint8_t LoadedChunkStack::occlusion(CubeType const side1, CubeType const side2, CubeType const corner) {
	bool occludedSide1 = false;
	bool occludedSide2 = false;
	bool occludedCorner = false;

	if (side1 != CubeType::AIR) {// && side1 != CubeType::WATER) {
		occludedSide1 = true;
	}

	if (side2 != CubeType::AIR) {// && side2 != CubeType::WATER) {
		occludedSide2 = true;
	}

	if (corner != CubeType::AIR) {// && corner != CubeType::WATER) {
		occludedCorner = true;
	}

	if (occludedSide1 && occludedSide2) {
		return 0;
	}
	if (occludedSide1 && occludedCorner || occludedSide2 && occludedCorner) {
		return 1;
	}
	if (occludedSide1 || occludedSide2 || occludedCorner) {
		return 2;
	}

	return 3;
}

bool LoadedChunkStack::flippedTriangles(uint8_t const ambientOcclusionValue0, uint8_t const ambientOcclusionValue1, uint8_t const ambientOcclusiontValue2, uint8_t const ambientOcclusionValue3) {
	if (ambientOcclusionValue0 + ambientOcclusiontValue2 > ambientOcclusionValue1 + ambientOcclusionValue3) {
		return false;
	}

	return true;
}

struct LightPos {
	int u;
	int w;
	int v;
	int y;
};

void LoadedChunkStack::calculateLightLevels() {
	int8_t value = 15;
	int top = chunkStack.stack.size() - 1;

	std::queue<LightPos> queue;

	for (int u = 0; u < Settings::CHUNK_SIZE; u++) {
		for (int w = 0; w < Settings::CHUNK_SIZE; w++) {
			if (chunkStack.stack[top].cubes[u][w][Settings::CHUNK_SIZE - 1].cubeType == CubeType::AIR) {
				chunkStack.stack[top].lightLevel[u][w][Settings::CHUNK_SIZE - 1] = value;
				queue.push({ u, w, Settings::CHUNK_SIZE - 1, top });
			}
		}
	}

	int frontTop = frontStack.stack.size() - 1;
	std::queue<LightPos> frontQueue;
	for (int u = 0; u < Settings::CHUNK_SIZE; u++) {
		if (frontStack.stack[frontTop].cubes[u][Settings::CHUNK_SIZE - 1][Settings::CHUNK_SIZE - 1].cubeType == CubeType::AIR) {
			frontStack.stack[frontTop].lightLevel[u][Settings::CHUNK_SIZE - 1][Settings::CHUNK_SIZE - 1] = value;
			frontQueue.push({ u, Settings::CHUNK_SIZE - 1, Settings::CHUNK_SIZE - 1, frontTop });
		}
	}
	while (!frontQueue.empty()) {
		LightPos pos = frontQueue.front();
		frontQueue.pop();

		if (pos.v - 1 >= 0) {
			if (frontStack.stack[pos.y].cubes[pos.u][pos.w][pos.v - 1].cubeType == CubeType::AIR && frontStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v - 1] < frontStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
				frontStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v - 1] = frontStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v];
				frontQueue.emplace(LightPos{ pos.u, pos.w, pos.v - 1, pos.y });
			}
		}
		else {
			if (pos.y - 1 >= 0) {
				if (frontStack.stack[pos.y - 1].cubes[pos.u][pos.w][Settings::CHUNK_SIZE - 1].cubeType == CubeType::AIR && frontStack.stack[pos.y - 1].lightLevel[pos.u][pos.w][Settings::CHUNK_SIZE - 1] < frontStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
					frontStack.stack[pos.y - 1].lightLevel[pos.u][pos.w][Settings::CHUNK_SIZE - 1] = frontStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v];
					frontQueue.emplace(LightPos{ pos.u, pos.w, Settings::CHUNK_SIZE - 1, pos.y - 1 });
				}
			}
		}

		if (pos.w + 1 >= Settings::CHUNK_SIZE) {
			if (pos.y < chunkStack.stack.size()) {
				if (chunkStack.stack[pos.y].cubes[pos.u][0][pos.v].cubeType == CubeType::AIR && chunkStack.stack[pos.y].lightLevel[pos.u][0][pos.v] + 5 < frontStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
					chunkStack.stack[pos.y].lightLevel[pos.u][0][pos.v] = frontStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v] - 4;
					queue.emplace(LightPos{ pos.u, 0, pos.v, pos.y });
				}
			}
		}
	}

	int backTop = backStack.stack.size() - 1;
	std::queue<LightPos> backQueue;
	for (int u = 0; u < Settings::CHUNK_SIZE; u++) {
		if (backStack.stack[backTop].cubes[u][0][Settings::CHUNK_SIZE - 1].cubeType == CubeType::AIR) {
			backStack.stack[backTop].lightLevel[u][0][Settings::CHUNK_SIZE - 1] = value;
			backQueue.push({ u, 0, Settings::CHUNK_SIZE - 1, backTop });
		}
	}
	while (!backQueue.empty()) {
		LightPos pos = backQueue.front();
		backQueue.pop();

		if (pos.v - 1 >= 0) {
			if (backStack.stack[pos.y].cubes[pos.u][pos.w][pos.v - 1].cubeType == CubeType::AIR && backStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v - 1] < backStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
				backStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v - 1] = backStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v];
				backQueue.emplace(LightPos{ pos.u, pos.w, pos.v - 1, pos.y });
			}
		}
		else {
			if (pos.y - 1 >= 0) {
				if (backStack.stack[pos.y - 1].cubes[pos.u][pos.w][Settings::CHUNK_SIZE - 1].cubeType == CubeType::AIR && backStack.stack[pos.y - 1].lightLevel[pos.u][pos.w][Settings::CHUNK_SIZE - 1] < backStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
					backStack.stack[pos.y - 1].lightLevel[pos.u][pos.w][Settings::CHUNK_SIZE - 1] = backStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v];
					backQueue.emplace(LightPos{ pos.u, pos.w, Settings::CHUNK_SIZE - 1, pos.y - 1 });
				}
			}
		}

		if (pos.w - 1 < 0) {
			if (pos.y < chunkStack.stack.size()) {
				if (chunkStack.stack[pos.y].cubes[pos.u][Settings::CHUNK_SIZE - 1][pos.v].cubeType == CubeType::AIR && chunkStack.stack[pos.y].lightLevel[pos.u][Settings::CHUNK_SIZE - 1][pos.v] + 5 < backStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
					chunkStack.stack[pos.y].lightLevel[pos.u][Settings::CHUNK_SIZE - 1][pos.v] = backStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v] - 4;
					queue.emplace(LightPos{ pos.u, Settings::CHUNK_SIZE - 1, pos.v, pos.y });
				}
			}
		}
	}

	int leftTop = leftStack.stack.size() - 1;
	std::queue<LightPos> leftQueue;
	for (int w = 0; w < Settings::CHUNK_SIZE; w++) {
		if (leftStack.stack[leftTop].cubes[Settings::CHUNK_SIZE - 1][w][Settings::CHUNK_SIZE - 1].cubeType == CubeType::AIR) {
			leftStack.stack[leftTop].lightLevel[Settings::CHUNK_SIZE - 1][w][Settings::CHUNK_SIZE - 1] = value;
			leftQueue.push({ Settings::CHUNK_SIZE - 1, w, Settings::CHUNK_SIZE - 1, leftTop });
		}
	}
	while (!leftQueue.empty()) {
		LightPos pos = leftQueue.front();
		leftQueue.pop();

		if (pos.v - 1 >= 0) {
			if (leftStack.stack[pos.y].cubes[pos.u][pos.w][pos.v - 1].cubeType == CubeType::AIR && leftStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v - 1] < leftStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
				leftStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v - 1] = leftStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v];
				leftQueue.emplace(LightPos{ pos.u, pos.w, pos.v - 1, pos.y });
			}
		}
		else {
			if (pos.y - 1 >= 0) {
				if (leftStack.stack[pos.y - 1].cubes[pos.u][pos.w][Settings::CHUNK_SIZE - 1].cubeType == CubeType::AIR && leftStack.stack[pos.y - 1].lightLevel[pos.u][pos.w][Settings::CHUNK_SIZE - 1] < leftStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
					leftStack.stack[pos.y - 1].lightLevel[pos.u][pos.w][Settings::CHUNK_SIZE - 1] = leftStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v];
					leftQueue.emplace(LightPos{ pos.u, pos.w, Settings::CHUNK_SIZE - 1, pos.y - 1 });
				}
			}
		}

		if (pos.u + 1 >= Settings::CHUNK_SIZE) {
			if (pos.y < chunkStack.stack.size()) {
				if (chunkStack.stack[pos.y].cubes[0][pos.w][pos.v].cubeType == CubeType::AIR && chunkStack.stack[pos.y].lightLevel[0][pos.w][pos.v] + 5 < leftStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
					chunkStack.stack[pos.y].lightLevel[0][pos.w][pos.v] = leftStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v] - 4;
					queue.emplace(LightPos{ 0, pos.w, pos.v, pos.y });
				}
			}
		}
	}

	int rightTop = rightStack.stack.size() - 1;
	std::queue<LightPos> rightQueue;
	for (int w = 0; w < Settings::CHUNK_SIZE; w++) {
		if (rightStack.stack[rightTop].cubes[0][w][Settings::CHUNK_SIZE - 1].cubeType == CubeType::AIR) {
			rightStack.stack[rightTop].lightLevel[0][w][Settings::CHUNK_SIZE - 1] = value;
			rightQueue.push({ 0, w, Settings::CHUNK_SIZE - 1, rightTop });
		}
	}
	while (!rightQueue.empty()) {
		LightPos pos = rightQueue.front();
		rightQueue.pop();

		if (pos.v - 1 >= 0) {
			if (rightStack.stack[pos.y].cubes[pos.u][pos.w][pos.v - 1].cubeType == CubeType::AIR && rightStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v - 1] < rightStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
				rightStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v - 1] = rightStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v];
				rightQueue.emplace(LightPos{ pos.u, pos.w, pos.v - 1, pos.y });
			}
		}
		else {
			if (pos.y - 1 >= 0) {
				if (rightStack.stack[pos.y - 1].cubes[pos.u][pos.w][Settings::CHUNK_SIZE - 1].cubeType == CubeType::AIR && rightStack.stack[pos.y - 1].lightLevel[pos.u][pos.w][Settings::CHUNK_SIZE - 1] < rightStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
					rightStack.stack[pos.y - 1].lightLevel[pos.u][pos.w][Settings::CHUNK_SIZE - 1] = rightStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v];
					rightQueue.emplace(LightPos{ pos.u, pos.w, Settings::CHUNK_SIZE - 1, pos.y - 1 });
				}
			}
		}

		if (pos.u - 1 < 0) {
			if (pos.y < chunkStack.stack.size()) {
				if (chunkStack.stack[pos.y].cubes[Settings::CHUNK_SIZE - 1][pos.w][pos.v].cubeType == CubeType::AIR && chunkStack.stack[pos.y].lightLevel[Settings::CHUNK_SIZE - 1][pos.w][pos.v] + 5 < rightStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
					chunkStack.stack[pos.y].lightLevel[Settings::CHUNK_SIZE - 1][pos.w][pos.v] = rightStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v] - 4;
					queue.emplace(LightPos{ Settings::CHUNK_SIZE - 1, pos.w, pos.v, pos.y });
				}
			}
		}
	}

	while (!queue.empty()) {
		LightPos pos = queue.front();
		queue.pop();

		if (pos.v - 1 >= 0) {
			if (chunkStack.stack[pos.y].cubes[pos.u][pos.w][pos.v - 1].cubeType == CubeType::AIR && chunkStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v - 1] < chunkStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
				chunkStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v - 1] = chunkStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v];
				queue.emplace(LightPos{ pos.u, pos.w, pos.v - 1, pos.y });
			}
		}
		else {
			if (pos.y - 1 >= 0) {
				if (chunkStack.stack[pos.y - 1].cubes[pos.u][pos.w][Settings::CHUNK_SIZE - 1].cubeType == CubeType::AIR && chunkStack.stack[pos.y - 1].lightLevel[pos.u][pos.w][Settings::CHUNK_SIZE - 1] < chunkStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
					chunkStack.stack[pos.y - 1].lightLevel[pos.u][pos.w][Settings::CHUNK_SIZE - 1] = chunkStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v];
					queue.emplace(LightPos{ pos.u, pos.w, Settings::CHUNK_SIZE - 1, pos.y - 1 });
				}
			}
		}
		if (pos.u - 1 >= 0) {
			if (chunkStack.stack[pos.y].cubes[pos.u - 1][pos.w][pos.v].cubeType == CubeType::AIR && chunkStack.stack[pos.y].lightLevel[pos.u - 1][pos.w][pos.v] + 5 < chunkStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
				chunkStack.stack[pos.y].lightLevel[pos.u - 1][pos.w][pos.v] = chunkStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v] - 4;
				queue.emplace(LightPos{ pos.u - 1, pos.w, pos.v, pos.y });
			}
		}
		if (pos.u + 1 < Settings::CHUNK_SIZE) {
			if (chunkStack.stack[pos.y].cubes[pos.u + 1][pos.w][pos.v].cubeType == CubeType::AIR && chunkStack.stack[pos.y].lightLevel[pos.u + 1][pos.w][pos.v] + 5 < chunkStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
				chunkStack.stack[pos.y].lightLevel[pos.u + 1][pos.w][pos.v] = chunkStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v] - 4;
				queue.emplace(LightPos{ pos.u + 1, pos.w, pos.v, pos.y });
			}
		}
		if (pos.w - 1 >= 0) {
			if (chunkStack.stack[pos.y].cubes[pos.u][pos.w - 1][pos.v].cubeType == CubeType::AIR && chunkStack.stack[pos.y].lightLevel[pos.u][pos.w - 1][pos.v] + 5 < chunkStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
				chunkStack.stack[pos.y].lightLevel[pos.u][pos.w - 1][pos.v] = chunkStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v] - 4;
				queue.emplace(LightPos{ pos.u, pos.w - 1, pos.v, pos.y });
			}
		}
		if (pos.w + 1 < Settings::CHUNK_SIZE) {
			if (chunkStack.stack[pos.y].cubes[pos.u][pos.w + 1][pos.v].cubeType == CubeType::AIR && chunkStack.stack[pos.y].lightLevel[pos.u][pos.w + 1][pos.v] + 5 < chunkStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v]) {
				chunkStack.stack[pos.y].lightLevel[pos.u][pos.w + 1][pos.v] = chunkStack.stack[pos.y].lightLevel[pos.u][pos.w][pos.v] - 4;
				queue.emplace(LightPos{ pos.u, pos.w + 1, pos.v, pos.y });
			}
		}
	}
}

int8_t LoadedChunkStack::getLightLevel(int const u, int const w, int const v, int const y, int const side) {
	int8_t value = 0;

	switch (side) {
	case 0:
		if (v + 1 >= Settings::CHUNK_SIZE) {
			if (y + 1 >= chunkStack.stack.size()) {
				value = 15;
			}
			else {
				if (chunkStack.stack[y + 1].cubes[u][w][0].cubeType == CubeType::AIR) {
					value = chunkStack.stack[y + 1].lightLevel[u][w][0];
				}
			}
		}
		else {
			if (chunkStack.stack[y].cubes[u][w][v + 1].cubeType == CubeType::AIR) {
				value = chunkStack.stack[y].lightLevel[u][w][v + 1];
			}
		}
		break;
	case 1:
		if (w + 1 >= Settings::CHUNK_SIZE) {
			if (y >= backStack.stack.size()) {
				value = 15;
			}
			else {
				if (backStack.stack[y].cubes[u][0][v].cubeType == CubeType::AIR) {
					value = backStack.stack[y].lightLevel[u][0][v];
				}
			}
		}
		else {
			if (chunkStack.stack[y].cubes[u][w + 1][v].cubeType == CubeType::AIR) {
				value = chunkStack.stack[y].lightLevel[u][w + 1][v];
			}
		}
		break;
	case 2:
		if (u - 1 < 0) {
			if (y >= leftStack.stack.size()) {
				value = 15;
			}
			else {
				if (leftStack.stack[y].cubes[Settings::CHUNK_SIZE - 1][w][v].cubeType == CubeType::AIR) {
					value = leftStack.stack[y].lightLevel[Settings::CHUNK_SIZE - 1][w][v];
				}
			}
		}
		else {
			if (chunkStack.stack[y].cubes[u - 1][w][v].cubeType == CubeType::AIR) {
				value = chunkStack.stack[y].lightLevel[u - 1][w][v];
			}
		}
		break;
	case 3:
		if (v - 1 < 0) {
			if (y - 1 < 0) {
				value = 0;
			}
			else {
				if (chunkStack.stack[y - 1].cubes[u][w][Settings::CHUNK_SIZE - 1].cubeType == CubeType::AIR) {
					value = chunkStack.stack[y - 1].lightLevel[u][w][Settings::CHUNK_SIZE - 1];
				}
			}
		}
		else {
			if (chunkStack.stack[y].cubes[u][w][v - 1].cubeType == CubeType::AIR) {
				value = chunkStack.stack[y].lightLevel[u][w][v - 1];
			}
		}
		break;
	case 4:
		if (u + 1 >= Settings::CHUNK_SIZE) {
			if (y >= rightStack.stack.size()) {
				value = 15;
			}
			else {
				if (rightStack.stack[y].cubes[0][w][v].cubeType == CubeType::AIR) {
					value = rightStack.stack[y].lightLevel[0][w][v];
				}
			}
		}
		else {
			if (chunkStack.stack[y].cubes[u + 1][w][v].cubeType == CubeType::AIR) {
				value = chunkStack.stack[y].lightLevel[u + 1][w][v];
			}
		}
		break;
	case 5:
		if (w - 1 < 0) {
			if (y >= frontStack.stack.size()) {
				value = 15;
			}
			else {
				if (frontStack.stack[y].cubes[u][Settings::CHUNK_SIZE - 1][v].cubeType == CubeType::AIR) {
					value = frontStack.stack[y].lightLevel[u][Settings::CHUNK_SIZE - 1][v];
				}
			}
		}
		else {
			if (chunkStack.stack[y].cubes[u][w - 1][v].cubeType == CubeType::AIR) {
				value = chunkStack.stack[y].lightLevel[u][w - 1][v];
			}
		}
		break;
	default:
		break;
	}

	return value;
}

bool LoadedChunkStack::cullSide(int const u, int const w, int const v, int const y, int const side) {
	switch (side) {
	case 0:
		if (v + 1 >= Settings::CHUNK_SIZE) {
			if (y + 1 >= chunkStack.stack.size()) {
				return false;
			}
			else {
				if (chunkStack.stack[y + 1].cubes[u][w][0].cubeType == CubeType::AIR) {
					return false;
				}
			}
		}
		else {
			if (chunkStack.stack[y].cubes[u][w][v + 1].cubeType == CubeType::AIR) {
				return false;
			}
		}
		break;
	case 1:
		if (w + 1 >= Settings::CHUNK_SIZE) {
			if (y >= backStack.stack.size()) {
				return false;
			}
			else {
				if (backStack.stack[y].cubes[u][0][v].cubeType == CubeType::AIR) {
					return false;
				}
			}
		}
		else {
			if (chunkStack.stack[y].cubes[u][w + 1][v].cubeType == CubeType::AIR) {
				return false;
			}
		}
		break;
	case 2:
		if (u - 1 < 0) {
			if (y >= leftStack.stack.size()) {
				return false;
			}
			else {
				if (leftStack.stack[y].cubes[Settings::CHUNK_SIZE - 1][w][v].cubeType == CubeType::AIR) {
					return false;
				}
			}
		}
		else {
			if (chunkStack.stack[y].cubes[u - 1][w][v].cubeType == CubeType::AIR) {
				return false;
			}
		}
		break;
	case 3:
		if (v - 1 < 0) {
			if (y - 1 < 0) {
				return false;
			}
			else {
				if (chunkStack.stack[y - 1].cubes[u][w][Settings::CHUNK_SIZE - 1].cubeType == CubeType::AIR) {
					return false;
				}
			}
		}
		else {
			if (chunkStack.stack[y].cubes[u][w][v - 1].cubeType == CubeType::AIR) {
				return false;
			}
		}
		break;
	case 4:
		if (u + 1 >= Settings::CHUNK_SIZE) {
			if (y >= rightStack.stack.size()) {
				return false;
			}
			else {
				if (rightStack.stack[y].cubes[0][w][v].cubeType == CubeType::AIR) {
					return false;
				}
			}
		}
		else {
			if (chunkStack.stack[y].cubes[u + 1][w][v].cubeType == CubeType::AIR) {
				return false;
			}
		}
		break;
	case 5:
		if (w - 1 < 0) {
			if (y >= frontStack.stack.size()) {
				return false;
			}
			else {
				if (frontStack.stack[y].cubes[u][Settings::CHUNK_SIZE - 1][v].cubeType == CubeType::AIR) {
					return false;
				}
			}
		}
		else {
			if (chunkStack.stack[y].cubes[u][w - 1][v].cubeType == CubeType::AIR) {
				return false;
			}
		}
		break;
	default:
		break;
	}

	return true;
}

void LoadedChunkStack::greedyMesh1D(std::vector<Quad> &cubeSideQuads, int const side, bool const isX) {
	if (cubeSideQuads.empty()) {
		return;
	}

	int leftTop = 0;
	int leftBottom = 0;
	int rightTop = 0;
	int rightBottom = 0;

	if (isX) {
		if (side < 3) {
			leftTop = 0;
			leftBottom = 3;
			rightTop = 2;
			rightBottom = 1;
		}
		else {
			leftTop = 2;
			leftBottom = 1;
			rightTop = 0;
			rightBottom = 3;
		}
	} else {
		if (side < 3) {
			leftTop = 0;
			leftBottom = 2;
			rightTop = 3;
			rightBottom = 1;
		}
		else {
			leftTop = 3;
			leftBottom = 1;
			rightTop = 0;
			rightBottom = 2;
		}
	}

	std::sort(cubeSideQuads.begin(), cubeSideQuads.end());

	std::vector<Quad> meshedQuads;
	meshedQuads.push_back(cubeSideQuads[0]);

	for (int i = 1; i < cubeSideQuads.size(); i++) {
		bool added = false;

		for (int j = 0; j < meshedQuads.size(); j++) {
			if (fitsTogether(meshedQuads[j], cubeSideQuads[i], side, isX)) {
				float temp = 0.0f;
				float wrap = 1.0f;

				if (side > 2) {
					wrap = -1.0f;
				}

				if (isX) {
					temp = meshedQuads[j].vertices[rightTop].textureCoordinate.s;
				} else {
					temp = meshedQuads[j].vertices[rightTop].textureCoordinate.t;
				}

				meshedQuads[j].vertices[rightTop] = cubeSideQuads[i].vertices[rightTop];
				meshedQuads[j].vertices[rightBottom] = cubeSideQuads[i].vertices[rightBottom];

				if (isX) {
					meshedQuads[j].vertices[rightTop].textureCoordinate.s = temp + wrap;
					meshedQuads[j].vertices[rightBottom].textureCoordinate.s = temp + wrap;
				} else {
					meshedQuads[j].vertices[rightTop].textureCoordinate.t = temp + wrap;
					meshedQuads[j].vertices[rightBottom].textureCoordinate.t = temp + wrap;
				}

				added = true;
				break;
			}
		}

		if (!added) {
			meshedQuads.emplace_back(cubeSideQuads[i]);
		}
	}

	cubeSideQuads = meshedQuads;
}

bool LoadedChunkStack::fitsTogether(Quad const &main, Quad const &addition, int const side, bool const isX) {
	int leftTop = 0;
	int leftBottom = 0;
	int rightTop = 0;
	int rightBottom = 0;

	if (isX) {
		if (side < 3) {
			leftTop = 0;
			leftBottom = 3;
			rightTop = 2;
			rightBottom = 1;
		}
		else {
			leftTop = 2;
			leftBottom = 1;
			rightTop = 0;
			rightBottom = 3;
		}
	}
	else {
		if (side < 3) {
			leftTop = 0;
			leftBottom = 2;
			rightTop = 3;
			rightBottom = 1;
		}
		else {
			leftTop = 3;
			leftBottom = 1;
			rightTop = 0;
			rightBottom = 2;
		}
	}

	if (main.vertices[0].textureID != addition.vertices[0].textureID) {
		return false;
	}

	if (main.vertices[0].lightLevel != addition.vertices[0].lightLevel) {
		return false;
	}

	if (main.vertices[leftTop].ambientOcclusionValue != main.vertices[rightTop].ambientOcclusionValue) {
		return false;
	}

	if (main.vertices[leftBottom].ambientOcclusionValue != main.vertices[rightBottom].ambientOcclusionValue) {
		return false;
	}

	if (addition.vertices[leftTop].ambientOcclusionValue != addition.vertices[rightTop].ambientOcclusionValue) {
		return false;
	}

	if (addition.vertices[leftBottom].ambientOcclusionValue != addition.vertices[rightBottom].ambientOcclusionValue) {
		return false;
	}

	if (main.vertices[rightTop] == addition.vertices[leftTop] && main.vertices[rightBottom] == addition.vertices[leftBottom]) {
		return true;
	}

	return false;
}

void LoadedChunkStack::greedyMesh2D(std::vector<Quad> &cubeSideQuads, int const side) {
	greedyMesh1D(cubeSideQuads, side, true);
	greedyMesh1D(cubeSideQuads, side, false);

	/*std::vector<Quad> q1 = cubeSideQuads;
	greedyMesh1D(q1, side, true);
	greedyMesh1D(q1, side, false);

	std::vector<Quad> q2 = cubeSideQuads;
	greedyMesh1D(q2, side, false);
	greedyMesh1D(q2, side, true);

	if (q1.size() < q2.size()) {
		cubeSideQuads = q1;
		if (q1.size() != 0)
		std::cout << "Saved " << (q2.size() - q1.size()) * 2 << " triangles" << std::endl;
	}
	else {
		cubeSideQuads = q2;
		if (q2.size() != 0)
		std::cout << "Saved " << (q1.size() - q2.size()) * 2 << " triangles" << std::endl;
	}*/
}
