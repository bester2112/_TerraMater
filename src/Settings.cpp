#include "Settings.h"

Settings::~Settings() {}

int Settings::WINDOW_WIDTH = 1024;
int Settings::WINDOW_HEIGHT = 768;
float const Settings::BIOM_SIZE = 0.125f;

BiomData const Settings::plainsData = { 0.125f, 0.15f, 7.0f, 0.75f, 0.003125f, 1.0f, 0.00625f }; //Warm und feucht
BiomData const Settings::mountainData = { 0.125f, 1.0f, 6.0f, 1.0f, 0.0125f, 0.5f, 0.0125f }; //Kalt und trocken
BiomData const Settings::desertData = { 0.125f, 0.5f, 5.0f, 0.9f, 0.00625f, 0.25f, 0.003125f }; //Warm und trocken
BiomData const Settings::forestData = { 0.0625f, 0.1f, 4.0f, 0.85f, 0.04f, 0.25f, 0.0125f }; //Kalt und feucht

TreeDescription const Settings::simpleOakTreeDescription = { "A", {{'A', "TtaB"}, {'a', "ta"}, {'B', "T[+LL<L<L>L>l][-LL<L<L>L>l][*LL<L<L>L>l][/LL<L<L>L>l]bC"}, {'b', "T[+LL<L<L>L>l][-LL<L<L>L>l][*LL<L<L>L>l][/LL<L<L>L>l]bc"}, {'C', "T[+L][-L][*L][/L]D"}, {'c', "T[+L][-L][*L][/L]"}, {'D', "L"}}, 4, CubeType::OAK_LOG, CubeType::OAK_LEAVES, 0.5f, 0.5f, 0.5f };
TreeDescription const Settings::simpleFirTreeDescription = { "A", {{'A', "TtaB"}, {'a', "T[+L][-L][*L][/L]"}, {'B', "T[+LL<L<L>L>l][-LL<L<L>L>l][*LL<L<L>L>l][/LL<L<L>L>l]T[+L][-L][*L][/L]bC"}, {'b', "T[+LL<L<L>L>l][-LL<L<L>L>l][*LL<L<L>L>l][/LL<L<L>L>l]T[+L][-L][*L][/L]b"}, {'C', "TLd"}, {'d', "[+L][-L][*L][/L]e"}, {'e', "L"} }, 4, CubeType::SPRUCE_LOG, CubeType::SPRUCE_LEAVES, 0.5f, 0.5f, 0.5f };
TreeDescription const Settings::simpleCactusDescription = { "A", {{'A', "Ta"}, {'a', "Ta"}}, 3, CubeType::CACTUS, CubeType::CACTUS, 0.5f, 0.5f, 0.5f };

bool Settings::IN_PHOTO_MODE = false;
bool Settings::UPDATE_FRUSTUM = true;

SkyUBO Settings::skyUbo = {glm::vec4(0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)};