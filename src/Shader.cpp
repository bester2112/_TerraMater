#include "Shader.h"
#include "FileLoader.h"

#include <stdexcept>

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

Shader::Shader(VkDevice const &device, char const *shaderFilePath)
	: device(device) {
	std::vector<char> shaderCode = FileLoader::readShaderFile(shaderFilePath);

	createShaderModule(shaderCode);

#ifdef _DEBUG
	//Debug utility printing out shader infos
	std::cout << "Shader created:" << std::endl;
	std::cout << "\t" << "File path: " << shaderFilePath << std::endl;
	std::cout << "\t" << "File size: " << shaderCode.size() << std::endl;
#endif // _DEBUG
}

Shader::~Shader() {
	vkDestroyShaderModule(device, shaderModule, nullptr);
}

void Shader::createShaderModule(std::vector<char> const &shaderCode) {

	//Creating the shader module create info struct, which will be passed as argument to the createShaderModule call
	VkShaderModuleCreateInfo shaderModuleCreateInfo{};
	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfo.codeSize = shaderCode.size();
	shaderModuleCreateInfo.pCode = reinterpret_cast<uint32_t const *>(shaderCode.data());

	if (vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create shader module");
	}
}
