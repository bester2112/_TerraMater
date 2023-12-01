#ifndef TEXTUREARRAY_H
#define TEXTUREARRAY_H

#include "Settings.h"
#include "Texture.h"

class TextureArray {
public:
	TextureArray();

	TextureArray(VkDevice const &device, ImageCreator const &imageCreator);

	~TextureArray();

	Texture textures[Settings::TEXTURE_COUNT];

	Texture objTextures[Settings::OBJ_COUNT];

	Texture guiTextues[Settings::GUI_TEXTURE_COUNT];

	Texture grassTextues[Settings::GRASS_TEXTURE_COUNT];

private:

};

#endif // !TEXTUREARRAY_H
