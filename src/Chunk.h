/**
 * @file Chunk.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Contains the Chunk class, a data container class for mutliple cubes.
 * @version 0.1
 * @date 2020-05-07
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef CHUNK
#define CHUNK

#include "Cube.h"
#include "Vertex.h"
#include "Settings.h"
#include "ObjData.h"

 /**
  * @brief Container only class for storing a multiple cubes in one container.
  */
class Chunk {
public:
	Chunk();
	~Chunk();

	/**
	 * @brief Array containing the differenct cubes.
	 */
	Cube cubes[Settings::CHUNK_SIZE][Settings::CHUNK_SIZE][Settings::CHUNK_SIZE];

	ObjData objData[Settings::CHUNK_SIZE][Settings::CHUNK_SIZE][Settings::CHUNK_SIZE];

	int8_t lightLevel[Settings::CHUNK_SIZE][Settings::CHUNK_SIZE][Settings::CHUNK_SIZE];

private:

};

#endif // !CHUNK
