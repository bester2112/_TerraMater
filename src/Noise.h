#ifndef NOISE_H
#define NOISE_H

#include "glm/glm.hpp"

#include <vector>

class Noise {
public:
	Noise();
	~Noise();

	float worley3D(glm::vec3 position, int const level) const;

private:

	std::vector<glm::vec3> randomPositions[4];

	void setUpWorley3D(int const level);

};

#endif // !NOISE_H
