#include "ComputeSettings.h"

int ComputeSettings::COMPUTE_WIDTH = 1024;
int ComputeSettings::COMPUTE_HEIGHT = 768;
glm::vec4 ComputeSettings::sensorData = glm::vec4(4.0f, 3.0f, 3.56f, 0.0f);

glm::ivec4 ComputeSettings::iData = glm::ivec4(0, 0, 1, 0);

int ComputeSettings::oldIntegrator = 0;

int ComputeSettings::oldScaling = 1;

int ComputeSettings::oldCamera = 0;

BenchmarkStatus ComputeSettings::benchmarkStatus = BenchmarkStatus::OFF;

uint64_t ComputeSettings::benchmarkMaxIterations = 256;
double ComputeSettings::benchmarkMaxTime = 10000;

WriteBackData ComputeSettings::writeBackData = { glm::ivec4(0) };