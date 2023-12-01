#ifndef PLANE_H
#define PLANE_H

#include "glm/glm.hpp"

class Plane {
public:
	Plane();
	Plane(glm::vec3 const &origin, glm::vec3 const &normal);
	~Plane();

	glm::vec3 origin;
	glm::vec3 normal;

private:

};

#endif // !PLANE_H
