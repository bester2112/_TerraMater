/**
 * @file UniformBufferObject.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Contains a simple UniformBufferObject used by the shaders.
 * @version 0.1
 * @date 2020-05-10
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef UNIFORMBUFFEROBJECT_H
#define UNIFORMBUFFEROBJECT_H

#include <glm/glm.hpp>

 /**
  * @brief Simple UniformBufferObject containing all possibly needed transform matrices.
  */
struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec4 sunDirection;
};

#endif // !UNIFORMBUFFEROBJECT_H
