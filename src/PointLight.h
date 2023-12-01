#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <glm/glm.hpp>

struct PointLight {
	glm::vec4 position;
	glm::vec4 intensity;
};

#endif // !POINTLIGHT_H
