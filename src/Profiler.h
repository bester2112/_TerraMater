#ifndef PROFILER_H
#define PROFILER_H

#include "BenchmarkStatus.h"

#include "glm/glm.hpp"

#include <chrono>
#include <string>

class Profiler {
public:
	Profiler();
	~Profiler();

	void profilerSetUpData();

	void profilerCollectData();
	
	void benchmarkSetUpData(glm::vec3 &cameraPosition, glm::vec3 &cameraDirection, glm::vec3 &cameraUp, glm::ivec4 &iData);

	void benchmarkCollectData();

	BenchmarkStatus oldBenchmarkStatus = BenchmarkStatus::OFF;

private:

	uint64_t iteration = 0;

	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point stop;
	double elapsed = 0.0;


	glm::vec3 benchmarkCameraPosition;
	glm::vec3 benchmarkCameraDirection;
	glm::vec3 benchmarkCameraUp;

	glm::ivec4 benchmarkIData;

	uint64_t benchmarkIteration = 0;
	double benchmarkRunTime = 0.0;

	std::string dateString;

	void createBenchmarkFolders();
};

#endif // !PROFILER_H
