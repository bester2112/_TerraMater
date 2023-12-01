#include "MapGenerator.h"
#include "FileWriter.h"
#include "CubeType.h"
#include "TreeDescription.h"
#include "TreeBuilder.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

MapGenerator::MapGenerator() {}

MapGenerator::~MapGenerator() {}

void MapGenerator::generateChunkHeight(int const x, int const z, ChunkStack &chunkStack) {
	for (size_t w = 0; w < Settings::CHUNK_SIZE; w++) {
		for (size_t u = 0; u < Settings::CHUNK_SIZE; u++) {
			float s = x + (float)u / Settings::CHUNK_SIZE;
			float t = z + (float)w / Settings::CHUNK_SIZE;

			float moisture = perlinNoise.combinedOctaves(s, t, Settings::BIOM_SIZE, 2.0f, 1.0f, 0.5f, 2);

			float temperature = perlinNoise.invert(perlinNoise.domainWarpingCombinedOctaves(s, t, Settings::BIOM_SIZE / 2.0f, 2.0f, 1.0f, 0.5f, 2));

			BiomData biomData;
			getInterpolatedBiom(moisture, temperature, biomData);

			moisture = perlinNoise.combinedOctaves(s, t, Settings::BIOM_SIZE, 2.0f, 1.0f, 0.5f, 10);
			temperature = perlinNoise.invert(perlinNoise.domainWarpingCombinedOctaves(s, t, Settings::BIOM_SIZE / 2.0f, 2.0f, 1.0f, 0.5f, 10));

			float heightNoise = perlinNoise.invert(perlinNoise.domainWarpingCombinedOctaves(s, t, biomData.frequency, 2.0f, biomData.amplitude, 0.5f, (int)biomData.octaveCount));

			float elevation = moisture = perlinNoise.combinedOctaves(t, s, Settings::BIOM_SIZE, 2.0f, 1.0f, 0.5f, 10);

			int height = (heightNoise * 0.5f + 0.5f) * biomData.heightUsage * Settings::MAX_HEIGHT -Settings::WATER_LEVEL * (elevation * 0.5f + 0.5f);

			if (height < Settings::MIN_HEIGHT) {
				height = Settings::MIN_HEIGHT;
			}

			int chunkYIndex = height / Settings::CHUNK_SIZE + 1;

			if (chunkStack.stack.size() < chunkYIndex) {
				chunkStack.stack.resize(chunkYIndex);
			}

			//Setting up the lowest layer
			chunkStack.stack[0].cubes[u][w][0] = Cube(CubeType::BEDROCK);

			for (int v = 1; v < height; v++) {
				int stackIndex = v / Settings::CHUNK_SIZE;
				int chunkIndex = v % Settings::CHUNK_SIZE;

				if (v < height * 0.8f) {
					//Filling the bottom with stone
					chunkStack.stack[stackIndex].cubes[u][w][chunkIndex] = Cube(CubeType::STONE);
				}
				else {
					//Setting the rest as default to dirt
					chunkStack.stack[stackIndex].cubes[u][w][chunkIndex] = Cube(CubeType::DIRT);

					//If the terrain uses alot of height, setting the cubes bilding this mountain to stone
					if (biomData.heightUsage > 0.95f) {
						chunkStack.stack[stackIndex].cubes[u][w][chunkIndex] = Cube(CubeType::STONE);
					}

					//Checking if we have desert
					if (temperature > 0.0f && moisture > 0.0f && height < 90) {
						chunkStack.stack[stackIndex].cubes[u][w][chunkIndex] = Cube(CubeType::SAND);
					}

					//Setting up the beaches
					if (v > 61 && v < 66 && height > 61 && height < 66) {
						chunkStack.stack[stackIndex].cubes[u][w][chunkIndex] = Cube(CubeType::SAND);
					}

					//If under water fill with sand
					if (height < Settings::WATER_LEVEL) {
						chunkStack.stack[stackIndex].cubes[u][w][chunkIndex] = Cube(CubeType::SAND);
					}
				}
			}

			int stackIndex = (height - 1) / Settings::CHUNK_SIZE;
			int chunkIndex = (height - 1) % Settings::CHUNK_SIZE;

			//Setting the top layer, if it's not beach
			if (chunkStack.stack[stackIndex].cubes[u][w][chunkIndex].cubeType != CubeType::SAND) {
				//Deciding between dark or normal grass
				if (temperature > 0.0f && moisture < 0.0f) {
					chunkStack.stack[stackIndex].cubes[u][w][chunkIndex] = Cube(CubeType::GRASS_BLOCK);
				} else {
					chunkStack.stack[stackIndex].cubes[u][w][chunkIndex] = Cube(CubeType::DARK_GRASS_BLOCK);
				}

				//Adding snow on the mountain tops
				if (temperature < 0.0f && height > 120) {
					RandomSampler randomSampler = RandomSampler();
					//if (randomSampler.getSample1D(x + u, z + w) < (height - 120) * 0.0625f + (temperature * 0.5f + 0.5f)) {
					if (randomSampler.getSample1D(x * Settings::CHUNK_SIZE + u, z * Settings::CHUNK_SIZE + w) < (height - 120) * 0.0625f + (temperature * 0.5f + 0.5f)) {
						chunkStack.stack[stackIndex].cubes[u][w][chunkIndex] = Cube(CubeType::SNOW);
					}
				}
			}

			//Raising the water
			if (height < Settings::WATER_LEVEL) {
				if (chunkStack.stack.size() < Settings::WATER_LEVEL / Settings::CHUNK_SIZE) {
					chunkStack.stack.resize(Settings::WATER_LEVEL / Settings::CHUNK_SIZE);
				}

				for (int v = height; v < Settings::WATER_LEVEL; v++) {
					chunkStack.stack[v / Settings::CHUNK_SIZE].cubes[u][w][v % Settings::CHUNK_SIZE] = Cube(CubeType::WATER);
				}
			}

			//Planting trees
			if (height > Settings::WATER_LEVEL) {
				if (temperature < 0.0f && height > 120) {
					continue;
				}

				RandomSampler randomSampler = RandomSampler();
				randomSampler.resetSampler(x * Settings::CHUNK_SIZE + u, z * Settings::CHUNK_SIZE + w);
				
				TreeDescription treeDescription;

				if (temperature > 0.0f && moisture < 0.0f) {
					biomData.treeDensity = Settings::plainsData.treeDensity;
					treeDescription = Settings::simpleOakTreeDescription;

					float r1 = randomSampler.getSample1DNonReset();

					if (r1 <= 0.25f) {
						treeDescription.trunkType = CubeType::OAK_LOG;
						treeDescription.leafType = CubeType::OAK_LEAVES;
					} else if (r1 <= 0.5f) {
						treeDescription.trunkType = CubeType::DARK_OAK_LOG;
						treeDescription.leafType = CubeType::DARK_OAK_LEAVES;
					} else if (r1 <= 0.75f) {
						treeDescription.trunkType = CubeType::BIRCH_LOG;
						treeDescription.leafType = CubeType::BIRCH_LEAVES;
					} else if (r1 <= 1.0f) {
						treeDescription.trunkType = CubeType::ACACIA_LOG;
						treeDescription.leafType = CubeType::ACACIA_LEAVES;
					}
				}
				else {
					biomData.treeDensity = Settings::forestData.treeDensity;
					treeDescription = Settings::simpleFirTreeDescription;
				}

				if (biomData.heightUsage > 0.95f) {
					biomData.treeDensity = Settings::mountainData.treeDensity;
					treeDescription = Settings::simpleFirTreeDescription;
				}

				if (temperature > 0.0f && moisture > 0.0f && height < 90) {
					biomData.treeDensity = Settings::desertData.treeDensity;
					treeDescription = Settings::simpleCactusDescription;
				}

				if (randomSampler.getSample1DNonReset() < biomData.treeDensity) {
					glm::vec3 position = glm::vec3(u, height, w);

					std::vector<std::pair<glm::vec3, Cube>> treeCubes;
					TreeBuilder::generateTreeCubes(treeDescription, randomSampler, treeCubes);

					if (ceckChunkBorder(treeCubes, position)) {

						for (size_t i = 0; i < treeCubes.size(); i++) {
							glm::vec3 cubePosition = position + treeCubes[i].first;

							int cubeStackIndex = (int)cubePosition.y / Settings::CHUNK_SIZE;
							int cubeChunkIndex = (int)cubePosition.y % Settings::CHUNK_SIZE;
							int cubeU = (int)cubePosition.x;
							int cubeW = (int)cubePosition.z;

							if (chunkStack.stack.size() < cubeStackIndex + 1) {
								chunkStack.stack.resize(cubeStackIndex + 1);
							}

							if (chunkStack.stack[cubeStackIndex].cubes[cubeU][cubeW][cubeChunkIndex].cubeType == CubeType::AIR || chunkStack.stack[cubeStackIndex].cubes[cubeU][cubeW][cubeChunkIndex].cubeType == CubeType::OAK_LEAVES) {
								chunkStack.stack[cubeStackIndex].cubes[cubeU][cubeW][cubeChunkIndex] = Cube(treeCubes[i].second.cubeType);
							}
						}

					}
				} else if (randomSampler.getSample1DNonReset() < biomData.flowerDensity) {
					ObjType objType = ObjType::EMPTY;

					float xOffset = randomSampler.getSample1DNonReset() * 0.8f - 0.4f;
					float zOffset = randomSampler.getSample1DNonReset() * 0.8f - 0.4f;
					float yRotation = randomSampler.getSample1DNonReset() * 2.0f * M_PI;

					if (height > 61 && height < 66) {
						objType = ObjType::SUCCULENT;
					} else {
						if (temperature > 0.0f && moisture < 0.0f) {
							objType = ObjType::SUNFLOWER;
							yRotation = 0.0f;
						}
						else {
							objType = ObjType::LOTUS;
						}
					}

					//Checking if we have desert
					if (temperature > 0.0f && moisture > 0.0f && height < 90) {
						objType = ObjType::SUCCULENT;
					}

					glm::vec3 position = glm::vec3(u, height, w);

					int cubeStackIndex = (int)position.y / Settings::CHUNK_SIZE;
					int cubeChunkIndex = (int)position.y % Settings::CHUNK_SIZE;
					int cubeU = (int)position.x;
					int cubeW = (int)position.z;

					if (chunkStack.stack.size() < cubeStackIndex + 1) {
						chunkStack.stack.resize(cubeStackIndex + 1);
					}

					chunkStack.stack[cubeStackIndex].objData[cubeU][cubeW][cubeChunkIndex] = ObjData(objType, xOffset, zOffset, yRotation);
				}
			}
		}
	}
}

void MapGenerator::perlinNoiseImage() {
	size_t size = Settings::LOADED_CHUNKS * Settings::LOADED_CHUNKS * Settings::CHUNK_SIZE * Settings::CHUNK_SIZE;

	std::vector<float> moistureValues;
	moistureValues.resize(size);

	std::vector<float> temperatureValues;
	temperatureValues.resize(size);

	std::vector<float> heightValues;
	heightValues.resize(size);

	for (size_t z = 0; z < Settings::LOADED_CHUNKS; z++) {
		for (size_t x = 0; x < Settings::LOADED_CHUNKS; x++) {
			for (size_t w = 0; w < Settings::CHUNK_SIZE; w++) {
				for (size_t u = 0; u < Settings::CHUNK_SIZE; u++) {
					float s = x + (float)u / Settings::CHUNK_SIZE;
					float t = z + (float)w / Settings::CHUNK_SIZE;

					float moisture = perlinNoise.combinedOctaves(s, t, Settings::BIOM_SIZE, 2.0f, 1.0f, 0.5f, 2);
					moistureValues[x * Settings::CHUNK_SIZE + z * Settings::LOADED_CHUNKS * Settings::CHUNK_SIZE * Settings::CHUNK_SIZE + w * Settings::LOADED_CHUNKS * Settings::CHUNK_SIZE + u] = moisture * 0.5f + 0.5f;

					float temperature = perlinNoise.invert(perlinNoise.domainWarpingCombinedOctaves(s, t, Settings::BIOM_SIZE / 2.0f, 2.0f, 1.0f, 0.5f, 2));
					temperatureValues[x * Settings::CHUNK_SIZE + z * Settings::LOADED_CHUNKS * Settings::CHUNK_SIZE * Settings::CHUNK_SIZE + w * Settings::LOADED_CHUNKS * Settings::CHUNK_SIZE + u] = temperature * 0.5f + 0.5f;

					BiomData biomData;
					CubeType cubeType;
					getInterpolatedBiom(moisture, temperature, biomData);

					float heightNoise = perlinNoise.invert(perlinNoise.domainWarpingCombinedOctaves(s, t, biomData.frequency, 2.0f, biomData.amplitude, 0.5f, (int)biomData.octaveCount));
					heightValues[x * Settings::CHUNK_SIZE + z * Settings::LOADED_CHUNKS * Settings::CHUNK_SIZE * Settings::CHUNK_SIZE + w * Settings::LOADED_CHUNKS * Settings::CHUNK_SIZE + u] = heightNoise * 0.5f + 0.5f;
				}
			}
		}
	}

	FileWriter::writeGreyScalePPM("moisture.ppm", Settings::LOADED_CHUNKS * Settings::CHUNK_SIZE, Settings::LOADED_CHUNKS * Settings::CHUNK_SIZE, moistureValues);
	FileWriter::writeGreyScalePPM("temperature.ppm", Settings::LOADED_CHUNKS * Settings::CHUNK_SIZE, Settings::LOADED_CHUNKS * Settings::CHUNK_SIZE, temperatureValues);
	FileWriter::writeGreyScalePPM("height.ppm", Settings::LOADED_CHUNKS * Settings::CHUNK_SIZE, Settings::LOADED_CHUNKS * Settings::CHUNK_SIZE, heightValues);
}

void MapGenerator::getInterpolatedBiom(float const moisture, float const temperature, BiomData &biomData) {
	float moistureWeight = (moisture * 0.5f + 0.5f) * 2.0f - 0.5f;
	float temperatureWeight = (temperature * 0.5f + 0.5f) * 2.0f - 0.5f;

	if (moisture < -0.5f) {
		if (temperature < -0.5f) {
			//Lower left corner so it will be forest
			biomData = Settings::forestData;
			return;
		}
		else if (temperature > 0.5f) {
			//Higher left corner so it will be plains
			biomData = Settings::plainsData;
			return;
		}
		else {
			//Left so blending between forest and plains
			biomData.amplitude = lerp(Settings::forestData.amplitude, Settings::plainsData.amplitude, temperatureWeight);
			biomData.frequency = lerp(Settings::forestData.frequency, Settings::plainsData.frequency, temperatureWeight);
			biomData.octaveCount = lerp(Settings::forestData.octaveCount, Settings::plainsData.octaveCount, temperatureWeight);
			biomData.heightUsage = lerp(Settings::forestData.heightUsage, Settings::plainsData.heightUsage, temperatureWeight);
			biomData.treeDensity = lerp(Settings::forestData.treeDensity, Settings::plainsData.treeDensity, temperatureWeight);
			biomData.grasDensity = lerp(Settings::forestData.grasDensity, Settings::plainsData.grasDensity, temperatureWeight);
			biomData.flowerDensity = lerp(Settings::forestData.flowerDensity, Settings::plainsData.flowerDensity, temperatureWeight);
			return;
		}
	}
	else if (moisture > 0.5f) {
		if (temperature < -0.5f) {
			//Lower right corner so it will be mountains
			biomData = Settings::mountainData;
			return;
		}
		else if (temperature > 0.5f) {
			//Higher right corner so it will be desert
			biomData = Settings::desertData;
			return;
		}
		else {
			//Right so blending between mountains and desert
			biomData.amplitude = lerp(Settings::mountainData.amplitude, Settings::desertData.amplitude, temperatureWeight);
			biomData.frequency = lerp(Settings::mountainData.frequency, Settings::desertData.frequency, temperatureWeight);
			biomData.octaveCount = lerp(Settings::mountainData.octaveCount, Settings::desertData.octaveCount, temperatureWeight);
			biomData.heightUsage = lerp(Settings::mountainData.heightUsage, Settings::desertData.heightUsage, temperatureWeight);
			biomData.treeDensity = lerp(Settings::mountainData.treeDensity, Settings::desertData.treeDensity, temperatureWeight);
			biomData.grasDensity = lerp(Settings::mountainData.grasDensity, Settings::desertData.grasDensity, temperatureWeight);
			biomData.flowerDensity = lerp(Settings::mountainData.flowerDensity, Settings::desertData.flowerDensity, temperatureWeight);
			return;
		}
	}

	if (temperature < -0.5f) {
		//Lower so blending between forest and mountain
		biomData.amplitude = lerp(Settings::forestData.amplitude, Settings::mountainData.amplitude, moistureWeight);
		biomData.frequency = lerp(Settings::forestData.frequency, Settings::mountainData.frequency, moistureWeight);
		biomData.octaveCount = lerp(Settings::forestData.octaveCount, Settings::mountainData.octaveCount, moistureWeight);
		biomData.heightUsage = lerp(Settings::forestData.heightUsage, Settings::mountainData.heightUsage, moistureWeight);
		biomData.treeDensity = lerp(Settings::forestData.treeDensity, Settings::mountainData.treeDensity, temperatureWeight);
		biomData.grasDensity = lerp(Settings::forestData.grasDensity, Settings::mountainData.grasDensity, temperatureWeight);
		biomData.flowerDensity = lerp(Settings::forestData.flowerDensity, Settings::mountainData.flowerDensity, temperatureWeight);
		return;
	}
	else if (temperature > 0.5f) {
		//Higher so blending between plains and desert
		biomData.amplitude = lerp(Settings::plainsData.amplitude, Settings::desertData.amplitude, moistureWeight);
		biomData.frequency = lerp(Settings::plainsData.frequency, Settings::desertData.frequency, moistureWeight);
		biomData.octaveCount = lerp(Settings::plainsData.octaveCount, Settings::desertData.octaveCount, moistureWeight);
		biomData.heightUsage = lerp(Settings::plainsData.heightUsage, Settings::desertData.heightUsage, moistureWeight);
		biomData.treeDensity = lerp(Settings::plainsData.treeDensity, Settings::desertData.treeDensity, temperatureWeight);
		biomData.grasDensity = lerp(Settings::plainsData.grasDensity, Settings::desertData.grasDensity, temperatureWeight);
		biomData.flowerDensity = lerp(Settings::plainsData.flowerDensity, Settings::desertData.flowerDensity, temperatureWeight);
		return;
	}

	//Blending between all 4 for the middle
	BiomData biomDataLowTemperature;
	biomDataLowTemperature.amplitude = lerp(Settings::forestData.amplitude, Settings::mountainData.amplitude, moistureWeight);
	biomDataLowTemperature.frequency = lerp(Settings::forestData.frequency, Settings::mountainData.frequency, moistureWeight);
	biomDataLowTemperature.octaveCount = lerp(Settings::forestData.octaveCount, Settings::mountainData.octaveCount, moistureWeight);
	biomDataLowTemperature.heightUsage = lerp(Settings::forestData.heightUsage, Settings::mountainData.heightUsage, moistureWeight);
	biomDataLowTemperature.treeDensity = lerp(Settings::forestData.treeDensity, Settings::mountainData.treeDensity, temperatureWeight);
	biomDataLowTemperature.grasDensity = lerp(Settings::forestData.grasDensity, Settings::mountainData.grasDensity, temperatureWeight);
	biomDataLowTemperature.flowerDensity = lerp(Settings::forestData.flowerDensity, Settings::mountainData.flowerDensity, temperatureWeight);

	BiomData biomDataHighTemperature;
	biomDataHighTemperature.amplitude = lerp(Settings::plainsData.amplitude, Settings::desertData.amplitude, moistureWeight);
	biomDataHighTemperature.frequency = lerp(Settings::plainsData.frequency, Settings::desertData.frequency, moistureWeight);
	biomDataHighTemperature.octaveCount = lerp(Settings::plainsData.octaveCount, Settings::desertData.octaveCount, moistureWeight);
	biomDataHighTemperature.heightUsage = lerp(Settings::plainsData.heightUsage, Settings::desertData.heightUsage, moistureWeight);
	biomDataHighTemperature.treeDensity = lerp(Settings::plainsData.treeDensity, Settings::desertData.treeDensity, temperatureWeight);
	biomDataHighTemperature.grasDensity = lerp(Settings::plainsData.grasDensity, Settings::desertData.grasDensity, temperatureWeight);
	biomDataHighTemperature.flowerDensity = lerp(Settings::plainsData.flowerDensity, Settings::desertData.flowerDensity, temperatureWeight);

	biomData.amplitude = lerp(biomDataLowTemperature.amplitude, biomDataHighTemperature.amplitude, temperatureWeight);
	biomData.frequency = lerp(biomDataLowTemperature.frequency, biomDataHighTemperature.frequency, temperatureWeight);
	biomData.octaveCount = lerp(biomDataLowTemperature.octaveCount, biomDataHighTemperature.octaveCount, temperatureWeight);
	biomData.heightUsage = lerp(biomDataLowTemperature.heightUsage, biomDataHighTemperature.heightUsage, temperatureWeight);
	biomData.treeDensity = lerp(biomDataLowTemperature.treeDensity, biomDataHighTemperature.treeDensity, temperatureWeight);
	biomData.grasDensity = lerp(biomDataLowTemperature.grasDensity, biomDataHighTemperature.grasDensity, temperatureWeight);
	biomData.flowerDensity = lerp(biomDataLowTemperature.flowerDensity, biomDataHighTemperature.flowerDensity, temperatureWeight);
}

float MapGenerator::lerp(float const a, float const b, float const weight) {
	return a + weight * (b - a);
}

bool MapGenerator::ceckChunkBorder(std::vector<std::pair<glm::vec3, Cube>> const &treeCubes, glm::vec3 const position) {
	for (size_t i = 0; i < treeCubes.size(); i++) {
		glm::vec3 cubePosition = position + treeCubes[i].first;

		int cubeU = (int)cubePosition.x;
		int cubeW = (int)cubePosition.z;

		if (cubeU < 0 || cubeU >= Settings::CHUNK_SIZE) {
			return false;
		}
		if (cubeW < 0 || cubeW >= Settings::CHUNK_SIZE) {
			return false;
		}
	}

	return true;
}
