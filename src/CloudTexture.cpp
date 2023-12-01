#include "CloudTexture.h"
#include "FileWriter.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>

#include <thread>

CloudTexture::CloudTexture(VkDevice const &device, ImageCreator const &imageCreator)
	: device(device) {
	if (!std::filesystem::exists("clouds")) {
		std::filesystem::create_directory("clouds");
		setUpNoise();
		createTextureFiles();
	}
	else {
		loadTextureFiles();
	}

	/*if (!std::filesystem::exists(std::to_string(Settings::SEED) + "/clouds")) {
		std::filesystem::create_directory(std::to_string(Settings::SEED) + "/clouds");
		setUpNoise();
		createTextureFiles();
	} else {
		loadTextureFiles();
	}*/

	setUpTexture();
	//createTexturePPM();
	createTextureImage(imageCreator);
}

CloudTexture::~CloudTexture() {
	vkDestroySampler(device, textureSampler, nullptr);
	vkDestroyImageView(device, textureImageView, nullptr);
	vkDestroyImage(device, textureImage, nullptr);
	vkFreeMemory(device, textureImageMemory, nullptr);
}

float remap(float oldValue, float oldMin, float oldMax, float newMin, float newMax) {
	return newMin + (((oldValue - oldMin) / (oldMax - oldMin)) * (newMax - newMin));
}

void CloudTexture::setUpNoise() {

	std::thread first([this] {parallelNoiseRange(0, 16); });
	std::thread second([this] {parallelNoiseRange(16, 32); });
	std::thread third([this] {parallelNoiseRange(32, 48); });
	std::thread fourth([this] {parallelNoiseRange(48, 64); });
	std::thread fifth([this] {parallelNoiseRange(64, 80); });
	std::thread sixth([this] {parallelNoiseRange(80, 96); });
	std::thread seventh([this] {parallelNoiseRange(96, 112); });
	std::thread eight([this] {parallelNoiseRange(112, 128); });

	/*std::thread first([this] {parallelNoiseRange(0, 32); });
	std::thread second([this] {parallelNoiseRange(32, 64); });
	std::thread third([this] {parallelNoiseRange(64, 96); });
	std::thread fourth([this] {parallelNoiseRange(96, 128); });
	std::thread fifth([this] {parallelNoiseRange(128, 160); });
	std::thread sixth([this] {parallelNoiseRange(160, 192); });
	std::thread seventh([this] {parallelNoiseRange(192, 224); });
	std::thread eight([this] {parallelNoiseRange(224, 256); });*/

	first.join();
	second.join();
	third.join();
	fourth.join();
	fifth.join();
	sixth.join();
	seventh.join();
	eight.join();
}

void CloudTexture::parallelNoiseRange(int start, int end) {
	float counter = 0.0f;

		for (int z = start; z < end; z++) {
			for (int y = 0; y < Settings::cloudTextureSize; y++) {
				for (int x = 0; x < Settings::cloudTextureSize; x++) {
					glm::vec3 position = glm::vec3(x, y, z) / glm::vec3(Settings::cloudTextureSize);

					int index = x + (y * Settings::cloudTextureSize) + (z * Settings::cloudTextureSize * Settings::cloudTextureSize);

					noiseValues[0][index] = noise.worley3D(position, 0);/*
					noiseValues[1][index] = noise.worley3D(position, 1);
					noiseValues[2][index] = noise.worley3D(position, 2);
					noiseValues[3][index] = noise.worley3D(position, 3);*/

					std::cout << counter / ((end - start) * Settings::cloudTextureSize * Settings::cloudTextureSize) << std::endl;
					counter++;
				}
			}
		}
}

float CloudTexture::readNoise(glm::vec3 const &position, int level, float frequency) {
	glm::vec3 scaled = position * frequency;
	
	int x = (int)scaled.x % Settings::cloudTextureSize;
	int y = (int)scaled.y % Settings::cloudTextureSize;
	int z = (int)scaled.z % Settings::cloudTextureSize;

	int index = x + (y * Settings::cloudTextureSize) + (z * Settings::cloudTextureSize * Settings::cloudTextureSize);

	return noiseValues[level][index];
}

void CloudTexture::setUpTexture() {
	for (int z = 0; z < Settings::cloudTextureSize; z++) {
		for (int y = 0; y < Settings::cloudTextureSize; y++) {
			for (int x = 0; x < Settings::cloudTextureSize; x++) {
				glm::vec3 position = glm::vec3(x, y, z);// / glm::vec3(Settings::cloudTextureSize);

				int index = x + (y * Settings::cloudTextureSize) + (z * Settings::cloudTextureSize * Settings::cloudTextureSize);

				//values[index].r = 1.0f - std::clamp(remap(noiseValues[0][index], 0.0f, 0.25f, 0.0f, 1.0f), 0.0f, 1.0f);
				//values[index].g = 1.0f - std::clamp(remap(noiseValues[1][index], 0.0f, 0.25f, 0.0f, 1.0f), 0.0f, 1.0f);
				//values[index].b = 1.0f - std::clamp(remap(noiseValues[2][index], 0.0f, 0.25f, 0.0f, 1.0f), 0.0f, 1.0f);
				//values[index].a = 1.0f - std::clamp(remap(noiseValues[3][index], 0.0f, 0.1f, 0.0f, 0.5f), 0.0f, 1.0f);
				
				/*values[index].r = remap(std::clamp(1.0f - 5.0f * noiseValues[0][index], 0.0f, 1.0f), 0.0f, 1.0f, 0.0f, 0.5f);
				values[index].g = remap(std::clamp(1.0f - 5.0f * noiseValues[1][index], 0.0f, 1.0f), 0.0f, 1.0f, 0.0f, 0.5f);
				values[index].b = remap(std::clamp(1.0f - 5.0f * noiseValues[2][index], 0.0f, 1.0f), 0.0f, 1.0f, 0.0f, 0.5f);
				values[index].a = remap(std::clamp(1.0f - 5.0f * noiseValues[3][index], 0.0f, 1.0f), 0.0f, 1.0f, 0.0f, 0.5f);*/

				/*values[index].r = remap(std::clamp(1.0f - 5.0f * readNoise(position, 3, 1.0f), 0.0f, 1.0f), 0.0f, 1.0f, 0.0f, 0.5f);
				values[index].g = remap(std::clamp(1.0f - 5.0f * readNoise(position, 3, 2.0f), 0.0f, 1.0f), 0.0f, 1.0f, 0.0f, 0.5f);
				values[index].b = remap(std::clamp(1.0f - 5.0f * readNoise(position, 3, 4.0f), 0.0f, 1.0f), 0.0f, 1.0f, 0.0f, 0.5f);
				values[index].a = remap(std::clamp(1.0f - 5.0f * readNoise(position, 3, 8.0f), 0.0f, 1.0f), 0.0f, 1.0f, 0.0f, 0.5f);*/
				
				/*values[index].r = std::clamp(1.0f - 5.0f * readNoise(position, 2, 2.0f), 0.0f, 1.0f);
				values[index].g = std::clamp(1.0f - 5.0f * readNoise(position, 2, 4.0f), 0.0f, 1.0f);
				values[index].b = std::clamp(1.0f - 5.0f * readNoise(position, 2, 8.0f), 0.0f, 1.0f);
				values[index].a = std::clamp(1.0f - 5.0f * readNoise(position, 2, 16.0f), 0.0f, 1.0f);*/

				values[index].r = std::clamp(1.0f - 5.0f * readNoise(position, 0, 2.0f), 0.0f, 1.0f);
				values[index].g = std::clamp(1.0f - 5.0f * readNoise(position, 0, 4.0f), 0.0f, 1.0f);
				values[index].b = std::clamp(1.0f - 5.0f * readNoise(position, 0, 8.0f), 0.0f, 1.0f);
				values[index].a = std::clamp(1.0f - 5.0f * readNoise(position, 0, 16.0f), 0.0f, 1.0f);

				values[index].r = values[index].r - (1.0f - values[index].g) * 0.25f;
				values[index].r = values[index].r - (1.0f - values[index].b) * 0.25f;
				values[index].r = values[index].r - (1.0f - values[index].a) * 0.25f;
				values[index].r = remap(std::clamp(values[index].r, 0.0f, 1.0f), 0.0f, 1.0f, 0.0f, 0.5f);
			}
		}
	}
}

void CloudTexture::createTextureImage(ImageCreator const &imageCreator) {
	imageCreator.createCloudTextureImage(values, Settings::cloudTextureSize, Settings::cloudTextureSize, Settings::cloudTextureSize, textureImage, textureImageMemory);
	imageCreator.createCloudTextureImageView(textureImage, textureImageView);
	imageCreator.createCloudTextureSampler(textureSampler);
}

void CloudTexture::createTexturePPM() {
	std::vector<float> outValuesR;
	std::vector<float> outValuesG;
	std::vector<float> outValuesB;
	std::vector<float> outValuesA;

	//for (size_t i = 0; i < Settings::cloudTextureSize * Settings::cloudTextureSize; i++) {
	for (size_t i = 0; i < Settings::cloudTextureSize * Settings::cloudTextureSize * Settings::cloudTextureSize; i++) {
		outValuesR.push_back(values[i].r);
		outValuesG.push_back(values[i].g);
		outValuesB.push_back(values[i].b);
		outValuesA.push_back(values[i].a);
	}

	//FileWriter::writeGreyScalePPM("cloud.ppm", Settings::cloudTextureSize, Settings::cloudTextureSize, outValues);
	FileWriter::writeGreyScalePPM("cloudR.ppm", Settings::cloudTextureSize, Settings::cloudTextureSize * Settings::cloudTextureSize, outValuesR);
	FileWriter::writeGreyScalePPM("cloudG.ppm", Settings::cloudTextureSize, Settings::cloudTextureSize * Settings::cloudTextureSize, outValuesG);
	FileWriter::writeGreyScalePPM("cloudB.ppm", Settings::cloudTextureSize, Settings::cloudTextureSize * Settings::cloudTextureSize, outValuesB);
	FileWriter::writeGreyScalePPM("cloudA.ppm", Settings::cloudTextureSize, Settings::cloudTextureSize * Settings::cloudTextureSize, outValuesA);
}

void CloudTexture::createTextureFiles() {
	for (size_t y = 0; y < 4; y++) {
		//FILE *file = fopen((std::to_string(Settings::SEED) + "/clouds" + "/" + std::to_string(y) + ".txt").c_str(), "wb");
		FILE *file = fopen(("clouds/" + std::to_string(y) + ".txt").c_str(), "wb");
		fwrite(noiseValues[y], sizeof(float), Settings::cloudTextureSize * Settings::cloudTextureSize * Settings::cloudTextureSize, file);
		fclose(file);
	}
}

void CloudTexture::loadTextureFiles() {
	for (size_t y = 0; y < 4; y++) {
		FILE *file = fopen(("clouds/" + std::to_string(y) + ".txt").c_str(), "rb");
		fread(noiseValues[y], sizeof(float), Settings::cloudTextureSize * Settings::cloudTextureSize * Settings::cloudTextureSize, file);
		fclose(file);
	}
}
