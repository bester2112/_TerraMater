/**
 * @file Texture.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Contains a data class, which contains all neccessary vulkan texture attributes.
 * @version 0.1
 * @date 2020-05-05
 *
 * @copyright Copyright (c) 2020
 *
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "ImageCreator.h"

#include "vulkan/vulkan.h"

 /**
  * @brief Contains all neccessary vulkan texture attributes.
  */
class Texture {
public:

	/**
	 * @brief Handle of the texture image.
	 */
	VkImage textureImage;

	/**
	 * @brief Handle of the texture image view.
	 */
	VkImageView textureImageView;

	/**
	 * @brief Handle of the sampler for this texture.
	 */
	VkSampler textureSampler;

	/**
	 * @brief //Handle of the Memory for this texture.
	 */
	VkDeviceMemory textureImageMemory;

	Texture();

	/**
	 * @brief Constructs a new texture object.
	 *
	 * Creates textureImage, textureImageMemory, textureImageView and textureSampler.
	 *
	 * @param Reference to the device, whis is needed for the destructor call.
	 * @param filePath Path to the texture, which should be loaded.
	 * @param imageCreator Reference to the image creator, needed for the creation of the texture attributes.
	 */
	Texture(VkDevice const &device, const char *filePath, ImageCreator const &imageCreator);

	~Texture();

	void free();

private:

	/**
	* @brief Reference to the device, whis is needed for some vulkan function calls.
	*/
	VkDevice device;
};

#endif // !TEXTURE_H
