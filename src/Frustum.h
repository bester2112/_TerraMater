#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "Plane.h"
#include "AABB.h"

#include "glm/glm.hpp"

#define TOP_PLANE 0
#define BOTTOM_PLANE 1
#define LEFT_PLANE 2
#define RIGHT_PLANE 3
#define FAR_PLANE 4
#define NEAR_PLANE 5

class Frustum {
public:
	Frustum();
	Frustum(float const farPlaneDistance, float const nearPlaneDistance, glm::vec3 const &up);
	~Frustum();

	void updateFrustum(glm::vec3 const &cameraPosition, glm::vec3 const cameraDirection, float const cameraFov);

	bool isInside(AABB const &aabb);

private:

	glm::vec3 apex;
	glm::vec3 up;
	glm::vec3 direction;

	float aspectRatio;
	float fov;

	float farPlaneDistance;
	float nearPlaneDistance;

	Plane planes[6];

	float farPlaneWidth;
	float farPlaneHeight;

	void calculateDimensions();

};

#endif // !FRUSTUM_H
