/**
 * @file Shader.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Contains a class which handles shader creation.
 * @version 0.1
 * @date 2020-05-04
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef SHADER_H
#define SHADER_H

#include "vulkan/vulkan.h"

#include <vector>

 /**
  * @brief Class to load a shader from a file and create it's shader module.
  */
class Shader {
public:

	/**
	 * @brief Handle for the created shader Module
	 */
	VkShaderModule shaderModule;

	/**
	 * @brief Constructs a new shader object
	 *
	 * @param device Handle to the vulkan device, needed for the createShaderModule call.
	 * @param shaderFilePath Path to the shader file, which will be used to create the shader/module object.
	 */
	Shader(VkDevice const &device, char const *shaderFilePath);
	~Shader();

private:

	/**
	 * @brief Reference to the device, whis is need for some vulkan function calls.
	 */
	VkDevice device;

	/**
	 * @brief Creates a shader module object.
	 *
	 * @param shaderCode Contains the content of the loaded shader file.
	 */
	void createShaderModule(std::vector<char> const &shaderCode);
};

#endif // !SHADER_H