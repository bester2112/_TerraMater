#include "Noise.h"
#include "RandomSampler.h"
#include "Settings.h"


Noise::Noise() {
	setUpWorley3D(0);
	setUpWorley3D(1);
	setUpWorley3D(2);
	setUpWorley3D(3);
}

Noise::~Noise() {}

float Noise::worley3D(glm::vec3 position, int const level) const {
	float distance = FLT_MAX;

	for (int i = 0; i < Settings::worleyNoisePointCounts[level] * 27; i++) {
		//float newDistance = glm::length(randomPositions[level][i] - position) * (level + 1);
		float newDistance = glm::length(randomPositions[level][i] - position) * Settings::worleyNoiseScaling[level];

		if (newDistance < distance) {
			distance = newDistance;
		}
	}

	return distance;
}

void Noise::setUpWorley3D(int const level) {
	//randomPositions[level].clear();

	RandomSampler randomSampler = RandomSampler();

	for (int i = 0; i < Settings::worleyNoisePointCounts[level] * 27; i++) {
		glm::vec3 randomVector = glm::vec3(randomSampler.getSample1DNonReset(), randomSampler.getSample1DNonReset(), randomSampler.getSample1DNonReset());
		//randomPositions[level].push_back(randomVector);

		randomPositions[level].push_back(randomVector + glm::vec3(-1.0f, -1.0f, -1.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(-1.0f, 0.0f, -1.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(-1.0f, 1.0f, -1.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(0.0f, -1.0f, -1.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(0.0f, 0.0f, -1.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(0.0f, 1.0f, -1.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(1.0f, -1.0f, -1.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(1.0f, 0.0f, -1.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(1.0f, 1.0f, -1.0f));

		randomPositions[level].push_back(randomVector + glm::vec3(-1.0f, -1.0f, 0.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(-1.0f, 0.0f, 0.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(-1.0f, 1.0f, 0.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(0.0f, -1.0f, 0.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(0.0f, 0.0f, 0.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(0.0f, 1.0f, 0.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(1.0f, -1.0f, 0.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(1.0f, 0.0f, 0.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(1.0f, 1.0f, 0.0f));

		randomPositions[level].push_back(randomVector + glm::vec3(-1.0f, -1.0f, 1.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(-1.0f, 0.0f, 1.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(-1.0f, 1.0f, 1.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(0.0f, -1.0f, 1.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(0.0f, 0.0f, 1.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(0.0f, 1.0f, 1.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(1.0f, -1.0f, 1.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(1.0f, 0.0f, 1.0f));
		randomPositions[level].push_back(randomVector + glm::vec3(1.0f, 1.0f, 1.0f));

	}
}