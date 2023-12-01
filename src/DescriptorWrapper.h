/**
 * @file DescriptorWrapper.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Contains a class which wraps all vulkan descriptor functionality.
 * @version 0.1
 * @date 2020-05-04
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef DESCRIPTORWRAPPER_H
#define DESCRIPTORWRAPPER_H

#include "TextureArray.h"
#include "SkyBox.h"
#include "CloudTexture.h"

#include "vulkan/vulkan.h"

#include <vector>

 /**
  * @brief Wrapper class containing all neccessary vulkan descriptor functionality into one place.
  */
class DescriptorWrapper {
public:
	/**
	 * @brief Handle of the descriptor set layout.
	 */
	VkDescriptorSetLayout descriptorSetLayout;

	/**
	 * @brief Handle for the descriptor pool.
	 */
	VkDescriptorPool descriptorPool;

	/**
	* @brief Handle of the descriptor sets.
	*/
	std::vector<VkDescriptorSet> descriptorSets;

	DescriptorWrapper();

	/**
	 * @brief Constructs a new Descriptor Wrapper object.
	 *
	 * Sets up the descriptor set layout.
	 * Creates the descriptor pool.
	 *
	 * @param device Handle to the vulkan device, needed for the create calls.
	 * @param descriptorCount Specifies how many descriptors for each type the pool should store.
	 */
	DescriptorWrapper(VkDevice &device, uint32_t descriptorCount);

	~DescriptorWrapper();

	/**
	 * @brief Creates the descriptor sets.
	 *
	 * @param uniformBuffers The uniform buffers which will be used in the descriptors.
	 * @param texture Contains the needed texture image view and texture sampler.
	 */
	void createDescriptorSets(std::vector<VkBuffer> const &uniformBuffers, TextureArray const &textureArray);

	VkDescriptorSetLayout quadDescriptorSetLayout;
	VkDescriptorSetLayout computeDescriptorSetLayout;

	VkDescriptorPool quadDescriptorPool;
	VkDescriptorPool computeDescriptorPool;
	
	std::vector<VkDescriptorSet> quadDescriptorSets;
	std::vector<VkDescriptorSet> computeDescriptorSets;

	void createQuadDescriptorSets(std::vector<VkBuffer> const &uniformBuffers, VkImageView const &imageView, VkSampler const &sampler);
	void createComputeDescriptorSets(std::vector<VkBuffer> const &cameraDataBuffers, VkImageView const &imageView, VkSampler const &sampler, TextureArray const &textureArray, SkyBox const &skyBox, VkBuffer const &boxDataBuffer, std::vector<BoxData> const &boxData, VkBuffer const &emissiveBoxDataBuffer, std::vector<BoxData> const &emissiveBoxData, VkBuffer const &pointLightBuffer, std::vector<PointLight> const &pointLights, VkBuffer const &writeBackDataBuffer, bool &allocated);

	VkDescriptorSetLayout objDescriptorSetLayout;
	VkDescriptorPool objDescriptorPool;
	std::vector<VkDescriptorSet> objDescriptorSets;

	void createObjDescriptorSets(std::vector<VkBuffer> const &uniformBuffers, TextureArray const &textureArray);

	VkDescriptorSetLayout skyDescriptorSetLayout;
	VkDescriptorPool skyDescriptorPool;
	std::vector<VkDescriptorSet> skyDescriptorSets;

	void createSkyDescriptorSets(std::vector<VkBuffer> const &uniformBuffers, std::vector<VkBuffer> const &skyUniformBuffers, CloudTexture const &cloudTexture);

private:
	/**
	* @brief Reference to the device, whis is need for some vulkan function calls.
	*/
	VkDevice device;

	/**
	* @brief Specifies how many descriptors for each type the pool should store.
	*/
	uint32_t descriptorCount;

	/**
	 * @brief Creates the descriptor set layout.
	 */
	void createDescriptorSetLayout();

	/**
	 * @brief Creates the descriptor pool, which is later used for the descriptor sets.
	 *
	 * @param descriptorCount Specifies how many descriptors for each type the pool should store.
	 */
	void createDescriptorPool();

	void createComputeDescriptorSetLayout();
	void createComputeDescriptorPool();

	void createObjDescriptorSetLayout();
	void createObjDescriptorPool();

	void createSkyDescriptorSetLayout();
	void createSkyDescriptorPool();
};

#endif // !DESCRIPTORWRAPPER_H
