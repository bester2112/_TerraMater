#include "RandomSampler.h"
#include "Settings.h"

#include <iostream>

thread_local std::mt19937_64 RandomSampler::merseneTwister = std::mt19937_64();
thread_local std::uniform_real_distribution<float> RandomSampler::distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);

RandomSampler::RandomSampler() {}

RandomSampler::~RandomSampler() {}

float RandomSampler::getSample1D(int32_t const x, int32_t const y) {
	merseneTwister.seed(Settings::SEED ^ (static_cast<uint64_t>(x) << 32 | static_cast<uint32_t>(y)));

	return distribution(merseneTwister);
}

void RandomSampler::getSample2D(int32_t const x, int32_t const y, float &r1, float &r2) {
	merseneTwister.seed(Settings::SEED ^ (static_cast<uint64_t>(x) << 32 | static_cast<uint32_t>(y)));

	r1 = distribution(merseneTwister);
	r2 = distribution(merseneTwister);
}

void RandomSampler::resetSampler(int32_t const x, int32_t const y) {
	merseneTwister.seed(Settings::SEED ^ (static_cast<uint64_t>(x) << 32 | static_cast<uint32_t>(y)));

	merseneTwister.discard(1);
}

float RandomSampler::getSample1DNonReset() {
	return distribution(merseneTwister);
}
