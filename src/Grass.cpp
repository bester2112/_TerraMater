#include "Grass.h"

Grass::Grass() : grassType(GrassType::GRASS) {}

Grass::Grass(GrassType grassType)
	: grassType(grassType) {}

Grass::~Grass() {}

void Grass::getVertices(std::vector<Vertex> &vertices) {
    vertices = {
        // waving grass side 1
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, 0, 0, 0},
        {{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f}, 0, 0, 0},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, 0, 0, 0},
        {{0.0f, 0.0f, 0.5f}, {0.0f, 1.0f}, 0, 0, 0},

        // waving grass side 1 other side
        {{0.5f, 0.5f, 0.0f}, {1.0f, 0.0f}, 0, 0, 0},
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, 0, 0, 0},
        {{0.0f, 0.0f, 0.5f}, {0.0f, 1.0f}, 0, 0, 0},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}, 0, 0, 0},

        // waving grass side 2
        {{0.5f, 0.0f, 0.0f}, {0.0f, 0.0f}, 0, 0, 0},
        {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f}, 0, 0, 0},
        {{0.0f, 0.5f, 0.5f}, {1.0f, 1.0f}, 0, 0, 0},
        {{0.5f, 0.0f, 0.5f}, {0.0f, 1.0f}, 0, 0, 0},

        // waving grass side 2 other side
        {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f}, 0, 0, 0},
        {{0.5f, 0.0f, 0.0f}, {0.0f, 0.0f}, 0, 0, 0},
        {{0.5f, 0.0f, 0.5f}, {0.0f, 1.0f}, 0, 0, 0},
        {{0.0f, 0.5f, 0.5f}, {1.0f, 1.0f}, 0, 0, 0}
    };
}
void Grass::getIndices(std::vector<uint32_t>& indices) {
    indices = {
        0, 1, 2, 2, 3, 0, //1
        4, 5, 6, 6, 7, 4, //2
        8, 9, 10, 10, 11, 8, // 3
        12, 13, 14, 14, 15, 12 // 4
    };
}
