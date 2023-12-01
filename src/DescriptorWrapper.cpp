#include "DescriptorWrapper.h"
#include "Settings.h"
#include "CameraData.h"
#include "BoxData.h"
#include "PointLight.h"
#include "SkyUBO.h"

#include <stdexcept>
#include <array>

DescriptorWrapper::DescriptorWrapper()
	: device(VK_NULL_HANDLE), descriptorSetLayout(VK_NULL_HANDLE) {}

DescriptorWrapper::DescriptorWrapper(VkDevice &device, uint32_t descriptorCount)
	: device(device), descriptorCount(descriptorCount) {
	createDescriptorSetLayout();
	createDescriptorPool();

	createComputeDescriptorSetLayout();
	createComputeDescriptorPool();

	createObjDescriptorSetLayout();
	createObjDescriptorPool();

	createSkyDescriptorSetLayout();
	createSkyDescriptorPool();
}

DescriptorWrapper::~DescriptorWrapper() {
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(device, descriptorPool, nullptr);

	vkDestroyDescriptorSetLayout(device, quadDescriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(device, quadDescriptorPool, nullptr);

	vkDestroyDescriptorSetLayout(device, computeDescriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(device, computeDescriptorPool, nullptr);

	vkDestroyDescriptorSetLayout(device, objDescriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(device, objDescriptorPool, nullptr);

	vkDestroyDescriptorSetLayout(device, skyDescriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(device, skyDescriptorPool, nullptr);
}

void DescriptorWrapper::createDescriptorSets(std::vector<VkBuffer> const &uniformBuffers, TextureArray const &textureArray) {
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts(descriptorCount, descriptorSetLayout);

	//Creating the descriptor set allocate info struct, which will be passed as argument to the allocateDescriptorSets call
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = descriptorCount;
	descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

	descriptorSets.resize(descriptorCount);
	if (vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate descriptor sets");
	}

	//Writing the uniform buffer object and texture information into each descriptor set
	for (size_t i = 0; i < descriptorCount; i++) {

		//Setting up the information of the uniform buffer object
		VkDescriptorBufferInfo descriptorBufferInfo{};
		descriptorBufferInfo.buffer = uniformBuffers[i];
		descriptorBufferInfo.offset = 0;
		descriptorBufferInfo.range = sizeof(UniformBufferObject);

		//Setting up the information of the texture(sampler)
		VkDescriptorImageInfo descriptorImageInfos[Settings::TEXTURE_COUNT];
		for (size_t j = 0; j < Settings::TEXTURE_COUNT; j++) {
			descriptorImageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptorImageInfos[j].imageView = textureArray.textures[j].textureImageView;
			descriptorImageInfos[j].sampler = textureArray.textures[j].textureSampler;
		}

		//Creating the write descriptor sets structs, which will be filled with the above created infos, after that they get written into the descriptor sets
		std::array<VkWriteDescriptorSet, 2> writeDescriptorSets{};
		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].dstSet = descriptorSets[i];
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].pBufferInfo = &descriptorBufferInfo;

		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].dstSet = descriptorSets[i];
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].dstArrayElement = 0;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[1].descriptorCount = Settings::TEXTURE_COUNT;
		writeDescriptorSets[1].pImageInfo = descriptorImageInfos;

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}
}

void DescriptorWrapper::createQuadDescriptorSets(std::vector<VkBuffer> const &uniformBuffers, VkImageView const &imageView, VkSampler const &sampler) {
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts(descriptorCount, quadDescriptorSetLayout);

	//Creating the descriptor set allocate info struct, which will be passed as argument to the allocateDescriptorSets call
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = quadDescriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = descriptorCount;
	descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

	quadDescriptorSets.resize(descriptorCount);
	if (vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, quadDescriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate quad descriptor sets");
	}

	//Writing the uniform buffer object and texture information into each descriptor set
	for (size_t i = 0; i < descriptorCount; i++) {

		//Setting up the information of the uniform buffer object
		VkDescriptorBufferInfo descriptorBufferInfo{};
		descriptorBufferInfo.buffer = uniformBuffers[i];
		descriptorBufferInfo.offset = 0;
		descriptorBufferInfo.range = sizeof(UniformBufferObject);

		//Setting up the information of the texture(sampler)
		VkDescriptorImageInfo descriptorImageInfo;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptorImageInfo.imageView = imageView;
		descriptorImageInfo.sampler = sampler;

		//Creating the write descriptor sets structs, which will be filled with the above created infos, after that they get written into the descriptor sets
		std::array<VkWriteDescriptorSet, 2> writeDescriptorSets{};
		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].dstSet = quadDescriptorSets[i];
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].pBufferInfo = &descriptorBufferInfo;

		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].dstSet = quadDescriptorSets[i];
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].dstArrayElement = 0;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[1].descriptorCount = 1;
		writeDescriptorSets[1].pImageInfo = &descriptorImageInfo;

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}
}

void DescriptorWrapper::createComputeDescriptorSets(std::vector<VkBuffer> const &cameraDataBuffers, VkImageView const &imageView, VkSampler const &sampler, TextureArray const &textureArray, SkyBox const &skyBox, VkBuffer const &boxDataBuffer, std::vector<BoxData> const &boxData, VkBuffer const &emissiveBoxDataBuffer, std::vector<BoxData> const &emissiveBoxData, VkBuffer const &pointLightBuffer, std::vector<PointLight> const &pointLights, VkBuffer const &writeBackDataBuffer, bool &allocated) {
	if (!allocated) {
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts(descriptorCount, computeDescriptorSetLayout);

		//Creating the descriptor set allocate info struct, which will be passed as argument to the allocateDescriptorSets call
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = computeDescriptorPool;
		descriptorSetAllocateInfo.descriptorSetCount = descriptorCount;
		descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

		computeDescriptorSets.resize(descriptorCount);
		if (vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, computeDescriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate compute descriptor sets");
		}

		allocated = true;
	}

	//Writing the uniform buffer object and texture information into each descriptor set
	for (size_t i = 0; i < descriptorCount; i++) {

		//Setting up the information of the uniform buffer object
		VkDescriptorBufferInfo descriptorBufferInfo{};
		descriptorBufferInfo.buffer = cameraDataBuffers[i];
		descriptorBufferInfo.offset = 0;
		descriptorBufferInfo.range = sizeof(CameraData);

		//Setting up the information of the texture(sampler)
		VkDescriptorImageInfo computeDescriptorImageInfo;
		//descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		computeDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		computeDescriptorImageInfo.imageView = imageView;
		computeDescriptorImageInfo.sampler = sampler;

		//Setting up the information of the texture(sampler)
		VkDescriptorImageInfo descriptorImageInfos[Settings::TEXTURE_COUNT];
		for (size_t j = 0; j < Settings::TEXTURE_COUNT; j++) {
			descriptorImageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptorImageInfos[j].imageView = textureArray.textures[j].textureImageView;
			descriptorImageInfos[j].sampler = textureArray.textures[j].textureSampler;
		}

		//Setting up the information of the texture(sampler)
		VkDescriptorImageInfo skyBoxDescriptorImageInfos[Settings::SKYBOX_TEXTURE_COUNT];
		for (size_t j = 0; j < Settings::SKYBOX_TEXTURE_COUNT; j++) {
			skyBoxDescriptorImageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			skyBoxDescriptorImageInfos[j].imageView = skyBox.textures[j].textureImageView;
			skyBoxDescriptorImageInfos[j].sampler = skyBox.textures[j].textureSampler;
		}

		VkDescriptorBufferInfo descriptorBoxBufferInfo{};
		descriptorBoxBufferInfo.buffer = boxDataBuffer;
		descriptorBoxBufferInfo.offset = 0;
		descriptorBoxBufferInfo.range = sizeof(boxData[0]) * boxData.size();

		VkDescriptorBufferInfo descriptorEmissiveBoxBufferInfo{};
		descriptorEmissiveBoxBufferInfo.buffer = emissiveBoxDataBuffer;
		descriptorEmissiveBoxBufferInfo.offset = 0;
		descriptorEmissiveBoxBufferInfo.range = sizeof(emissiveBoxData[0]) * emissiveBoxData.size();

		VkDescriptorBufferInfo descriptorPointLightBufferInfo{};
		descriptorPointLightBufferInfo.buffer = pointLightBuffer;
		descriptorPointLightBufferInfo.offset = 0;
		descriptorPointLightBufferInfo.range = sizeof(pointLights[0]) * pointLights.size();

		VkDescriptorBufferInfo descriptorWriteBackDataBufferInfo{};
		descriptorWriteBackDataBufferInfo.buffer = writeBackDataBuffer;
		descriptorWriteBackDataBufferInfo.offset = 0;
		descriptorWriteBackDataBufferInfo.range = sizeof(WriteBackData);

		//Creating the write descriptor sets structs, which will be filled with the above created infos, after that they get written into the descriptor sets
		std::array<VkWriteDescriptorSet, 8> writeDescriptorSets{};
		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].dstSet = computeDescriptorSets[i];
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].pBufferInfo = &descriptorBufferInfo;

		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].dstSet = computeDescriptorSets[i];
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].dstArrayElement = 0;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		writeDescriptorSets[1].descriptorCount = 1;
		writeDescriptorSets[1].pImageInfo = &computeDescriptorImageInfo;

		writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[2].dstSet = computeDescriptorSets[i];
		writeDescriptorSets[2].dstBinding = 2;
		writeDescriptorSets[2].dstArrayElement = 0;
		writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[2].descriptorCount = Settings::TEXTURE_COUNT;
		writeDescriptorSets[2].pImageInfo = descriptorImageInfos;

		writeDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[3].dstSet = computeDescriptorSets[i];
		writeDescriptorSets[3].dstBinding = 34;
		writeDescriptorSets[3].dstArrayElement = 0;
		writeDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[3].descriptorCount = Settings::SKYBOX_TEXTURE_COUNT;
		writeDescriptorSets[3].pImageInfo = skyBoxDescriptorImageInfos;

		writeDescriptorSets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[4].dstSet = computeDescriptorSets[i];
		writeDescriptorSets[4].dstBinding = 46;
		writeDescriptorSets[4].dstArrayElement = 0;
		writeDescriptorSets[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSets[4].descriptorCount = 1;
		writeDescriptorSets[4].pBufferInfo = &descriptorBoxBufferInfo;

		writeDescriptorSets[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[5].dstSet = computeDescriptorSets[i];
		writeDescriptorSets[5].dstBinding = 47;
		writeDescriptorSets[5].dstArrayElement = 0;
		writeDescriptorSets[5].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSets[5].descriptorCount = 1;
		writeDescriptorSets[5].pBufferInfo = &descriptorEmissiveBoxBufferInfo;

		writeDescriptorSets[6].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[6].dstSet = computeDescriptorSets[i];
		writeDescriptorSets[6].dstBinding = 48;
		writeDescriptorSets[6].dstArrayElement = 0;
		writeDescriptorSets[6].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSets[6].descriptorCount = 1;
		writeDescriptorSets[6].pBufferInfo = &descriptorPointLightBufferInfo;

		writeDescriptorSets[7].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[7].dstSet = computeDescriptorSets[i];
		writeDescriptorSets[7].dstBinding = 49;
		writeDescriptorSets[7].dstArrayElement = 0;
		writeDescriptorSets[7].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSets[7].descriptorCount = 1;
		writeDescriptorSets[7].pBufferInfo = &descriptorWriteBackDataBufferInfo;

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}
}

void DescriptorWrapper::createObjDescriptorSets(std::vector<VkBuffer> const &uniformBuffers, TextureArray const &textureArray) {
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts(descriptorCount, objDescriptorSetLayout);

	//Creating the descriptor set allocate info struct, which will be passed as argument to the allocateDescriptorSets call
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = objDescriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = descriptorCount;
	descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

	objDescriptorSets.resize(descriptorCount);
	if (vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, objDescriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate obj descriptor sets");
	}

	//Writing the uniform buffer object and texture information into each descriptor set
	for (size_t i = 0; i < descriptorCount; i++) {

		//Setting up the information of the uniform buffer object
		VkDescriptorBufferInfo descriptorBufferInfo{};
		descriptorBufferInfo.buffer = uniformBuffers[i];
		descriptorBufferInfo.offset = 0;
		descriptorBufferInfo.range = sizeof(UniformBufferObject);

		//Setting up the information of the texture(sampler)
		VkDescriptorImageInfo descriptorImageInfos[Settings::OBJ_COUNT];
		for (size_t j = 0; j < Settings::OBJ_COUNT; j++) {
			descriptorImageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptorImageInfos[j].imageView = textureArray.objTextures[j].textureImageView;
			descriptorImageInfos[j].sampler = textureArray.objTextures[j].textureSampler;
		}

		//Creating the write descriptor sets structs, which will be filled with the above created infos, after that they get written into the descriptor sets
		std::array<VkWriteDescriptorSet, 2> writeDescriptorSets{};
		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].dstSet = objDescriptorSets[i];
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].pBufferInfo = &descriptorBufferInfo;

		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].dstSet = objDescriptorSets[i];
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].dstArrayElement = 0;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[1].descriptorCount = Settings::OBJ_COUNT;
		writeDescriptorSets[1].pImageInfo = descriptorImageInfos;

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}
}

void DescriptorWrapper::createSkyDescriptorSets(std::vector<VkBuffer> const &uniformBuffers, std::vector<VkBuffer> const &skyUniformBuffers, CloudTexture const &cloudTexture) {
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts(descriptorCount, skyDescriptorSetLayout);

	//Creating the descriptor set allocate info struct, which will be passed as argument to the allocateDescriptorSets call
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool = skyDescriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = descriptorCount;
	descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

	skyDescriptorSets.resize(descriptorCount);
	if (vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, skyDescriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate sky descriptor sets");
	}

	//Writing the uniform buffer object and texture information into each descriptor set
	for (size_t i = 0; i < descriptorCount; i++) {

		//Setting up the information of the uniform buffer object
		VkDescriptorBufferInfo descriptorBufferInfo{};
		descriptorBufferInfo.buffer = uniformBuffers[i];
		descriptorBufferInfo.offset = 0;
		descriptorBufferInfo.range = sizeof(UniformBufferObject);

		//Setting up the information of the uniform buffer object
		VkDescriptorBufferInfo skyDescriptorBufferInfo{};
		skyDescriptorBufferInfo.buffer = skyUniformBuffers[i];
		skyDescriptorBufferInfo.offset = 0;
		skyDescriptorBufferInfo.range = sizeof(SkyUBO);

		//Setting up the information of the texture(sampler)
		VkDescriptorImageInfo descriptorImageInfos;
			descriptorImageInfos.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptorImageInfos.imageView = cloudTexture.textureImageView;
			descriptorImageInfos.sampler = cloudTexture.textureSampler;

		//Creating the write descriptor sets structs, which will be filled with the above created infos, after that they get written into the descriptor sets
		std::array<VkWriteDescriptorSet, 3> writeDescriptorSets{};
		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].dstSet = skyDescriptorSets[i];
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].pBufferInfo = &descriptorBufferInfo;

		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].dstSet = skyDescriptorSets[i];
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].dstArrayElement = 0;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[1].descriptorCount = 1;
		writeDescriptorSets[1].pBufferInfo = &skyDescriptorBufferInfo;

		writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[2].dstSet = skyDescriptorSets[i];
		writeDescriptorSets[2].dstBinding = 2;
		writeDescriptorSets[2].dstArrayElement = 0;
		writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[2].descriptorCount = 1;
		writeDescriptorSets[2].pImageInfo = &descriptorImageInfos;

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
	}
}

void DescriptorWrapper::createDescriptorPool() { //descriptorPoolSize should be the swapchain image count

	//Setting up the pool size for the uniform buffer object descriptor
	VkDescriptorPoolSize uniformBufferObjectDescriptorPoolSize{};
	uniformBufferObjectDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformBufferObjectDescriptorPoolSize.descriptorCount = descriptorCount;

	//Setting up the pool size for the texture sampler descriptor
	VkDescriptorPoolSize textureSamplerDescriptorPoolSize{};
	textureSamplerDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureSamplerDescriptorPoolSize.descriptorCount = descriptorCount;

	VkDescriptorPoolSize descriptorPoolSizes[] = { uniformBufferObjectDescriptorPoolSize, textureSamplerDescriptorPoolSize };

	//Creating the descriptor pool create info struct, which will be passed as argument to the CreateDescriptorPool call
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.poolSizeCount = 2;
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes;
	descriptorPoolCreateInfo.maxSets = descriptorCount;

	if (vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor pool");
	}
}

void DescriptorWrapper::createComputeDescriptorSetLayout() {
	//Setting up the binding for the used uniform buffer object
	VkDescriptorSetLayoutBinding uniformBufferObjectBinding{};
	uniformBufferObjectBinding.binding = 0;
	uniformBufferObjectBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformBufferObjectBinding.descriptorCount = 1;
	uniformBufferObjectBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	//Setting up the binding for the used texture sampler
	VkDescriptorSetLayoutBinding textureSamplerBinding{};
	textureSamplerBinding.binding = 1;
	textureSamplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureSamplerBinding.descriptorCount = 1;
	textureSamplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[] = { uniformBufferObjectBinding , textureSamplerBinding };

	//Creating the descriptor set layout create info struct, which will be passed as argument to the CreateDescriptorSetLayout call
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = 2;
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings;

	if (vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &quadDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create quad descriptor set layout");
	}

	VkDescriptorSetLayoutBinding computeUniformBufferObjectBinding{};
	computeUniformBufferObjectBinding.binding = 0;
	computeUniformBufferObjectBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	computeUniformBufferObjectBinding.descriptorCount = 1;
	computeUniformBufferObjectBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	VkDescriptorSetLayoutBinding computeImageBinding{};
	computeImageBinding.binding = 1;
	computeImageBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	computeImageBinding.descriptorCount = 1;
	computeImageBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	//Setting up the binding for the used texture sampler
	VkDescriptorSetLayoutBinding computeTextureSamplerBinding{};
	computeTextureSamplerBinding.binding = 2;
	computeTextureSamplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	computeTextureSamplerBinding.descriptorCount = Settings::TEXTURE_COUNT;
	computeTextureSamplerBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	//Setting up the binding for the used texture sampler
	VkDescriptorSetLayoutBinding computeSkyBoxTextureSamplerBinding{};
	computeSkyBoxTextureSamplerBinding.binding = 34;
	computeSkyBoxTextureSamplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	computeSkyBoxTextureSamplerBinding.descriptorCount = Settings::SKYBOX_TEXTURE_COUNT;
	computeSkyBoxTextureSamplerBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	VkDescriptorSetLayoutBinding computeStorageBufferBinding{};
	computeStorageBufferBinding.binding = 46;
	computeStorageBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	computeStorageBufferBinding.descriptorCount = 1;
	computeStorageBufferBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	VkDescriptorSetLayoutBinding computeLightStorageBufferBinding{};
	computeLightStorageBufferBinding.binding = 47;
	computeLightStorageBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	computeLightStorageBufferBinding.descriptorCount = 1;
	computeLightStorageBufferBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	VkDescriptorSetLayoutBinding computePointLightStorageBufferBinding{};
	computePointLightStorageBufferBinding.binding = 48;
	computePointLightStorageBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	computePointLightStorageBufferBinding.descriptorCount = 1;
	computePointLightStorageBufferBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	VkDescriptorSetLayoutBinding computeWriteBackDataStorageBufferBinding{};
	computeWriteBackDataStorageBufferBinding.binding = 49;
	computeWriteBackDataStorageBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	computeWriteBackDataStorageBufferBinding.descriptorCount = 1;
	computeWriteBackDataStorageBufferBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	VkDescriptorSetLayoutBinding computeDescriptorSetLayoutBindings[] = { computeUniformBufferObjectBinding, computeImageBinding, computeTextureSamplerBinding, computeSkyBoxTextureSamplerBinding, computeStorageBufferBinding, computeLightStorageBufferBinding, computePointLightStorageBufferBinding, computeWriteBackDataStorageBufferBinding };

	//Creating the descriptor set layout create info struct, which will be passed as argument to the CreateDescriptorSetLayout call
	VkDescriptorSetLayoutCreateInfo computeDescriptorSetLayoutCreateInfo{};
	computeDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	computeDescriptorSetLayoutCreateInfo.bindingCount = 8;
	computeDescriptorSetLayoutCreateInfo.pBindings = computeDescriptorSetLayoutBindings;

	if (vkCreateDescriptorSetLayout(device, &computeDescriptorSetLayoutCreateInfo, nullptr, &computeDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create compute descriptor set layout");
	}
}

void DescriptorWrapper::createComputeDescriptorPool() {
	//Setting up the pool size for the uniform buffer object descriptor
	VkDescriptorPoolSize uniformBufferObjectDescriptorPoolSize{};
	uniformBufferObjectDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformBufferObjectDescriptorPoolSize.descriptorCount = descriptorCount;

	//Setting up the pool size for the texture sampler descriptor
	VkDescriptorPoolSize computeTextureSamplerDescriptorPoolSize{};
	computeTextureSamplerDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	computeTextureSamplerDescriptorPoolSize.descriptorCount = descriptorCount;

	//Setting up the pool size for the texture sampler descriptor
	VkDescriptorPoolSize textureSamplerDescriptorPoolSize{};
	textureSamplerDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureSamplerDescriptorPoolSize.descriptorCount = descriptorCount;

	VkDescriptorPoolSize storageBufferPoolSize{};
	storageBufferPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	storageBufferPoolSize.descriptorCount = descriptorCount;


	VkDescriptorPoolSize descriptorPoolSizes[] = { uniformBufferObjectDescriptorPoolSize, computeTextureSamplerDescriptorPoolSize, textureSamplerDescriptorPoolSize, textureSamplerDescriptorPoolSize, storageBufferPoolSize, storageBufferPoolSize, storageBufferPoolSize, storageBufferPoolSize };

	//Creating the descriptor pool create info struct, which will be passed as argument to the CreateDescriptorPool call
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.poolSizeCount = 6;
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes;
	descriptorPoolCreateInfo.maxSets = descriptorCount;

	if (vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &computeDescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create compute descriptor pool");
	}

	//Setting up the pool size for the texture sampler descriptor
	VkDescriptorPoolSize quadTextureSamplerDescriptorPoolSize{};
	quadTextureSamplerDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	quadTextureSamplerDescriptorPoolSize.descriptorCount = descriptorCount;

	//Creating the descriptor pool create info struct, which will be passed as argument to the CreateDescriptorPool call
	VkDescriptorPoolCreateInfo quadDescriptorPoolCreateInfo{};
	quadDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	quadDescriptorPoolCreateInfo.poolSizeCount = 1;
	quadDescriptorPoolCreateInfo.pPoolSizes = &quadTextureSamplerDescriptorPoolSize;
	quadDescriptorPoolCreateInfo.maxSets = descriptorCount;

	if (vkCreateDescriptorPool(device, &quadDescriptorPoolCreateInfo, nullptr, &quadDescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create quad descriptor pool");
	}
}

void DescriptorWrapper::createObjDescriptorSetLayout() {
	//Setting up the binding for the used uniform buffer object
	VkDescriptorSetLayoutBinding uniformBufferObjectBinding{};
	uniformBufferObjectBinding.binding = 0;
	uniformBufferObjectBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformBufferObjectBinding.descriptorCount = 1;
	uniformBufferObjectBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	//Setting up the binding for the used texture sampler
	VkDescriptorSetLayoutBinding textureSamplerBinding{};
	textureSamplerBinding.binding = 1;
	textureSamplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureSamplerBinding.descriptorCount = Settings::OBJ_COUNT;
	textureSamplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[] = { uniformBufferObjectBinding, textureSamplerBinding };

	//Creating the descriptor set layout create info struct, which will be passed as argument to the CreateDescriptorSetLayout call
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = 2;
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings;

	if (vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &objDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create obj descriptor set layout");
	}
}

void DescriptorWrapper::createObjDescriptorPool() {
	//Setting up the pool size for the uniform buffer object descriptor
	VkDescriptorPoolSize uniformBufferObjectDescriptorPoolSize{};
	uniformBufferObjectDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformBufferObjectDescriptorPoolSize.descriptorCount = descriptorCount;

	//Setting up the pool size for the texture sampler descriptor
	VkDescriptorPoolSize textureSamplerDescriptorPoolSize{};
	textureSamplerDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureSamplerDescriptorPoolSize.descriptorCount = descriptorCount;

	VkDescriptorPoolSize descriptorPoolSizes[] = { uniformBufferObjectDescriptorPoolSize, textureSamplerDescriptorPoolSize };

	//Creating the descriptor pool create info struct, which will be passed as argument to the CreateDescriptorPool call
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.poolSizeCount = 2;
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes;
	descriptorPoolCreateInfo.maxSets = descriptorCount;

	if (vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &objDescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create obj descriptor pool");
	}
}

void DescriptorWrapper::createSkyDescriptorSetLayout() {
	//Setting up the binding for the used uniform buffer object
	VkDescriptorSetLayoutBinding uniformBufferObjectBinding{};
	uniformBufferObjectBinding.binding = 0;
	uniformBufferObjectBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformBufferObjectBinding.descriptorCount = 1;
	uniformBufferObjectBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	//Setting up the binding for the used uniform buffer object
	VkDescriptorSetLayoutBinding skyUniformBufferObjectBinding{};
	skyUniformBufferObjectBinding.binding = 1;
	skyUniformBufferObjectBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	skyUniformBufferObjectBinding.descriptorCount = 1;
	skyUniformBufferObjectBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	//Setting up the binding for the used texture sampler
	VkDescriptorSetLayoutBinding textureSamplerBinding{};
	textureSamplerBinding.binding = 2;
	textureSamplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureSamplerBinding.descriptorCount = 1;
	textureSamplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[] = { uniformBufferObjectBinding, skyUniformBufferObjectBinding, textureSamplerBinding };

	//Creating the descriptor set layout create info struct, which will be passed as argument to the CreateDescriptorSetLayout call
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = 3;
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings;

	if (vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &skyDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create sky descriptor set layout");
	}
}

void DescriptorWrapper::createSkyDescriptorPool() {
	//Setting up the pool size for the uniform buffer object descriptor
	VkDescriptorPoolSize uniformBufferObjectDescriptorPoolSize{};
	uniformBufferObjectDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformBufferObjectDescriptorPoolSize.descriptorCount = descriptorCount;

	//Setting up the pool size for the texture sampler descriptor
	VkDescriptorPoolSize textureSamplerDescriptorPoolSize{};
	textureSamplerDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureSamplerDescriptorPoolSize.descriptorCount = descriptorCount;

	VkDescriptorPoolSize descriptorPoolSizes[] = { uniformBufferObjectDescriptorPoolSize, uniformBufferObjectDescriptorPoolSize, textureSamplerDescriptorPoolSize };

	//Creating the descriptor pool create info struct, which will be passed as argument to the CreateDescriptorPool call
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.poolSizeCount = 3;
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes;
	descriptorPoolCreateInfo.maxSets = descriptorCount;

	if (vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &skyDescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create sky descriptor pool");
	}
}

void DescriptorWrapper::createDescriptorSetLayout() {

	//Setting up the binding for the used uniform buffer object
	VkDescriptorSetLayoutBinding uniformBufferObjectBinding{};
	uniformBufferObjectBinding.binding = 0;
	uniformBufferObjectBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformBufferObjectBinding.descriptorCount = 1;
	uniformBufferObjectBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	//Setting up the binding for the used texture sampler
	VkDescriptorSetLayoutBinding textureSamplerBinding{};
	textureSamplerBinding.binding = 1;
	textureSamplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureSamplerBinding.descriptorCount = Settings::TEXTURE_COUNT;
	textureSamplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding descriptorSetLayoutBindings[] = { uniformBufferObjectBinding , textureSamplerBinding };

	//Creating the descriptor set layout create info struct, which will be passed as argument to the CreateDescriptorSetLayout call
	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = 2;
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBindings;

	if (vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor set layout");
	}
}
