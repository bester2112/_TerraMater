#include "Frustum.h"
#include "Settings.h"

#define _USE_MATH_DEFINES
#include <math.h>

Frustum::Frustum() {}

Frustum::Frustum(float const farPlaneDistance, float const nearPlaneDistance, glm::vec3 const &up)
	: farPlaneDistance(farPlaneDistance), nearPlaneDistance(nearPlaneDistance), up(up) {}

Frustum::~Frustum() {}

void Frustum::updateFrustum(glm::vec3 const &cameraPosition, glm::vec3 const cameraDirection, float const cameraFov) {
	apex = cameraPosition;
	direction = glm::normalize(cameraDirection);
	fov = cameraFov;

	aspectRatio = (float)Settings::WINDOW_WIDTH / Settings::WINDOW_HEIGHT;

	calculateDimensions();


	glm::vec3 x, y, z;
	z = direction;
	x = glm::normalize(glm::cross(up, z));
	y = glm::normalize(glm::cross(z, x));

	glm::vec3 farPlaneCenter = apex + direction * farPlaneDistance;
	glm::vec3 nearPlaneCenter = apex + direction * nearPlaneDistance;

	glm::vec3 topDirection = glm::normalize((farPlaneCenter + (y * farPlaneHeight)) - apex);
	glm::vec3 bottomDirection = glm::normalize((farPlaneCenter - (y * farPlaneHeight)) - apex);
	glm::vec3 leftDirection = glm::normalize((farPlaneCenter - (x * farPlaneWidth)) - apex);
	glm::vec3 rightDirection = glm::normalize((farPlaneCenter + (x * farPlaneWidth)) - apex);

	planes[TOP_PLANE] = Plane(apex, -glm::normalize(glm::cross(topDirection, x)));
	planes[BOTTOM_PLANE] = Plane(apex, -glm::normalize(glm::cross(x, bottomDirection)));
	planes[LEFT_PLANE] = Plane(apex, -glm::normalize(glm::cross(leftDirection, y)));
	planes[RIGHT_PLANE] = Plane(apex, -glm::normalize(glm::cross(y, rightDirection)));
	planes[NEAR_PLANE] = Plane(nearPlaneCenter, direction);
	planes[FAR_PLANE] = Plane(farPlaneCenter, -direction);
}

bool Frustum::isInside(AABB const &aabb) {
	for (int i = 0; i < 6; i++) {
		int inCounter = 0;
		int outCounter = 0;

		for (int corner = 0; corner < 8; corner++) {
			if (inCounter == 0 || outCounter == 0) {
				
				if (glm::dot(glm::normalize(aabb.corners[corner] - planes[i].origin), planes[i].normal) >= 0.0f) {
					inCounter++;
				}
				else {
					outCounter++;
				}
			}
		}

		if (inCounter == 0) {
			return false;
		}
	}

	return true;
}

void Frustum::calculateDimensions() {
	farPlaneHeight = tanf(fov * (M_PI / 180.f) / 2.0f) * farPlaneDistance;
	farPlaneWidth = farPlaneHeight * aspectRatio;
}
