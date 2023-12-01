#ifndef CLOUDTEXTURE_H
#define CLOUDTEXTURE_H

#include "Noise.h"
#include "Settings.h"
#include "ImageCreator.h"

#include "vulkan/vulkan.h"

class CloudTexture {
public:
	CloudTexture(VkDevice const &device, ImageCreator const &imageCreator);
	~CloudTexture();

	glm::vec4 values[Settings::cloudTextureSize * Settings::cloudTextureSize * Settings::cloudTextureSize];
	
	float noiseValues[7][Settings::cloudTextureSize * Settings::cloudTextureSize * Settings::cloudTextureSize];

	VkImage textureImage;
	VkImageView textureImageView;
	VkSampler textureSampler;
	VkDeviceMemory textureImageMemory;

private:
	VkDevice device;

	Noise noise;

	void setUpNoise();

	void parallelNoiseRange(int start, int end);

	float readNoise(glm::vec3 const &position, int level, float frequency);

	void setUpTexture();

	void createTextureImage(ImageCreator const &imageCreator);

	void createTexturePPM();

	void createTextureFiles();

	void loadTextureFiles();
};

#endif // !CLOUDTEXTURE_H
