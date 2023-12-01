#ifndef RANDOMSAMPLER_H
#define RANDOMSAMPLER_H

#include <random>

class RandomSampler {
public:
	RandomSampler();
	~RandomSampler();

	float getSample1D(int32_t const x, int32_t const y);
	void getSample2D(int32_t const x, int32_t const y, float &r1, float &r2);

	void resetSampler(int32_t const x, int32_t const y);
	float getSample1DNonReset();

private:
	static thread_local std::mt19937_64 merseneTwister;
	static thread_local std::uniform_real_distribution<float> distribution;
};

#endif // !RANDOMSAMPLER_H