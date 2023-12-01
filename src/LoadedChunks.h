/**
 * @file LoadedChunks.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief File contains the loaded chunk class for handnling all in game available chunks.
 * @version 0.1
 * @date 2020-05-07
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef LOADEDCHUNKS_H
#define LOADEDCHUNKS_H

#include "Settings.h"
#include "Chunk.h"
#include "ChunkStack.h"
#include "VulkanWrapper.h"
#include "Map.h"
#include "LoadedChunkStack.h"
#include "ThreadPool.h"
#include "ObjArray.h"

#include "glm/glm.hpp"

#include <vector>

 /**
  * @brief This class contains all currently loaded and accessible chunks in the game.
  */
class LoadedChunks {
public:
	LoadedChunks(VulkanWrapper &vulkanWrapper);

	~LoadedChunks();

	//std::vector<LoadedChunkStack> loadedChunkStacks;
	//std::map<Coordinates, LoadedChunkStack> loadedChunkStacks;
	std::map<Coordinates, LoadedChunkStack*> loadedChunkStacks;


    void loadMap();

	void updateMiddle(glm::vec3 position);

	void generateBoxData(std::vector<BoxData> &boxData, std::vector<BoxData> &emissiveBoxData);

private:

	/**
	 * @brief Handle of the vulkan wrapper to get acces to the vulkan buffer generation.
	 */
	VulkanWrapper *vulkanWrapper;

	Map map;

	Coordinates middle = { 0,0 };

    void addLoadedChunkStack(int const x, int const z);

	ObjArray *objArray;
};

#endif // !LOADEDCHUNKS_H
