#ifndef COMPUTESETTINGS_H
#define COMPUTESETTING_H

#include "BenchmarkStatus.h"
#include "WriteBackData.h"

#include "glm/glm.hpp"

#include <cstdint>

class ComputeSettings {
public:
	static int COMPUTE_WIDTH;
	static int COMPUTE_HEIGHT;

	static constexpr char const COMPUTE_VERTEX_SHADER_PATH[] = "shaders/compute.vert.spv";
	static constexpr char const COMPUTE_FRAGMENT_SHADER_PATH[] = "shaders/compute.frag.spv";
	static constexpr char const COMPUTE_SHADER_PATH[] = "shaders/compute.comp.spv";

	static uint32_t const groupSizeX = 32;
	static uint32_t const groupSizeY = 32;

	static glm::vec4 sensorData;

	static glm::ivec4 iData;

	static int oldIntegrator;
	static int const integratorCount = 11;

	static int oldScaling;

	static int oldCamera;
	static int const cameraCount = 3;

	static BenchmarkStatus benchmarkStatus;

	static int const benchmarkStartIntegrator = 5;
	static uint64_t benchmarkMaxIterations;
	static double benchmarkMaxTime;

	static WriteBackData writeBackData;

private:
	ComputeSettings();
	~ComputeSettings();
};

#endif // !COMPUTESETTINGS_H
