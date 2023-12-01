#include "LoadedChunks.h"

#include <math.h>
#include <iostream>
#include <set>
#include <thread>

LoadedChunks::LoadedChunks(VulkanWrapper &vulkanWrapper)
	: vulkanWrapper(&vulkanWrapper) {
	objArray = new ObjArray();
}

LoadedChunks::~LoadedChunks() {
	for (auto iterator = loadedChunkStacks.begin(); iterator != loadedChunkStacks.end(); iterator++) {
		map.saveChunkStack(iterator->second->chunkStack);
		iterator->second->deleteVulkanChunks();
	}
}

void LoadedChunks::loadMap() {
	for (size_t x = 0; x < Settings::LOADED_CHUNKS; x++) {
		for (size_t z = 0; z < Settings::LOADED_CHUNKS; z++) {
			addLoadedChunkStack((int)x - Settings::LOADED_CHUNKS / 2, (int)z - Settings::LOADED_CHUNKS / 2);
		}
	}
}

void LoadedChunks::updateMiddle(glm::vec3 position) {
	Coordinates newMiddle = Coordinates{ (int)floorf(position.x / Settings::CHUNK_SIZE), (int)floorf(position.z / Settings::CHUNK_SIZE) };

	int directionX = newMiddle.x - middle.x;
	int directionZ = newMiddle.z - middle.z;

	std::vector<Coordinates> coordinatesRemoved;

	for (auto iterator = loadedChunkStacks.begin(); iterator != loadedChunkStacks.end(); iterator++) {
		Coordinates coordinates = iterator->first;

		if (iterator->second->chunkRemoved) {
			coordinatesRemoved.push_back(coordinates);
		}

		if (iterator->second->chunkStackReady) {
			if (abs(coordinates.x - newMiddle.x) > (Settings::LOADED_CHUNKS / 2) || abs(coordinates.z - newMiddle.z) > (Settings::LOADED_CHUNKS / 2)) {
				iterator->second->willBeRemoved = true;

				iterator->second->chunkStackReady = false;

				ThreadPool::getInstance().submit([this, iterator] {
					map.saveChunkStack(iterator->second->chunkStack);

					while (iterator->second->lifeCounter > 0) {
						//std::cout << iterator->second.lifeCounter << std::endl;
						std::this_thread::sleep_for(std::chrono::seconds(1));
					}

					iterator->second->deleteVulkanChunks();
					iterator->second->chunkRemoved = true;
					});
			}
		}
	}

	for (size_t i = 0; i < coordinatesRemoved.size(); i++) {
		auto iterator = loadedChunkStacks.find(coordinatesRemoved[i]);
		LoadedChunkStack *pointer = iterator->second;

		loadedChunkStacks.erase(coordinatesRemoved[i]);

		delete pointer;
	}

	if (directionX != 0 || directionZ != 0) {
		for (size_t x = 0; x < Settings::LOADED_CHUNKS; x++) {
			for (size_t z = 0; z < Settings::LOADED_CHUNKS; z++) {
				addLoadedChunkStack(newMiddle.x + (int)x - Settings::LOADED_CHUNKS / 2, newMiddle.z + (int)z - Settings::LOADED_CHUNKS / 2);
			}
		}

		middle = newMiddle;
	}
}

void LoadedChunks::generateBoxData(std::vector<BoxData> &boxData, std::vector<BoxData> &emissiveBoxData) {
	for (auto iterator = loadedChunkStacks.begin(); iterator != loadedChunkStacks.end(); iterator++) {
		if (iterator->second->chunkStackReady &&  !iterator->second->chunkRemoved) {
			iterator->second->generateBoxData(boxData, emissiveBoxData);
		} 
	}
}

void LoadedChunks::addLoadedChunkStack(int const x, int const z) {
	Coordinates coordinates = { x, z };

	auto iterator = loadedChunkStacks.find(coordinates);
	if (iterator != loadedChunkStacks.end()) {
		return;
	}

	//loadedChunkStacks.insert(std::pair<Coordinates, LoadedChunkStack>(coordinates, LoadedChunkStack(*vulkanWrapper)));
	loadedChunkStacks.emplace(std::pair<Coordinates, LoadedChunkStack*>(coordinates, new LoadedChunkStack(*vulkanWrapper, objArray)));
	iterator = loadedChunkStacks.find(coordinates);

	ThreadPool::getInstance().submit([this, coordinates, iterator] {
		map.loadChunkStack(coordinates, iterator->second->chunkStack);

		map.loadChunkStack({ coordinates.x - 1, coordinates.z }, iterator->second->leftStack);
		map.loadChunkStack({ coordinates.x + 1, coordinates.z }, iterator->second->rightStack);
		map.loadChunkStack({ coordinates.x, coordinates.z - 1 }, iterator->second->frontStack);
		map.loadChunkStack({ coordinates.x, coordinates.z + 1 }, iterator->second->backStack);
		map.loadChunkStack({ coordinates.x - 1, coordinates.z - 1 }, iterator->second->frontLeftStack);
		map.loadChunkStack({ coordinates.x + 1, coordinates.z - 1 }, iterator->second->frontRightStack);
		map.loadChunkStack({ coordinates.x - 1, coordinates.z + 1 }, iterator->second->backLeftStack);
		map.loadChunkStack({ coordinates.x + 1, coordinates.z + 1 }, iterator->second->backRightStack);

		iterator->second->generateVulkanChunks();
		});
}