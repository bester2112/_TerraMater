#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include "RandomSampler.h"

#include "glm/glm.hpp"

#include <cstdint>

class PerlinNoise {
public:
	PerlinNoise();
	~PerlinNoise();

	float noise(float const s, float const t);
	float octave(float const s, float const t, float const frequency, float const amplitude);
	float combinedOctaves(float const s, float const t, float const startFrequency, float const frequencyStep, float const startAmplitude, float const amplitudeStep, int const octaveCount);
	float domainWarpingCombinedOctaves(float const s, float const t, float const startFrequency, float const frequencyStep, float const startAmplitude, float const amplitudeStep, int const octaveCount);
	float domainWarping(float const s, float const t, float const startFrequency, float const startAmplitude);
	float invert(float const input);
	float redistribute(float const input, float const alpha);

private:
	RandomSampler randomSampler;

	glm::vec2 getGradient(int32_t const x, int32_t const z);
	float smootherstep(float const a, float const b, float const value);
};

#endif // !PERLINNOISE_H