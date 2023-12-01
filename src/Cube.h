/**
 * @file Cube.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Contains the Cube class, which represents an ingame cube.
 * @version 0.1
 * @date 2020-05-07
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef CUBE_H
#define CUBE_H

#include "Vertex.h"
#include "CubeType.h"

#include "vector"

 /**
  * @brief Represents an ingame cube described by its cube type.
  */
class Cube {
public:

	/**
	 * @brief Type of the cube, 0 stands for air.
	 */
	CubeType cubeType;

	Cube();
	Cube(CubeType cubeType);
	~Cube();

	/**
	 * @brief Gets the vertices of the standard cube.
	 *
	 * @param vertices Vector in which the vertices will be stored.
	 */
	void getVertices(std::vector<Vertex> &vertices);

	/**
	 * @brief Gets the indices of the standard cube.
	 *
	 * @param indices Vector in which the indices will be stored.
	 */
	void getIndices(std::vector <uint32_t> &indices);

private:
};

#endif // !CUBE_H
