#ifndef MAPGENERATOR_H
#define MAPGENERATOR_H

#include "PerlinNoise.h"
#include "Settings.h"
#include "Chunk.h"
#include "ChunkStack.h"
#include "BiomData.h"

class MapGenerator {
public:
	MapGenerator();
	~MapGenerator();

	void generateChunkHeight(int const x, int const z, ChunkStack &chunkStack);

	void perlinNoiseImage();

private:
	PerlinNoise perlinNoise;

	void getInterpolatedBiom(float const moisture, float const temperature, BiomData &biomData);

	float lerp(float const a, float const b, float const weight);

	bool ceckChunkBorder(std::vector<std::pair<glm::vec3, Cube>> const &treeCubes, glm::vec3 const position);
};

#endif // !MAPGENERATOR_H
