#ifndef QUAD_H
#define QUAD_H

#include "Vertex.h"

struct Quad {
	Vertex vertices[4];
	uint32_t indices[6] = { 0, 1, 2, 3, 1, 0 };
};

inline bool operator< (Quad const &a, Quad const &b) {
	if (a.vertices[0].position.x < b.vertices[0].position.x) {
		return true;
	} else if (abs(a.vertices[0].position.x - b.vertices[0].position.x) < 1e-6) {
		if (a.vertices[0].position.z < b.vertices[0].position.z) {
			return true;
		} else if (abs(a.vertices[0].position.z - b.vertices[0].position.z) < 1e-6) {
			if (a.vertices[0].position.y < b.vertices[0].position.y) {
				return true;
			}
		}
	}

	return false;
}

#endif // !QUAD_H
