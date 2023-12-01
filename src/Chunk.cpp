#include "Chunk.h"
#include "CubeType.h"

Chunk::Chunk() {
	for (size_t x = 0; x < Settings::CHUNK_SIZE; x++) {
		for (size_t y = 0; y < 1; y++) {
			for (size_t z = 0; z < Settings::CHUNK_SIZE; z++) {
				cubes[x][y][z] = Cube(CubeType::AIR);
			}
		}
	}

	for (size_t x = 0; x < Settings::CHUNK_SIZE; x++) {
		for (size_t y = 0; y < Settings::CHUNK_SIZE; y++) {
			for (size_t z = 0; z < Settings::CHUNK_SIZE; z++) {
				objData[x][y][z] = ObjData(ObjType::EMPTY, 0.0f, 0.0f, 0.0f);
			}
		}
	}

	for (size_t x = 0; x < Settings::CHUNK_SIZE; x++) {
		for (size_t y = 0; y < 1; y++) {
			for (size_t z = 0; z < Settings::CHUNK_SIZE; z++) {
				lightLevel[x][y][z] = 0;
			}
		}
	}
}

Chunk::~Chunk() {}