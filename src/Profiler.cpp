#include "Profiler.h"
#include "ComputeSettings.h"

#include <iostream>
#include <filesystem>
#include <iomanip>
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>

Profiler::Profiler() {
	if (!std::filesystem::exists("Benchmarks")) {
		std::filesystem::create_directory("Benchmarks");
	}
}

Profiler::~Profiler() {}

void Profiler::profilerSetUpData() {
	start = std::chrono::high_resolution_clock::now();
}

void Profiler::profilerCollectData() {
	stop = std::chrono::high_resolution_clock::now();

	elapsed = std::chrono::duration<double, std::chrono::milliseconds::period>(stop - start).count();

	if (oldBenchmarkStatus == BenchmarkStatus::OFF) {
		std::cout << std::fixed << std::setprecision(2) << "Iteration: " << iteration << "\t" << elapsed << "ms elapsed" << "\t" << 1000.0 / elapsed << "fps" << "\t" << ComputeSettings::writeBackData.data.x << "Rays" << "\t" << (double)ComputeSettings::writeBackData.data.x / 1000 / 1000 / 1000 / (elapsed / 1000)  << "GRays/sec" << std::endl;
	}

	iteration++;
}

void Profiler::benchmarkSetUpData(glm::vec3 &cameraPosition, glm::vec3 &cameraDirection, glm::vec3 &cameraUp, glm::ivec4 &iData) {
	if (oldBenchmarkStatus == BenchmarkStatus::OFF && ComputeSettings::benchmarkStatus != BenchmarkStatus::OFF) {
		benchmarkCameraPosition = glm::vec3(cameraPosition);
		benchmarkCameraDirection = glm::vec3(cameraDirection);
		benchmarkCameraUp = glm::vec3(cameraUp);

		benchmarkIData = glm::ivec4(iData);
		benchmarkIData.x = ComputeSettings::benchmarkStartIntegrator;

		oldBenchmarkStatus = ComputeSettings::benchmarkStatus;

		benchmarkIteration = 0;
		benchmarkRunTime = 0.0;

		createBenchmarkFolders();
	} else {
		cameraPosition = benchmarkCameraPosition;
		cameraDirection = benchmarkCameraDirection;
		cameraUp = benchmarkCameraUp;

		iData = benchmarkIData;

		benchmarkIteration++;
	}
}

void Profiler::benchmarkCollectData() {
	benchmarkRunTime += elapsed;

	if (oldBenchmarkStatus == BenchmarkStatus::TIME) {
		std::cout << std::fixed << std::setprecision(2) << "Iteration: " << benchmarkIteration << "\t" << benchmarkRunTime / 1000 << "/" << ComputeSettings::benchmarkMaxTime / 1000 << "s running" << "\t" << elapsed << "ms elapsed" << "\t" << 1000.0 / elapsed << "fps" << "\t" << ComputeSettings::writeBackData.data.x << "Rays" << "\t" << (double)ComputeSettings::writeBackData.data.x / 1000 / 1000 / 1000 / (elapsed / 1000) << "GRays/sec" << std::endl;

		std::ofstream file("Benchmarks/" + dateString + "/" + std::to_string(benchmarkIData.x) + "/profiling.txt", std::ios::out | std::ios::app);
		if (file.is_open()) {
			file << std::fixed << std::setprecision(2) << "Iteration: " << benchmarkIteration << "\t" << benchmarkRunTime / 1000 << "/" << ComputeSettings::benchmarkMaxTime / 1000 << "s running" << "\t" << elapsed << "ms elapsed" << "\t" << 1000.0 / elapsed << "fps" << "\t" << ComputeSettings::writeBackData.data.x << "Rays" << "\t" << (double)ComputeSettings::writeBackData.data.x / 1000 / 1000 / 1000 / (elapsed / 1000) << "GRays/sec" << std::endl;
			file.close();
		}

		if (benchmarkRunTime >= ComputeSettings::benchmarkMaxTime) {
			benchmarkIteration = 0;
			benchmarkRunTime = 0.0;

			benchmarkIData.x += 1;

			if (benchmarkIData.x >= ComputeSettings::integratorCount) {
				oldBenchmarkStatus = BenchmarkStatus::OFF;
				ComputeSettings::benchmarkStatus = BenchmarkStatus::OFF;
			}
		}	
	} else if (oldBenchmarkStatus == BenchmarkStatus::ITERATION) {
		std::cout << std::fixed << std::setprecision(2) << "Iteration: " << benchmarkIteration << "/" << ComputeSettings::benchmarkMaxIterations << "\t" << elapsed << "ms elapsed" << "\t" << 1000.0 / elapsed << "fps" << "\t" << ComputeSettings::writeBackData.data.x << "Rays" << "\t" << (double)ComputeSettings::writeBackData.data.x / 1000 / 1000 / 1000 / (elapsed / 1000) << "GRays/sec" << std::endl;

		std::ofstream file("Benchmarks/" + dateString + "/" + std::to_string(benchmarkIData.x) + "/profiling.txt", std::ios::out | std::ios::app);
		if (file.is_open()) {
			file << std::fixed << std::setprecision(2) << "Iteration: " << benchmarkIteration << "/" << ComputeSettings::benchmarkMaxIterations << "\t" << elapsed << "ms elapsed" << "\t" << 1000.0 / elapsed << "fps" << "\t" << ComputeSettings::writeBackData.data.x << "Rays" << "\t" << (double)ComputeSettings::writeBackData.data.x / 1000 / 1000 / 1000 / (elapsed / 1000) << "GRays/sec" << std::endl;
			file.close();
		}

		if (benchmarkIteration >= ComputeSettings::benchmarkMaxIterations) {
			benchmarkIteration = 0;
			benchmarkRunTime = 0.0;

			benchmarkIData.x += 1;

			if (benchmarkIData.x >= ComputeSettings::integratorCount) {
				oldBenchmarkStatus = BenchmarkStatus::OFF;
				ComputeSettings::benchmarkStatus = BenchmarkStatus::OFF;
			}
		}
	}
}

void Profiler::createBenchmarkFolders() {
	auto time = std::time(nullptr);
	auto localTime = *std::localtime(&time);
	std::ostringstream ostringstream;
	ostringstream << std::put_time(&localTime, "%d-%m-%Y %H-%M-%S");
	dateString = ostringstream.str();

	std::filesystem::create_directory("Benchmarks/" + dateString);

	for (int i = ComputeSettings::benchmarkStartIntegrator; i < ComputeSettings::integratorCount; i++) {
		std::filesystem::create_directory("Benchmarks/" + dateString + "/" + std::to_string(i));
	}
}
