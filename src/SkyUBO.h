#ifndef SKYUBO_H
#define SKYUBO_H

#include "glm/glm.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

struct SkyUBO {
	glm::vec4 cameraPosition;
	glm::vec4 lightDirection;
	float time;
};

#endif // !SKYUBO_H
