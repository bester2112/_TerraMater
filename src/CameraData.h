#ifndef CAMERADATA_H
#define CAMERADATA_H

#include <glm/glm.hpp>

struct CameraData {
	glm::vec4 position;
	glm::vec4 direction;
	glm::vec4 up;
	glm::vec4 sensorData;
	glm::vec4 data;
	glm::ivec4 iData;
};

#endif // !CAMERADATA_H
