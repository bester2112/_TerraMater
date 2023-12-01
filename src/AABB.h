#ifndef AABB_H
#define AABB_H

#include "glm/glm.hpp"

class AABB {
public:
	AABB();
	AABB(glm::vec3 const &minB, glm::vec3 const &maxB);
	~AABB();

	glm::vec3 minB;
	glm::vec3 maxB;

	glm::vec3 corners[8];

private:

};

#endif // !AABB_H
