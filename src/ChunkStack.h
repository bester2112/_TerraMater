#ifndef CHUNKSTACK_H
#define CHUNKSTACK_H

#include "Chunk.h"
#include "Coordinates.h"

#include <vector>

class ChunkStack {
public:
	ChunkStack();

	ChunkStack(Coordinates const coordinates);

	~ChunkStack();

	std::vector<Chunk> stack;

	Coordinates coordinates;

	bool changed = false;

private:

};

#endif // !CHUNKSTACK_H
