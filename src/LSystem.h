#ifndef LSYSTEM_H
#define LSYSTEM_H

#include "LSystemRule.h"
#include "RandomSampler.h"

#include <string>
#include <vector>

class LSystem {
public:
	LSystem();
	~LSystem();

	static void grow(RandomSampler &randomSampler, std::string const &input, std::vector<LSystemRule> const rules, float const lowerCutProbability, std::string &output);

	static void growNTimes(RandomSampler &randomSampler, std::string const &input, std::vector<LSystemRule> const rules, float const lowerCutProbability, std::string &output, size_t const n);

private:

};

#endif // !LSYSTEM_H
