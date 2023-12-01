#include "LSystem.h"
#include "RandomSampler.h"

#include <ctype.h>

LSystem::LSystem() {}

LSystem::~LSystem() {}

void LSystem::grow(RandomSampler &randomSampler, std::string const &input, std::vector<LSystemRule> const rules, float const lowerCutProbability, std::string &output) {
	for (size_t i = 0; i < input.length(); i++) {
		bool found = false;

		if (islower(input[i]) && input[i] != 't' && input[i] != 'l') {
			if (randomSampler.getSample1DNonReset() < lowerCutProbability) {
				continue;
			}
		}

		for (size_t j = 0; j < rules.size(); j++) {
			if (input[i] == rules[j].in) {
				output += rules[j].out;
				found = true;
				break;
			}
		}

		if (!found) {
			output += input[i];
		}
	}
}

void LSystem::growNTimes(RandomSampler &randomSampler, std::string const &input, std::vector<LSystemRule> const rules, float const lowerCutProbability, std::string &output, size_t const n) {
	std::string tempInput;
	std::string tempOutput = std::string(input);

	for (size_t i = 0; i < n; i++) {
		std::swap(tempInput, tempOutput);
		tempOutput.clear();

		grow(randomSampler, tempInput, rules, lowerCutProbability, tempOutput);
	}

	output = std::string(tempOutput);
}
