/**
 * @file FileLoader.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Contains a helper class to load files.
 * @version 0.1
 * @date 2020-05-04
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef FILELOADER_H
#define FILELOADER_H

#include <vector>
#include <string>

 /**
  * @brief Helper class to load different file types.
  */
class FileLoader {
public:
	FileLoader();
	~FileLoader();

	/**
	 * @brief Loads an compiled shader file and saves it in a char vector.
	 *
	 * @param fileName Path to the shader file.
	 * @return std::vector<char> Shader file content stored inside this char vector.
	 */
	static std::vector<char> readShaderFile(std::string const &fileName);
};

#endif // !FILELOADER_H
