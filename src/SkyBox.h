#ifndef SKYBOX_H
#define SKYBOX_H

#include "Settings.h"
#include "Texture.h"

class SkyBox {
public:
	SkyBox();
	SkyBox(VkDevice const &device, ImageCreator const &imageCreator);
	~SkyBox();

	Texture textures[Settings::SKYBOX_TEXTURE_COUNT];

private:

};

#endif // !SKYBOX_H
