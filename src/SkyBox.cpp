#include "SkyBox.h"

SkyBox::SkyBox() {}

SkyBox::SkyBox(VkDevice const &device, ImageCreator const &imageCreator) {
	textures[0] = Texture(device, Settings::SKYBOX_TOP_DAY_TEXTURE_PATH, imageCreator);
	textures[1] = Texture(device, Settings::SKYBOX_BOTTOM_DAY_TEXTURE_PATH, imageCreator);
	textures[2] = Texture(device, Settings::SKYBOX_LEFT_DAY_TEXTURE_PATH, imageCreator);
	textures[3] = Texture(device, Settings::SKYBOX_RIGHT_DAY_TEXTURE_PATH, imageCreator);
	textures[4] = Texture(device, Settings::SKYBOX_FRONT_DAY_TEXTURE_PATH, imageCreator);
	textures[5] = Texture(device, Settings::SKYBOX_BACK_DAY_TEXTURE_PATH, imageCreator);
	textures[6] = Texture(device, Settings::SKYBOX_TOP_NIGHT_TEXTURE_PATH, imageCreator);
	textures[7] = Texture(device, Settings::SKYBOX_BOTTOM_NIGHT_TEXTURE_PATH, imageCreator);
	textures[8] = Texture(device, Settings::SKYBOX_LEFT_NIGHT_TEXTURE_PATH, imageCreator);
	textures[9] = Texture(device, Settings::SKYBOX_RIGHT_NIGHT_TEXTURE_PATH, imageCreator);
	textures[10] = Texture(device, Settings::SKYBOX_FRONT_NIGHT_TEXTURE_PATH, imageCreator);
	textures[11] = Texture(device, Settings::SKYBOX_BACK_NIGHT_TEXTURE_PATH, imageCreator);
}

SkyBox::~SkyBox() {
	for (size_t i = 0; i < Settings::SKYBOX_TEXTURE_COUNT; i++) {
		textures[i].free();
	}
}
