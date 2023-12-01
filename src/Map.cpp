#include "Map.h"
#include "Chunk.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>

Map::Map() {
	//mapGenerator.perlinNoiseImage();
	if (!std::filesystem::exists(std::to_string(Settings::SEED))) {
		std::filesystem::create_directory(std::to_string(Settings::SEED));
	}

	/*for (auto const &subfolder : std::filesystem::directory_iterator(std::to_string(Settings::SEED))) {
		std::string folderName = subfolder.path().filename().string();
		std::stringstream stream(folderName);
		std::string xString;
		std::string zString;

		std::getline(stream, xString, '_');
		std::getline(stream, zString, '_');

		int x = std::stoi(xString);
		int z = std::stoi(zString);

		ChunkStack stack;
		readChunkStackFromDisk({ x, z }, stack);
		stack.coordinates = { x, z };
		saveChunkStack(stack);


		std::cout << folderName << " " << xString << " " << zString << std::endl;

	}*/
}

Map::~Map() {
	for (auto iterator = map.begin(); iterator != map.end(); iterator++) {
		writeChunkStackToDisk(iterator->second);
	}
}

void Map::loadChunkStack(Coordinates const &coordinates, ChunkStack &chunkstack) {
		auto iterator = map.find(coordinates);

		//chunkstack = ChunkStack();

		if (iterator != map.end()) {
			chunkstack = ChunkStack(iterator->second);
			//chunkstack = iterator->second;
		}
		else if (existsOnDisk(coordinates)) {
			readChunkStackFromDisk(coordinates, chunkstack);
			chunkstack.coordinates = { coordinates.x, coordinates.z };
			saveChunkStack(chunkstack);
		}
		else {
			mapGenerator.generateChunkHeight(coordinates.x, coordinates.z, chunkstack);
			chunkstack.coordinates = { coordinates.x, coordinates.z };
			chunkstack.changed = true;
			saveChunkStack(chunkstack);
		}
}

void Map::saveChunkStack(ChunkStack &chunkStack) {
		auto iterator = map.find(chunkStack.coordinates);

		if (iterator != map.end()) {
			iterator->second = ChunkStack(chunkStack);
			//iterator->second = chunkStack;
		}
		else {
			map.insert(std::pair<Coordinates, ChunkStack>(chunkStack.coordinates, chunkStack));
		}
}

bool Map::existsOnDisk(Coordinates const &coordinates) {
		return std::filesystem::exists(std::to_string(Settings::SEED) + "/" + std::to_string(coordinates.x) + "_" + std::to_string(coordinates.z));
}

void Map::readChunkStackFromDisk(Coordinates const &coordinates, ChunkStack &chunkstack) {
		size_t size = std::distance(std::filesystem::directory_iterator(std::to_string(Settings::SEED) + "/" + std::to_string(coordinates.x) + "_" + std::to_string(coordinates.z)), std::filesystem::directory_iterator{});

		chunkstack.stack.resize(size);

		for (size_t y = 0; y < size; y++) {
			FILE *file = fopen((std::to_string(Settings::SEED) + "/" + std::to_string(coordinates.x) + "_" + std::to_string(coordinates.z) + "/" + std::to_string(y) + ".txt").c_str(), "rb");
			fread(chunkstack.stack[y].cubes, sizeof(Cube), sizeof(chunkstack.stack[y].cubes), file);
			fclose(file);
		}

		for (size_t y = 0; y < size; y++) {
			FILE *file = fopen((std::to_string(Settings::SEED) + "/" + std::to_string(coordinates.x) + "_" + std::to_string(coordinates.z) + "_obj" + "/" + std::to_string(y) + ".txt").c_str(), "rb");
			fread(chunkstack.stack[y].objData, sizeof(ObjData), Settings::CHUNK_SIZE * Settings::CHUNK_SIZE * Settings::CHUNK_SIZE, file);
			fclose(file);
		}
}

void Map::writeChunkStackToDisk(ChunkStack &chunkStack) {
	if (chunkStack.changed) {

		if (!std::filesystem::exists(std::to_string(Settings::SEED) + "/" + std::to_string(chunkStack.coordinates.x) + "_" + std::to_string(chunkStack.coordinates.z))) {
			std::filesystem::create_directory(std::to_string(Settings::SEED) + "/" + std::to_string(chunkStack.coordinates.x) + "_" + std::to_string(chunkStack.coordinates.z));
		}

		for (size_t y = 0; y < chunkStack.stack.size(); y++) {
			FILE *file = fopen((std::to_string(Settings::SEED) + "/" + std::to_string(chunkStack.coordinates.x) + "_" + std::to_string(chunkStack.coordinates.z) + "/" + std::to_string(y) + ".txt").c_str(), "wb");
			fwrite(chunkStack.stack[y].cubes, sizeof(Cube), sizeof(chunkStack.stack[y].cubes), file);
			fclose(file);
		}

		if (!std::filesystem::exists(std::to_string(Settings::SEED) + "/" + std::to_string(chunkStack.coordinates.x) + "_" + std::to_string(chunkStack.coordinates.z) + "_obj")) {
			std::filesystem::create_directory(std::to_string(Settings::SEED) + "/" + std::to_string(chunkStack.coordinates.x) + "_" + std::to_string(chunkStack.coordinates.z) + "_obj");
		}

		for (size_t y = 0; y < chunkStack.stack.size(); y++) {
			FILE *file = fopen((std::to_string(Settings::SEED) + "/" + std::to_string(chunkStack.coordinates.x) + "_" + std::to_string(chunkStack.coordinates.z) + "_obj" + "/" + std::to_string(y) + ".txt").c_str(), "wb");
			fwrite(chunkStack.stack[y].objData, sizeof(ObjData), Settings::CHUNK_SIZE * Settings::CHUNK_SIZE * Settings::CHUNK_SIZE, file);
			fclose(file);
		}
	}
}