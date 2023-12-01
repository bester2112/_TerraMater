#include "TextureArray.h"
#include "CubeType.h"
#include "ObjType.h"
#include "GuiType.h"
#include "GrassType.h"

TextureArray::TextureArray() {}

TextureArray::TextureArray(VkDevice const &device, ImageCreator const &imageCreator) :textures() {
	for (size_t i = 0; i < Settings::TEXTURE_COUNT; i++) {
		textures[i] = Texture(device, Settings::MISSING_TEXTURE_PATH, imageCreator);
	}

	for (size_t i = 0; i < Settings::OBJ_COUNT; i++) {
		objTextures[i] = Texture(device, Settings::MISSING_TEXTURE_PATH, imageCreator);
	}

	for (size_t i = 0; i < Settings::GUI_TEXTURE_COUNT; i++) {
		guiTextues[i] = Texture(device, Settings::MISSING_TEXTURE_PATH, imageCreator);
	}

	for (size_t i = 0; i < Settings::GRASS_TEXTURE_COUNT; i++) {
		grassTextues[i] = Texture(device, Settings::MISSING_TEXTURE_PATH, imageCreator);
	}

	textures[CubeType::WATER].free();
	textures[CubeType::WATER] = Texture(device, Settings::WATER_TEXTURE_PATH, imageCreator);

	textures[CubeType::GLASS].free();
	textures[CubeType::GLASS] = Texture(device, Settings::GLASS_TEXTURE_PATH, imageCreator);

	textures[CubeType::BEDROCK].free();
	textures[CubeType::BEDROCK] = Texture(device, Settings::BEDROCK_TEXTURE_PATH, imageCreator);

	textures[CubeType::DIRT].free();
	textures[CubeType::DIRT] = Texture(device, Settings::DIRT_TEXTURE_PATH, imageCreator);

	textures[CubeType::GRASS_BLOCK].free();
	textures[CubeType::GRASS_BLOCK] = Texture(device, Settings::GRASS_BLOCK_TEXTURE_PATH, imageCreator);

	textures[CubeType::GRASS_BLOCK_TOP].free();
	textures[CubeType::GRASS_BLOCK_TOP] = Texture(device, Settings::GRASS_BLOCK_TOP_TEXTURE_PATH, imageCreator);

	textures[CubeType::STONE].free();
	textures[CubeType::STONE] = Texture(device, Settings::STONE_TEXTURE_PATH, imageCreator);

	textures[CubeType::SNOW].free();
	textures[CubeType::SNOW] = Texture(device, Settings::SNOW_TEXTURE_PATH, imageCreator);

	textures[CubeType::SAND].free();
	textures[CubeType::SAND] = Texture(device, Settings::SAND_TEXTURE_PATH, imageCreator);

	textures[CubeType::COBBLESTONE].free();
	textures[CubeType::COBBLESTONE] = Texture(device, Settings::COBBLESTONE_TEXTURE_PATH, imageCreator);

	textures[CubeType::ACACIA_LEAVES].free();
	textures[CubeType::ACACIA_LEAVES] = Texture(device, Settings::ACACIA_LEAVES_TEXTURE_PATH, imageCreator);

	textures[CubeType::ACACIA_LOG].free();
	textures[CubeType::ACACIA_LOG] = Texture(device, Settings::ACACIA_LOG_TEXTURE_PATH, imageCreator);

	textures[CubeType::ACACIA_LOG_TOP].free();
	textures[CubeType::ACACIA_LOG_TOP] = Texture(device, Settings::ACACIA_LOG_TOP_TEXTURE_PATH, imageCreator);

	textures[CubeType::BIRCH_LEAVES].free();
	textures[CubeType::BIRCH_LEAVES] = Texture(device, Settings::BIRCH_LEAVES_TEXTURE_PATH, imageCreator);

	textures[CubeType::BIRCH_LOG].free();
	textures[CubeType::BIRCH_LOG] = Texture(device, Settings::BIRCH_LOG_TEXTURE_PATH, imageCreator);

	textures[CubeType::BIRCK_LOG_TOP].free();
	textures[CubeType::BIRCK_LOG_TOP] = Texture(device, Settings::BIRCK_LOG_TOP_TEXTURE_PATH, imageCreator);

	textures[CubeType::CACTUS].free();
	textures[CubeType::CACTUS] = Texture(device, Settings::CACTUS_TEXTURE_PATH, imageCreator);

	textures[CubeType::CACTUS_TOP].free();
	textures[CubeType::CACTUS_TOP] = Texture(device, Settings::CACTUS_TOP_TEXTURE_PATH, imageCreator);

	textures[CubeType::DARK_OAK_LEAVES].free();
	textures[CubeType::DARK_OAK_LEAVES] = Texture(device, Settings::DARK_OAK_LEAVES_TEXTURE_PATH, imageCreator);

	textures[CubeType::DARK_OAK_LOG].free();
	textures[CubeType::DARK_OAK_LOG] = Texture(device, Settings::DARK_OAK_LOG_TEXTURE_PATH, imageCreator);

	textures[CubeType::DARK_OAK_LOG_TOP].free();
	textures[CubeType::DARK_OAK_LOG_TOP] = Texture(device, Settings::DARK_OAK_LOG_TOP_TEXTURE_PATH, imageCreator);

	textures[CubeType::OAK_LEAVES].free();
	textures[CubeType::OAK_LEAVES] = Texture(device, Settings::OAK_LEAVES_TEXTURE_PATH, imageCreator);

	textures[CubeType::OAK_LOG].free();
	textures[CubeType::OAK_LOG] = Texture(device, Settings::OAK_LOG_TEXTURE_PATH, imageCreator);

	textures[CubeType::OAK_LOG_TOP].free();
	textures[CubeType::OAK_LOG_TOP] = Texture(device, Settings::OAK_LOG_TOP_TEXTURE_PATH, imageCreator);

	textures[CubeType::SPRUCE_LEAVES].free();
	textures[CubeType::SPRUCE_LEAVES] = Texture(device, Settings::SPRUCE_LEAVES_TEXTURE_PATH, imageCreator);

	textures[CubeType::SPRUCE_LOG].free();
	textures[CubeType::SPRUCE_LOG] = Texture(device, Settings::SPRUCE_LOG_TEXTURE_PATH, imageCreator);

	textures[CubeType::SPRUCE_LOG_TOP].free();
	textures[CubeType::SPRUCE_LOG_TOP] = Texture(device, Settings::SPRUCE_LOG_TOP_TEXTURE_PATH, imageCreator);

	textures[CubeType::DARK_GRASS_BLOCK].free();
	textures[CubeType::DARK_GRASS_BLOCK] = Texture(device, Settings::DARK_GRASS_BLOCK_TEXTURE_PATH, imageCreator);

	textures[CubeType::DARK_GRASS_BLOCK_TOP].free();
	textures[CubeType::DARK_GRASS_BLOCK_TOP] = Texture(device, Settings::DARK_GRASS_BLOCK_TOP_TEXTURE_PATH, imageCreator);

	objTextures[ObjType::SUNFLOWER].free();
	objTextures[ObjType::SUNFLOWER] = Texture(device, Settings::SUNFLOWER_TEXTURE_PATH, imageCreator);

	objTextures[ObjType::LOTUS].free();
	objTextures[ObjType::LOTUS] = Texture(device, Settings::LOTUS_TEXTURE_PATH, imageCreator);

	objTextures[ObjType::SUCCULENT].free();
	objTextures[ObjType::SUCCULENT] = Texture(device, Settings::SUCCULENT_TEXTURE_PATH, imageCreator);

	guiTextues[GuiType::ACACIA_LEAVES_GUI].free();
	guiTextues[GuiType::ACACIA_LEAVES_GUI] = Texture(device, Settings::ACACIA_LEAVES_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::ACACIA_LOG_GUI].free();
	guiTextues[GuiType::ACACIA_LOG_GUI] = Texture(device, Settings::ACACIA_LOG_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::ANDESITE_GUI].free();
	guiTextues[GuiType::ANDESITE_GUI] = Texture(device, Settings::ANDESITE_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::BEDROCK_GUI].free();
	guiTextues[GuiType::BEDROCK_GUI] = Texture(device, Settings::BEDROCK_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::BIRCH_LEAVES_GUI].free();
	guiTextues[GuiType::BIRCH_LEAVES_GUI] = Texture(device, Settings::BIRCH_LEAVES_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::CACTUS_GUI].free();
	guiTextues[GuiType::CACTUS_GUI] = Texture(device, Settings::CACTUS_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::COBBLESTONE_GUI].free();
	guiTextues[GuiType::COBBLESTONE_GUI] = Texture(device, Settings::COBBLESTONE_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::DARK_OAK_LEAVES_GUI].free();
	guiTextues[GuiType::DARK_OAK_LEAVES_GUI] = Texture(device, Settings::DARK_OAK_LEAVES_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::DARK_OAK_LOG_GUI].free();
	guiTextues[GuiType::DARK_OAK_LOG_GUI] = Texture(device, Settings::DARK_OAK_LOG_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::DARKGRASS_GUI].free();
	guiTextues[GuiType::DARKGRASS_GUI] = Texture(device, Settings::DARKGRASS_BLOCK_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::DIRT_GUI].free();
	guiTextues[GuiType::DIRT_GUI] = Texture(device, Settings::DIRT_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::GLASS_GUI].free();
	guiTextues[GuiType::GLASS_GUI] = Texture(device, Settings::GLASS_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::GRASS_GUI].free();
	guiTextues[GuiType::GRASS_GUI] = Texture(device, Settings::GRASS_BLOCK_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::GRAVEL_GUI].free();
	guiTextues[GuiType::GRAVEL_GUI] = Texture(device, Settings::GRAVEL_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::ICE_GUI].free();
	guiTextues[GuiType::ICE_GUI] = Texture(device, Settings::ICE_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::OAK_LEAVES_GUI].free();
	guiTextues[GuiType::OAK_LEAVES_GUI] = Texture(device, Settings::OAK_LEAVES_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::OAK_LOG_GUI].free();
	guiTextues[GuiType::OAK_LOG_GUI] = Texture(device, Settings::OAK_LOG_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::OAK_PLANKS_GUI].free();
	guiTextues[GuiType::OAK_PLANKS_GUI] = Texture(device, Settings::OAK_PLANKS_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::SAND_GUI].free();
	guiTextues[GuiType::SAND_GUI] = Texture(device, Settings::SAND_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::SANDSTONE_GUI].free();
	guiTextues[GuiType::SANDSTONE_GUI] = Texture(device, Settings::SANDSTONE_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::SNOW_GUI].free();
	guiTextues[GuiType::SNOW_GUI] = Texture(device, Settings::SNOW_BLOCK_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::SPRUCE_LEAVES_GUI].free();
	guiTextues[GuiType::SPRUCE_LEAVES_GUI] = Texture(device, Settings::SPRUCE_LEAVES_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::SPRUCE_LOG_GUI].free();
	guiTextues[GuiType::SPRUCE_LOG_GUI] = Texture(device, Settings::SPRUCE_LOG_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::STONE_GUI].free();
	guiTextues[GuiType::STONE_GUI] = Texture(device, Settings::STONE_BLOCK_PATH, imageCreator);

	guiTextues[GuiType::DEFAULT_GUI].free();
	guiTextues[GuiType::DEFAULT_GUI] = Texture(device, Settings::DEFAULT_TEXTURE_PATH, imageCreator);
	
	guiTextues[GuiType::HUD_ARROW_LEFT_SELECTED].free();
	guiTextues[GuiType::HUD_ARROW_LEFT_SELECTED] = Texture(device, Settings::HUD_ARROW_LEFT_SELECTED, imageCreator);

	guiTextues[GuiType::HUD_ARROW_LEFT_UNSELECTED].free();
	guiTextues[GuiType::HUD_ARROW_LEFT_UNSELECTED] = Texture(device, Settings::HUD_ARROW_LEFT_UNSELECTED, imageCreator);

	guiTextues[GuiType::HUD_ARROW_RIGHT_SELECTED].free();
	guiTextues[GuiType::HUD_ARROW_RIGHT_SELECTED] = Texture(device, Settings::HUD_ARROW_RIGHT_SELECTED, imageCreator);

	guiTextues[GuiType::HUD_ARROW_RIGHT_UNSELECTED].free();
	guiTextues[GuiType::HUD_ARROW_RIGHT_UNSELECTED] = Texture(device, Settings::HUD_ARROW_RIGHT_UNSELECTED, imageCreator);

	guiTextues[GuiType::HUD_HOTBAR].free();
	guiTextues[GuiType::HUD_HOTBAR] = Texture(device, Settings::HUD_HOTBAR, imageCreator);

	guiTextues[GuiType::HUD_MENUE].free();
	guiTextues[GuiType::HUD_MENUE] = Texture(device, Settings::HUD_MENUE, imageCreator);

	guiTextues[GuiType::HUD_PAGE_SELECTED].free();
	guiTextues[GuiType::HUD_PAGE_SELECTED] = Texture(device, Settings::HUD_PAGE_SELECTED, imageCreator);

	guiTextues[GuiType::HUD_PAGE_UNSELECTED].free();
	guiTextues[GuiType::HUD_PAGE_UNSELECTED] = Texture(device, Settings::HUD_PAGE_UNSELECTED, imageCreator);

	guiTextues[GuiType::HUD_BACKGROUND_SELECTED].free();
	guiTextues[GuiType::HUD_BACKGROUND_SELECTED] = Texture(device, Settings::HUD_BACKGROUND_SELECTED, imageCreator);

	guiTextues[GuiType::HUD_BACKGROUND_UNSELECTED_HOTBAR].free();
	guiTextues[GuiType::HUD_BACKGROUND_UNSELECTED_HOTBAR] = Texture(device, Settings::HUD_BACKGROUND_UNSELECTED_HOTBAR, imageCreator);

	guiTextues[GuiType::HUD_BACKGROUND_UNSELECTED_MENUE].free();
	guiTextues[GuiType::HUD_BACKGROUND_UNSELECTED_MENUE] = Texture(device, Settings::HUD_BACKGROUND_UNSELECTED_MENUE, imageCreator);

	guiTextues[GuiType::HUD_TRANSPARENT].free();
	guiTextues[GuiType::HUD_TRANSPARENT] = Texture(device, Settings::HUD_TRANSPARENT, imageCreator);

	grassTextues[GrassType::AZURE_BLUET].free();
	grassTextues[GrassType::AZURE_BLUET] = Texture(device, Settings::GRASS_AZURE_BLUET, imageCreator);

	grassTextues[GrassType::CORNFLOWER].free();
	grassTextues[GrassType::CORNFLOWER] = Texture(device, Settings::GRASS_CORNFLOWER, imageCreator);

	grassTextues[GrassType::DANDELION].free();
	grassTextues[GrassType::DANDELION] = Texture(device, Settings::GRASS_DANDELION, imageCreator);

	grassTextues[GrassType::GRASS].free();
	grassTextues[GrassType::GRASS] = Texture(device, Settings::GRASS_GRASS, imageCreator);

	grassTextues[GrassType::LILAC_BOTTOM].free();
	grassTextues[GrassType::LILAC_BOTTOM] = Texture(device, Settings::GRASS_LILAC_BOTTOM, imageCreator);

	grassTextues[GrassType::LILAC_TOP].free();
	grassTextues[GrassType::LILAC_TOP] = Texture(device, Settings::GRASS_LILAC_TOP, imageCreator);

	grassTextues[GrassType::LILY_OF_THE_VALLEY].free();
	grassTextues[GrassType::LILY_OF_THE_VALLEY] = Texture(device, Settings::GRASS_LILY_OF_THE_VALLEY, imageCreator);

	grassTextues[GrassType::ORANGE_TULIP].free();
	grassTextues[GrassType::ORANGE_TULIP] = Texture(device, Settings::GRASS_ORANGE_TULIP, imageCreator);
	
	grassTextues[GrassType::PINK_TULIP].free();
	grassTextues[GrassType::PINK_TULIP] = Texture(device, Settings::GRASS_PINK_TULIP, imageCreator);
	
	grassTextues[GrassType::POPPY].free();
	grassTextues[GrassType::POPPY] = Texture(device, Settings::GRASS_POPPY, imageCreator);
	
	grassTextues[GrassType::RED_TULIP].free();
	grassTextues[GrassType::RED_TULIP] = Texture(device, Settings::GRASS_RED_TULIP, imageCreator);

	grassTextues[GrassType::SUGAR_CANE].free();
	grassTextues[GrassType::SUGAR_CANE] = Texture(device, Settings::GRASS_SUGAR_CANE, imageCreator);

	grassTextues[GrassType::TALL_GRASS_BOTTOM].free();
	grassTextues[GrassType::TALL_GRASS_BOTTOM] = Texture(device, Settings::GRASS_TALL_GRASS_BOTTOM, imageCreator);

	grassTextues[GrassType::TALL_GRASS_TOP].free();
	grassTextues[GrassType::TALL_GRASS_TOP] = Texture(device, Settings::GRASS_TALL_GRASS_TOP, imageCreator);
	
	grassTextues[GrassType::WHEAT_STAGE7].free();
	grassTextues[GrassType::WHEAT_STAGE7] = Texture(device, Settings::GRASS_WHEAT_STAGE7, imageCreator);
	
	grassTextues[GrassType::WHITE_TULIP].free();
	grassTextues[GrassType::WHITE_TULIP] = Texture(device, Settings::GRASS_WHITE_TULIP, imageCreator);


}

TextureArray::~TextureArray() {
	for (size_t i = 0; i < Settings::TEXTURE_COUNT; i++) {
		textures[i].free();
	}

	for (size_t i = 0; i < Settings::OBJ_COUNT; i++) {
		objTextures[i].free();
	}

	for (size_t i = 0; i < Settings::GUI_TEXTURE_COUNT; i++) {
		guiTextues[i].free();
	}

	for (size_t i = 0; i < Settings::GRASS_TEXTURE_COUNT; i++) {
		grassTextues[i].free();
	}
}
