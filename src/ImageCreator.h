/**
 * @file ImageCreator.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Contains a creator class, to create vulkan images (and allocate the needed memory) and image views.
 * @version 0.1
 * @date 2020-05-04
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef IMAGEVIEWCREATOR_H
#define IMAGEVIEWCREATOR_H

#include "QueueFamilyIndices.h"
#include "CommandWrapper.h"
#include "BufferCreator.h"

#include "vulkan/vulkan.h"

 /**
  * @brief Helper class to create vulkan images (including allocating the neccessary memory) and vulkan image views.
  */
class ImageCreator {
public:
	ImageCreator();

	/**
	 * @brief Constructs a new image creator object.
	 *
	 * @param physicalDevice Handle of the physical device, used to determine the neccessary memory type.
	 * @param device Handle of the vulkan device, needed for the createImage call.
	 * @param queueFamilyIndices Queue family indices needed for clarification, which queues are going to access the image.
	 * @param commandWrapper Reference to the commandWrapper, needed to actually submit the commands to the commandBuffers.
	 * @param bufferCreator Reference to the bufferCreator, needed to load textures from a file into a buffer.
	 */
	ImageCreator(VkPhysicalDevice const &physicalDevice, VkDevice const &device, QueueFamilyIndices const &queueFamilyIndices, CommandWrapper &commandWrapper, BufferCreator const &bufferCreator);

	~ImageCreator();

	/**
	 * @brief Creates an vulkan image object and allocates the neccessary memory.
	 *
	 * @param width Width of the created image.
	 * @param height Height of the created image.
	 * @param format Format in which the image should be stored.
	 * @param imageTiling Setting which should be used for the tiling of the created image. (Layout of the image in memory, for example linear or optimal).
	 * @param imageUsageFlags Flags which describe the usage of the created image.
	 * @param memoryPropertyFlags Flags which describe the needed memory properties for the created image.
	 * @param image Handle in which the created image gets stored.
	 * @param imageMemory Handle for the allocated image memory.
	 */
	void createImage(uint32_t const width, uint32_t const height, VkFormat const format, VkImageTiling const imageTiling, VkImageUsageFlags const imageUsageFlags, VkMemoryPropertyFlags const memoryPropertyFlags, VkImage &image, VkDeviceMemory &imageMemory) const;

	void create3DImage(uint32_t const width, uint32_t const height, uint32_t const depth, VkFormat const format, VkImageTiling const imageTiling, VkImageUsageFlags const imageUsageFlags, VkMemoryPropertyFlags const memoryPropertyFlags, VkImage &image, VkDeviceMemory &imageMemory) const;

	/**
	 * @brief Creates an vulkan image view.
	 *
	 * @param image Handle of the image, for which a handle should be created.
	 * @param format Format for accessing the image using the view.
	 * @param aspectFlags Flags describing the usage of the image view.
	 * @param imageView Handle containing the created image view.
	 */
	void createImageView(VkImage const image, VkFormat const format, VkImageAspectFlags const aspectFlags, VkImageViewType const &viewType, VkImageView &imageView) const;

	/**
	 * @brief Creates a texture image view.
	 *
	 * @param textureImage Texture image, for which an view should be created.
	 * @param textureImageView Handle of the created texture image view.
	 */
	void createTextureImageView(VkImage const textureImage, VkImageView &textureImageView) const;

	

	/**
	 * @brief Creates a texture image.
	 *
	 * @param filePath Path to texture file.
	 * @param textureImage Handle of the created texture image.
	 * @param textureImageMemory Handle of the allocated texture image memory.
	 */
	void createTextureImage(char const *filePath, VkImage &textureImage, VkDeviceMemory &textureImageMemory) const;

	

	/**
	 * @brief Create a Texture sampler.
	 *
	 * @param textureSampler Handle of the created texture sampler.
	 */
	void createTextureSampler(VkSampler &textureSampler) const;
	
	void createCloudTextureImageView(VkImage const textureImage, VkImageView &textureImageView) const;
	void createCloudTextureImage(glm::vec4 values[], uint32_t const imageWidth, uint32_t const imageHeight, uint32_t const imageDepth, VkImage &textureImage, VkDeviceMemory &textureImageMemory) const;
	void createCloudTextureSampler(VkSampler &textureSampler) const;

	/**
	 * @brief Transitions a given image from an old layout into a new one.
	 *
	 * @param image Image which should be transitioned into a new layout.
	 * @param format Format of the image to use.
	 * @param oldImageLayout Layout the image uses currently.
	 * @param newImageLayout Layout the image should have after the transition.
	 */
	void transitionImageLayout(VkImage image, VkFormat const format, VkImageLayout const oldImageLayout, VkImageLayout const newImageLayout) const;

	/**
	 * @brief Finds a suitable depth format and returns it.
	 *
	 * @return VkFormat The found depth format.
	 */
	VkFormat findDepthFormat() const;

	void createComputTexture(uint32_t const width, uint32_t const height, VkImage &computeTextureImage, VkDeviceMemory &computeTextureImageMemory) const;
	void createComputeTextureSampler(VkSampler &computeTextureSampler) const;
	void createComputeTextureView(VkImage const computeTextureImage, VkImageView &computeTextureImageView) const;

private:

	/**
	 * @brief Reference to the physical device, which is need for infos about the available memory types.
	 */
	VkPhysicalDevice physicalDevice;

	/**
	 * @brief Reference to the device, whis is need for some vulkan function calls.
	 */
	VkDevice device;

	/**
	 * @brief Needed to determine which queues will have acces to the generated images.
	 */
	QueueFamilyIndices queueFamilyIndices;

	/**
	 * @brief Reference to the commandWrapper, needed to actually submit the commands to the commandBuffers.
	 */
	CommandWrapper *commandWrapper;

	/**
	 * @brief Reference to the bufferCreator, needed to load textures from a file into a buffer.
	 */
	BufferCreator bufferCreator;

	/**
	 * @brief Copys a buffer to an image.
	 *
	 * @param buffer Buffer which should be copied into an image.
	 * @param image Image into which the content of the buffer should be copied.
	 * @param imageWidth Width of the image.
	 * @param imageHeight Height of the image.
	 */
	void copyBufferToImage(VkBuffer const buffer, VkImage image, uint32_t const imageWidth, uint32_t const imageHeight, uint32_t const imageDepth) const;

	/**
	 * @brief Finds an support VkFormat given an list of canditates and flags enclosing the available ones.
	 *
	 * @param formatCandidates Candidates to choose from.
	 * @param imageTiling What imageTiling style should be used (optimal or linear).
	 * @param formatFeatureFlags Flags describing the feature set of the Format.
	 * @return VkFormat The found format.
	 */
	VkFormat findSupportedFormat(std::vector<VkFormat> const &formatCandidates, VkImageTiling const imageTiling, VkFormatFeatureFlags formatFeatureFlags) const;

	/**
	 * @brief Function to check if a given format contains a stencil component.
	 *
	 * @param format The format which is going to be checked.
	 * @return true If the format contains a stencil component.
	 * @return false If the format doesn't contain a stencil component.
	 */
	bool hasStencilComponent(VkFormat const format) const;
};

#endif // !IMAGEVIEWCREATOR_H
