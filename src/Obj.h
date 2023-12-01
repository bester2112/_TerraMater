#ifndef OBJ_H
#define OBJ_H

#include "BigVertex.h"

#include "tiny_obj_loader.h"
#define TINYOBJLOADER_IMPLEMENTATION

#include <vector>

class Obj {
public:
	Obj();
	Obj(char const *FilePath);
	~Obj();

	void getVertices(std::vector<BigVertex> &vertices);
	void getIndices(std::vector<uint32_t> &indices);

private:

	std::vector<BigVertex> objVertices;
	std::vector<uint32_t> objIndices;
};

#endif // !OBJ_H
