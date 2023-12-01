#include "PerlinNoise.h"
#include "Settings.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <iostream>

PerlinNoise::PerlinNoise() {}

PerlinNoise::~PerlinNoise() {}

float PerlinNoise::noise(float const s, float const t) {
	int s0 = (int)floor(s);
	int s1 = s0 + 1;
	int t0 = (int)floor(t);
	int t1 = t0 + 1;

	float weightS = s - s0;
	float weightT = t - t0;

	glm::vec2 gradient00 = getGradient(s0, t0);
	glm::vec2 gradient01 = getGradient(s0, t1);
	glm::vec2 gradient10 = getGradient(s1, t0);
	glm::vec2 gradient11 = getGradient(s1, t1);

	glm::vec2 direction00 = glm::vec2(s - s0, t - t0);
	glm::vec2 direction01 = glm::vec2(s - s0, t - t1);
	glm::vec2 direction10 = glm::vec2(s - s1, t - t0);
	glm::vec2 direction11 = glm::vec2(s - s1, t - t1);

	float dot00 = glm::dot(direction00, gradient00);
	float dot01 = glm::dot(direction01, gradient01);
	float dot10 = glm::dot(direction10, gradient10);
	float dot11 = glm::dot(direction11, gradient11);

	/*float interpolatedBottom = dot00 + weightS * (dot10 - dot00);
	float interpolatedTop = dot01 + weightS * (dot11 - dot01);
	float interpolatedCombined = interpolatedBottom + weightT * (interpolatedTop - interpolatedBottom);*/

	float interpolatedBottom = dot00 + smootherstep(0, 1, weightS) * (dot10 - dot00);
	float interpolatedTop = dot01 + smootherstep(0, 1, weightS) * (dot11 - dot01);
	float interpolatedCombined = interpolatedBottom + smootherstep(0, 1, weightT) * (interpolatedTop - interpolatedBottom);

	return interpolatedCombined;
}

float PerlinNoise::octave(float const s, float const t, float const frequency, float const amplitude) {
	return amplitude * noise(frequency * s, frequency * t);
}

float PerlinNoise::combinedOctaves(float const s, float const t, float const startFrequency, float const frequencyStep, float const startAmplitude, float const amplitudeStep, int const octaveCount) {
	float result = 0.0f;

	/*float frequency = 1.0f;
	float amplitude = 1.0f;*/
	//float frequency = 1.f / Settings::LOADED_CHUNKS;
	float frequency = startFrequency;
	float amplitude = startAmplitude;

	for (size_t i = 0; i < octaveCount; i++) {
		result += octave(s, t, frequency, amplitude);

		frequency *= frequencyStep;
		amplitude *= amplitudeStep;
	}

	return result;
}

float PerlinNoise::domainWarpingCombinedOctaves(float const s, float const t, float const startFrequency, float const frequencyStep, float const startAmplitude, float const amplitudeStep, int const octaveCount) {
	float warpedS = combinedOctaves(s, t, startFrequency, frequencyStep, startAmplitude, amplitudeStep, octaveCount);
	float warpedT = combinedOctaves(s + 32, t + 16, startFrequency, frequencyStep, startAmplitude, amplitudeStep, octaveCount);

	return combinedOctaves(s + 4.0f * warpedS, t + 4.0f * warpedT, startFrequency, frequencyStep, startAmplitude, amplitudeStep, octaveCount);
}

float PerlinNoise::domainWarping(float const s, float const t, float const startFrequency, float const startAmplitude) {
	float warpedS = combinedOctaves(s, t, startFrequency, 2.0f, startAmplitude, 1.0f, 1);
	float warpedT = combinedOctaves(s + 32, t + 16, startFrequency, 2.0f, startAmplitude, 1.0f, 1);

	float warpedS2 = combinedOctaves(s + 4.0f * warpedS + 23, t + 4.0f * warpedT + 36, startFrequency, 2.0f, startAmplitude, 1.0f, 1);
	float warpedT2 = combinedOctaves(s + 4.0f * warpedS + 12, t + 4.0f * warpedT + 44, startFrequency, 2.0f, startAmplitude, 1.0f, 1);

	return combinedOctaves(s + 4.0f * warpedS2, t + 4.0f * warpedT2, startFrequency, 2.0f, startAmplitude, 1.0f, 1);
}

float PerlinNoise::invert(float const input) {
	return -input;
}

float PerlinNoise::redistribute(float const input, float const alpha) {
	return input * input * input * alpha;
}

glm::vec2  PerlinNoise::getGradient(int32_t const x, int32_t const z) {
	float r1, r2;
	randomSampler.getSample2D(x, z, r1, r2);

	float r = sqrtf(r1);
	float theta = 2.0f * M_PI * r2;

	return  glm::normalize(glm::vec2(r * cosf(theta), r * sin(theta)));
}

float PerlinNoise::smootherstep(float const a, float const b, float const value) {
	float valueClamped = std::clamp((value - a) / (b - a), 0.0f, 1.0f);

	return valueClamped * valueClamped * valueClamped * (valueClamped * (valueClamped * 6.0f - 15.0f) + 10.0f);
}
