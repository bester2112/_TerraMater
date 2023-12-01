#include "Plane.h"

Plane::Plane()
	:origin(glm::vec3(0.0f)), normal(glm::vec3(0.0f)) {}

Plane::Plane(glm::vec3 const &origin, glm::vec3 const &normal)
	:origin(origin), normal(normal) {}

Plane::~Plane() {}
