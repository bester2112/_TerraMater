#include "Texture.h"

Texture::Texture() {}

Texture::Texture(VkDevice const &device, const char *filePath, ImageCreator const &imageCreator)
	: device(device) {
	imageCreator.createTextureImage(filePath, textureImage, textureImageMemory);
	imageCreator.createTextureImageView(textureImage, textureImageView);
	imageCreator.createTextureSampler(textureSampler);
}

Texture::~Texture() {}

void Texture::free() {
	vkDestroySampler(device, textureSampler, nullptr);
	vkDestroyImageView(device, textureImageView, nullptr);
	vkDestroyImage(device, textureImage, nullptr);
	vkFreeMemory(device, textureImageMemory, nullptr);
}
