#ifndef MAP_H
#define MAP_H

#include "ChunkStack.h"
#include "MapGenerator.h"
#include "Coordinates.h"

#include <algorithm>
#include <map>
#include <thread>
#include <condition_variable>

class Map {
public:
	Map();
	~Map();

	void loadChunkStack(Coordinates const &coordinates, ChunkStack &chunkstack);

	void saveChunkStack(ChunkStack &chunkstack);

private:
	std::map<Coordinates, ChunkStack> map;

	MapGenerator mapGenerator;

	bool existsOnDisk(Coordinates const &coordinates);

	void readChunkStackFromDisk(Coordinates const &coordinates, ChunkStack &chunkstack);

	void writeChunkStackToDisk(ChunkStack &chunkStack);
};

#endif // !MAP_H