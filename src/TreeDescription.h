#ifndef TREEDESCRIPTION_H
#define TREEDESCRIPTION_H

#include "LSystemRule.h"
#include "CubeType.h"

#include <vector>

struct TreeDescription {
	std::string axiom;
	std::vector<LSystemRule> rules;
	size_t n;
	CubeType trunkType;
	CubeType leafType;
	float trunkProbability;
	float leafProbability;
	float lowerCutProbability;
};

#endif // !TREEDESCRIPTION_H
