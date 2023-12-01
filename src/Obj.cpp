#include "Obj.h"

#include <string>
#include <stdexcept>

#include "tiny_obj_loader.h"
#define TINYOBJLOADER_IMPLEMENTATION

Obj::Obj() {}

Obj::Obj(char const *FilePath) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, FilePath)) {
		throw std::runtime_error(warn + err);
	}

	for (auto const &shape : shapes) {
		for (auto const &index : shape.mesh.indices) {
			BigVertex vertex;

			vertex.position = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.textureCoordinate = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};

			objVertices.push_back(vertex);

			objIndices.push_back(objIndices.size());
		}
	}
}

Obj::~Obj() {}

void Obj::getVertices(std::vector<BigVertex> &vertices) {
	vertices.insert(vertices.end(), objVertices.begin(), objVertices.end());
}

void Obj::getIndices(std::vector<uint32_t> &indices) {
	indices.insert(indices.end(), objIndices.begin(), objIndices.end());
}
