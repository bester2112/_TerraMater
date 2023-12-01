/**
 * @file Settings.h
 * @author Tim Lissek (usdko@student.kit.edu)
 * @brief Holds the Settings class, which is used to provide static access to game settings.
 * @version 0.1
 * @date 2020-05-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include "BiomData.h"
#include "TreeDescription.h"
#include "SkyUBO.h"

/**
 * @brief "Static" class, which provides helpful game settings.
 */
class Settings {
public:
	static int WINDOW_WIDTH;
	static int WINDOW_HEIGHT;

	static constexpr char const WINDOW_NAME[] = "Terra Mater";
    static constexpr char const ENGINE_NAME[] = "KITty Cat Engine";

    static constexpr char const VERTEX_SHADER_PATH[] = "shaders/shader.vert.spv";
    static constexpr char const FRAGMENT_SHADER_PATH[] = "shaders/shader.frag.spv";

    static int const TEXTURE_COUNT = 32;
    static constexpr char const MISSING_TEXTURE_PATH[] = "textures/missing.png";

    static constexpr char const WATER_TEXTURE_PATH[] = "textures/water_overlay.png";
    static constexpr char const GLASS_TEXTURE_PATH[] = "textures/glass.png";
    static constexpr char const BEDROCK_TEXTURE_PATH[] = "textures/bedrock.png";
    static constexpr char const DIRT_TEXTURE_PATH[] = "textures/dirt.png";
    static constexpr char const GRASS_BLOCK_TEXTURE_PATH[] = "textures/grass_block.png";
    static constexpr char const GRASS_BLOCK_TOP_TEXTURE_PATH[] = "textures/grass_block_top.png";
    static constexpr char const STONE_TEXTURE_PATH[] = "textures/stone.png";
    static constexpr char const SNOW_TEXTURE_PATH[] = "textures/snow.png";
    static constexpr char const SAND_TEXTURE_PATH[] = "textures/sand.png";
    static constexpr char const COBBLESTONE_TEXTURE_PATH[] = "textures/cobblestone.png";
    static constexpr char const ACACIA_LEAVES_TEXTURE_PATH[] = "textures/acacia_leaves.png";
    static constexpr char const ACACIA_LOG_TEXTURE_PATH[] = "textures/acacia_log.png";
    static constexpr char const ACACIA_LOG_TOP_TEXTURE_PATH[] = "textures/acacia_log_top.png";
    static constexpr char const BIRCH_LEAVES_TEXTURE_PATH[] = "textures/birch_leaves.png";
    static constexpr char const BIRCH_LOG_TEXTURE_PATH[] = "textures/birch_log.png";
    static constexpr char const BIRCK_LOG_TOP_TEXTURE_PATH[] = "textures/birch_log_top.png";
    static constexpr char const CACTUS_TEXTURE_PATH[] = "textures/cactus.png";
    static constexpr char const CACTUS_TOP_TEXTURE_PATH[] = "textures/cactus_top.png";
    static constexpr char const DARK_OAK_LEAVES_TEXTURE_PATH[] = "textures/dark_oak_leaves.png";
    static constexpr char const DARK_OAK_LOG_TEXTURE_PATH[] = "textures/dark_oak_log.png";
    static constexpr char const DARK_OAK_LOG_TOP_TEXTURE_PATH[] = "textures/dark_oak_log_top.png";
    static constexpr char const OAK_LEAVES_TEXTURE_PATH[] = "textures/oak_leaves.png";
    static constexpr char const OAK_LOG_TEXTURE_PATH[] = "textures/oak_log.png";
    static constexpr char const OAK_LOG_TOP_TEXTURE_PATH[] = "textures/oak_log_top.png";
    static constexpr char const SPRUCE_LEAVES_TEXTURE_PATH[] = "textures/spruce_leaves.png";
    static constexpr char const SPRUCE_LOG_TEXTURE_PATH[] = "textures/spruce_log.png";
    static constexpr char const SPRUCE_LOG_TOP_TEXTURE_PATH[] = "textures/spruce_log_top.png";
    static constexpr char const DARK_GRASS_BLOCK_TEXTURE_PATH[] = "textures/dark_grass_block.png";
    static constexpr char const DARK_GRASS_BLOCK_TOP_TEXTURE_PATH[] = "textures/dark_grass_block_top.png";

    static int const SKYBOX_TEXTURE_COUNT = 12;

    static constexpr char const SKYBOX_TOP_DAY_TEXTURE_PATH[] = "textures/skybox/skybox_top_day.png";
    static constexpr char const SKYBOX_BOTTOM_DAY_TEXTURE_PATH[] = "textures/skybox/skybox_bottom_day.png";
    static constexpr char const SKYBOX_LEFT_DAY_TEXTURE_PATH[] = "textures/skybox/skybox_left_day.png";
    static constexpr char const SKYBOX_RIGHT_DAY_TEXTURE_PATH[] = "textures/skybox/skybox_right_day.png";
    static constexpr char const SKYBOX_FRONT_DAY_TEXTURE_PATH[] = "textures/skybox/skybox_front_day.png";
    static constexpr char const SKYBOX_BACK_DAY_TEXTURE_PATH[] = "textures/skybox/skybox_back_day.png";
    static constexpr char const SKYBOX_TOP_NIGHT_TEXTURE_PATH[] = "textures/skybox/skybox_top_night.png";
    static constexpr char const SKYBOX_BOTTOM_NIGHT_TEXTURE_PATH[] = "textures/skybox/skybox_bottom_night.png";
    static constexpr char const SKYBOX_LEFT_NIGHT_TEXTURE_PATH[] = "textures/skybox/skybox_left_night.png";
    static constexpr char const SKYBOX_RIGHT_NIGHT_TEXTURE_PATH[] = "textures/skybox/skybox_right_night.png";
    static constexpr char const SKYBOX_FRONT_NIGHT_TEXTURE_PATH[] = "textures/skybox/skybox_front_night.png";
    static constexpr char const SKYBOX_BACK_NIGHT_TEXTURE_PATH[] = "textures/skybox/skybox_back_night.png";

    static constexpr char const OBJ_VERTEX_SHADER_PATH[] = "shaders/obj.vert.spv";
    static constexpr char const OBJ_FRAGMENT_SHADER_PATH[] = "shaders/obj.frag.spv";

    static int const OBJ_COUNT = 5;

    static constexpr char const ACACIA_LEAVES_BLOCK_PATH[] = "textures/blocks/acacia_leaves.png";
    static constexpr char const ACACIA_LOG_BLOCK_PATH[] = "textures/blocks/acacia_log.png";
    static constexpr char const ANDESITE_BLOCK_PATH[] = "textures/blocks/andesite.png";
    static constexpr char const BEDROCK_BLOCK_PATH[] = "textures/blocks/bedrock.png";
    static constexpr char const BIRCH_LEAVES_BLOCK_PATH[] = "textures/blocks/birch_leaves.png";
    static constexpr char const BIRCH_LOG_BLOCK_PATH[] = "textures/blocks/birch_log.png";
    static constexpr char const CACTUS_BLOCK_PATH[] = "textures/blocks/cactus.png";
    static constexpr char const COBBLESTONE_BLOCK_PATH[] = "textures/blocks/cobblestone.png";
    static constexpr char const DARK_OAK_LEAVES_BLOCK_PATH[] = "textures/blocks/dark_oak_leaves.png";
    static constexpr char const DARK_OAK_LOG_BLOCK_PATH[] = "textures/blocks/dark_oak_log.png";
    static constexpr char const DARKGRASS_BLOCK_BLOCK_PATH[] = "textures/blocks/darkgrass_block.png";
    static constexpr char const DIRT_BLOCK_PATH[] = "textures/blocks/dirt.png";
    static constexpr char const GLASS_BLOCK_PATH[] = "textures/blocks/glass.png";
    static constexpr char const GRASS_BLOCK_BLOCK_PATH[] = "textures/blocks/grass_block.png";
    static constexpr char const GRAVEL_BLOCK_PATH[] = "textures/blocks/gravel.png";
    static constexpr char const ICE_BLOCK_PATH[] = "textures/blocks/ice.png";
    static constexpr char const OAK_LEAVES_BLOCK_PATH[] = "textures/blocks/oak_leaves.png";
    static constexpr char const OAK_LOG_BLOCK_PATH[] = "textures/blocks/oak_log.png";
    static constexpr char const OAK_PLANKS_BLOCK_PATH[] = "textures/blocks/oak_planks.png";
    static constexpr char const SAND_BLOCK_PATH[] = "textures/blocks/sand.png";
    static constexpr char const SANDSTONE_BLOCK_PATH[] = "textures/blocks/sandstone.png";
    static constexpr char const SNOW_BLOCK_BLOCK_PATH[] = "textures/blocks/snow_block.png";
    static constexpr char const SPRUCE_LEAVES_BLOCK_PATH[] = "textures/blocks/spruce_leaves.png";
    static constexpr char const SPRUCE_LOG_BLOCK_PATH[] = "textures/blocks/spruce_log.png";
    static constexpr char const STONE_BLOCK_PATH[] = "textures/blocks/stone.png";
    static constexpr char const DEFAULT_TEXTURE_PATH[] = "textures/blocks/texture.jpg";

    static constexpr char const HUD_ARROW_LEFT_SELECTED[] = "textures/hud/arrow_left_selected.png";
    static constexpr char const HUD_ARROW_LEFT_UNSELECTED[] = "textures/hud/arrow_left_unselected.png";
    static constexpr char const HUD_ARROW_RIGHT_SELECTED[] = "textures/hud/arrow_right_selected.png";
    static constexpr char const HUD_ARROW_RIGHT_UNSELECTED[] = "textures/hud/arrow_right_unselected.png";
    static constexpr char const HUD_HOTBAR[] = "textures/hud/hotbar.png";
    static constexpr char const HUD_MENUE[] = "textures/hud/menue.png";
    static constexpr char const HUD_PAGE_SELECTED[] = "textures/hud/page_selected.png";
    static constexpr char const HUD_PAGE_UNSELECTED[] = "textures/hud/page_unselected.png";
    static constexpr char const HUD_BACKGROUND_SELECTED[] = "textures/hud/background_selected.png";
    static constexpr char const HUD_BACKGROUND_UNSELECTED_HOTBAR[] = "textures/hud/background_unselected_hotbar.png";
    static constexpr char const HUD_BACKGROUND_UNSELECTED_MENUE[] = "textures/hud/background_unselected_menue.png";
    static constexpr char const HUD_TRANSPARENT[] = "textures/hud/transparenz.png";

    static int const GUI_TEXTURE_COUNT = 38;

    static constexpr char const GRASS_AZURE_BLUET[] = "textures/grass/azure_bluet.png";
    static constexpr char const GRASS_CORNFLOWER[] = "textures/grass/cornflower.png";
    static constexpr char const GRASS_DANDELION[] = "textures/grass/dandelion.png";
    static constexpr char const GRASS_GRASS[] = "textures/grass/grass.png";
    static constexpr char const GRASS_LILAC_BOTTOM[] = "textures/grass/lilac_bottom.png";
    static constexpr char const GRASS_LILAC_TOP[] = "textures/grass/lilac_top.png";
    static constexpr char const GRASS_LILY_OF_THE_VALLEY[] = "textures/grass/lily_of_the_valley.png";
    static constexpr char const GRASS_ORANGE_TULIP[] = "textures/grass/orange_tulip.png";
    static constexpr char const GRASS_PINK_TULIP[] = "textures/grass/pink_tulip.png";
    static constexpr char const GRASS_POPPY[] = "textures/grass/poppy.png";
    static constexpr char const GRASS_RED_TULIP[] = "textures/grass/red_tulip.png";
    static constexpr char const GRASS_SUGAR_CANE[] = "textures/grass/sugar_cane.png";
    static constexpr char const GRASS_TALL_GRASS_BOTTOM[] = "textures/grass/tall_grass_bottom.png";
    static constexpr char const GRASS_TALL_GRASS_TOP[] = "textures/grass/tall_grass_top.png";
    static constexpr char const GRASS_WHEAT_STAGE7[] = "textures/grass/wheat_stage7.png";
    static constexpr char const GRASS_WHITE_TULIP[] = "textures/grass/white_tulip.png";

    static int const GRASS_TEXTURE_COUNT = 16;

    static constexpr char const SPHERE_OBJ_PATH[] = "obj/sphere.obj";
    static constexpr char const SUNFLOWER_OBJ_PATH[] = "obj/sunflower.obj";
    static constexpr char const LOTUS_OBJ_PATH[] = "obj/lotus.obj";
    static constexpr char const SUCCULENT_OBJ_PATH[] = "obj/succulent.obj";

    static constexpr char const SUNFLOWER_TEXTURE_PATH[] = "textures/obj/sunflower.png";
    static constexpr char const LOTUS_TEXTURE_PATH[] = "textures/obj/lotus.png";
    static constexpr char const SUCCULENT_TEXTURE_PATH[] = "textures/obj/succulent.png";

    static constexpr char const SKY_VERTEX_SHADER_PATH[] = "shaders/sky.vert.spv";
    static constexpr char const SKY_FRAGMENT_SHADER_PATH[] = "shaders/sky.frag.spv";

    static int const MAX_FRAMES_IN_FLIGHT = 2;

    static int const CHUNK_SIZE = 32;
    static int const LOADED_CHUNKS = 16;
    static int const MIN_HEIGHT = 1;
    static int const MAX_HEIGHT = 256;
    static int const WATER_LEVEL = 64;

    static float const BIOM_SIZE;
    static unsigned long const SEED = 0;
    static BiomData const plainsData;
    static BiomData const mountainData;
    static BiomData const desertData;
    static BiomData const forestData;

    static TreeDescription const simpleOakTreeDescription;
    static TreeDescription const simpleFirTreeDescription;
    static TreeDescription const simpleCactusDescription;

    static bool IN_PHOTO_MODE;
    static bool UPDATE_FRUSTUM;

    static SkyUBO skyUbo;

    //static constexpr int const worleyNoisePointCounts[4] = {25, 50, 100, 200};
    //static constexpr int const worleyNoisePointCounts[7] = {25, 50, 100, 200, 400, 800, 1600};
    static constexpr int const worleyNoisePointCounts[7] = {64, 1, 1, 1, 1, 1, 1};
    static constexpr int const worleyNoiseScaling[7] = {1, 1, 1, 1, 1, 1, 1};
    static int const cloudTextureSize = 128;
    static int const cloudTextureCount = 1;

private:
	Settings();
	~Settings();
};

#endif // !SETTINGS_H