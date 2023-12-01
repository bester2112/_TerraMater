#include "FileLoader.h"

#include <fstream>

FileLoader::FileLoader() {}

FileLoader::~FileLoader() {}

std::vector<char> FileLoader::readShaderFile(std::string const &fileName) {
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}