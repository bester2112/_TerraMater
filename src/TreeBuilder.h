#ifndef TREEBUILDER_H
#define TREEBUILDER_H

#include "Cube.h"
#include "Coordinates.h"
#include "TreeDescription.h"
#include "RandomSampler.h"

#include <glm/glm.hpp>

//#include <map>
#include <vector>

class TreeBuilder {
public:
	TreeBuilder();
	~TreeBuilder();

	static void generateTreeCubes(TreeDescription const &treeDescription, RandomSampler &randomSampler, std::vector<std::pair<glm::vec3, Cube>> &treeCubes);

private:

};

#endif // !TREEBUILDER_H
