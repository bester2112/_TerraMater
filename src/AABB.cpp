#include "AABB.h"

AABB::AABB()
	: minB(glm::vec3(0.0f)), maxB(glm::vec3(0.0f)) {}

AABB::AABB(glm::vec3 const &minB, glm::vec3 const &maxB)
	: minB(minB), maxB(maxB) {
	corners[0] = minB;
	corners[1] = glm::vec3(minB.x, minB.y, maxB.z);
	corners[2] = glm::vec3(minB.x, maxB.y, minB.z);
	corners[3] = glm::vec3(minB.x, maxB.y, maxB.z);
	corners[4] = glm::vec3(maxB.x, minB.y, minB.z);
	corners[5] = glm::vec3(maxB.x, minB.y, maxB.z);
	corners[6] = glm::vec3(maxB.x, maxB.y, minB.z);
	corners[7] = maxB;
}

AABB::~AABB() {}
