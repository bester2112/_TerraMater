#include "TreeBuilder.h"
#include "LSystem.h"

#include "glm/gtx/rotate_vector.hpp"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stack>

#include <iostream>

void TreeBuilder::generateTreeCubes(TreeDescription const &treeDescription, RandomSampler &randomSampler, std::vector<std::pair<glm::vec3, Cube>> &treeCubes) {
	std::string blueprint;
	LSystem::growNTimes(randomSampler, treeDescription.axiom, treeDescription.rules, treeDescription.lowerCutProbability, blueprint, treeDescription.n);

	glm::vec3 position = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 direction = glm::vec3(0.0f, 1.0f, 0.0f);

	std::stack<glm::vec3> positionStack;
	std::stack<glm::vec3> directionStack;

	for (size_t i = 0; i < blueprint.size(); i++) {
		switch (blueprint[i]) {
		case 'T':
			position += direction;
			treeCubes.push_back(std::pair<glm::vec3, Cube>(position, Cube(treeDescription.trunkType)));
			break;
		case 't':
			if (randomSampler.getSample1DNonReset() < treeDescription.trunkProbability) {
				position += direction;
				treeCubes.push_back(std::pair<glm::vec3, Cube>(position, Cube(treeDescription.trunkType)));
			}
			break;
		case 'L':
			position += direction;
			treeCubes.push_back(std::pair<glm::vec3, Cube>(position, Cube(treeDescription.leafType)));
			break;
		case 'l':
			if (randomSampler.getSample1DNonReset() < treeDescription.leafProbability) {
				position += direction;
				treeCubes.push_back(std::pair<glm::vec3, Cube>(position, Cube(treeDescription.leafType)));
			}
			break;
		case '[':
			positionStack.push(position);
			directionStack.push(direction);
			break;
		case ']':
			position = positionStack.top();
			positionStack.pop();
			direction = directionStack.top();
			directionStack.pop();
			break;
		case '+':
			direction = glm::rotateZ(direction, (float)M_PI / 2.0f);
			break;
		case '-':
			direction = glm::rotateZ(direction, -(float)M_PI / 2.0f);
			break;
		case '*':
			direction = glm::rotateX(direction, (float)M_PI / 2.0f);
			break;
		case '/':
			direction = glm::rotateX(direction, -(float)M_PI / 2.0f);
			break;
		case '>':
			direction = glm::rotateY(direction, (float)M_PI / 2.0f);
			break;
		case '<':
			direction = glm::rotateY(direction, -(float)M_PI / 2.0f);
			break;
		default:
			break;
		}
		
		//std::cout << direction.x << " " << direction.y << " " << direction.z << std::endl;
	}
}
