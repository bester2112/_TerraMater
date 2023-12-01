#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int8 : enable

#define M_PI 3.1415926538

layout(binding = 1) uniform SkyUniformBufferObject {
    vec4 cameraPosition;
	vec4 lightDirection;
	float time;
} skyUBO;
layout(binding = 2) uniform sampler3D cloudTexture;

layout(location = 0) in vec3 fragDirection;

layout(location = 0) out vec4 outColor;

float earthRadius = 6360e3f;
float atmosSphereRadius = 6420e3f;
float g = 0.76f;
float scaleHeightRayleigh = 7994.0f;
float scaleHeightMie = 1200.0f;
vec3 betaRayleigh = vec3(3.8e-6f, 13.5e-6f, 33.1e-6f);
vec3 betaMie = vec3(21e-6f);

int numSamples = 16;
int numSamplesLight = 8;

bool intersect(vec3 origin, vec3 direction, float radius, inout float t) {
	vec3 oc = origin - vec3(0.0f);
	float ocLength = length(oc);
	float doc = dot(direction, oc);
	float discriminant = doc * doc - (ocLength * ocLength - radius * radius);

	if (discriminant < 0.0f) {
		return false;
	}

	float squareRoot = sqrt(discriminant);

	float x1 = -doc - squareRoot;
	float x2 = -doc + squareRoot;

	if (x1 < 0.0f && x2 < 0.0f) {
		return false;
	}

	if (x1 < 0.0f) {
		t = x2;
		return true;
	}
	if (x2 < 0.0f) {
		t = x1;
		return true;
	}

	t = x1 < x2 ? x1 : x2;
	return true;
}

//For the li method I followed the tutorial from: https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/simulating-sky
//But I've cut some parts of the described algorithm, because those cases don't happen in my shader anyways, because the shader only get invoked for the normal cases.
//Furthermore I've changed the intersection testing to use the intersection method from my hobby pathtracer, because this should be faster, and easier since here I'm only calculating t and not tmin and tmax like it is done on scratchapixel,
//since those cases can't happen in this shader either.

vec3 atmosphericScattering() {
	vec3 origin = skyUBO.cameraPosition.xyz;
	origin.y += earthRadius;
	vec3 sunDirection = skyUBO.lightDirection.xyz;
	vec3 direction = normalize(fragDirection);

	float t = 0.0f;
	if (!intersect(origin, direction, atmosSphereRadius, t)) {
		return vec3(0.0f);
	}

	float segmentLength = t / numSamples;
	float tCurrent = 0.0f;

	vec3 liRayleigh = vec3(0.0f);
	vec3 liMie = vec3(0.0f);

	float opticalDepthRayleigh = 0.0f;
	float opticalDephtMie = 0.0f;

	float mu = dot(direction, sunDirection);
	float phaseRayleigh = 3.0f / (16.0f  * M_PI) * (1.0f + mu * mu);
	float phaseMie = 3.0f / (8.0f * M_PI) * ((1.0f - g * g) * (1.0f + mu * mu)) / ((2.0f + g * g) * pow(1.0f + g * g - 2.0f * g * mu, 1.5f));

	for (int i = 0; i < numSamples; i++) {
		vec3 samplePoint = origin + (tCurrent + segmentLength * 0.5f) * direction;

		float height = length(samplePoint) - earthRadius;

		float eRayleigh = exp(-height / scaleHeightRayleigh) * segmentLength;
		float eMie = exp(-height / scaleHeightMie) * segmentLength;

		opticalDepthRayleigh += eRayleigh;
		opticalDephtMie += eMie;

		float tLight = 0.0f;
		intersect(samplePoint, sunDirection, atmosSphereRadius, tLight);

		float segmentLengthLight = tLight / numSamplesLight;
		float tCurrentLight = 0.0f;
		
		float opticalDepthLightRayleigh = 0.0f;
		float opticalDepthLightMie = 0.0f;

		int j = 0;
		for (; j < numSamplesLight; j++) {
			vec3 samplePointLight = samplePoint + (tCurrentLight + segmentLengthLight * 0.5f) * sunDirection;

			float heightLight = length(samplePointLight) - earthRadius;
			
			opticalDepthLightRayleigh += exp(-heightLight / scaleHeightRayleigh) * segmentLengthLight;
			opticalDepthLightMie += exp(-heightLight / scaleHeightMie) * segmentLengthLight;

			tCurrentLight += segmentLengthLight;
		}

		vec3 tau = betaRayleigh * (opticalDepthRayleigh + opticalDepthLightRayleigh) + betaMie * 1.1f * (opticalDephtMie + opticalDepthLightMie);
		vec3 attenuation = exp(-tau);

		liRayleigh += attenuation * eRayleigh;
		liMie += attenuation * eMie;

		tCurrent += segmentLength;
	}

	return (liRayleigh * betaRayleigh * phaseRayleigh + liMie * betaMie * phaseMie) * 20.0f;
}

//For the volumetric clouds I followed Andrew Schneiders Chapter "Real-Time Volumetric Cloudscapes" from the Book "GPU Pro 7: Advances Rendering Techniques",
//where he describes how he implemented the volumetric clouds in the Playstation 4 title "Horizon: Zero Dawn".
//But I need to add, that this isn't a exact implementation like described in the book, because I changed various parameters and methods, for example my shadowRay calculation is a simple second ray marching step and not using cone sampling.
//Furthermore I've implemented the normal ray marching differently than the approach from the book, since I've changed the noise function to create more block like clouds, it made more sense doing it my way.

int cloudSampleCount = 64;
int cloudShadowSampleCount = 6;
vec2 cloudMinMax = vec2(1500.0f, 1750.0f);
float cloudG = 0.2f;

float getHeightFractionForPoint(vec3 inPoint, vec2 cloudMinMax) {
	float heightFraction = (inPoint.y - cloudMinMax.x) / (cloudMinMax.y - cloudMinMax.x);

	return clamp(heightFraction, 0.0f, 1.0f);
}

float remap(float oldValue, float oldMin, float oldMax, float newMin, float newMax) {
	return newMin + (((oldValue - oldMin) / (oldMax - oldMin)) * (newMax - newMin));
}

float sampleCloudDensity(vec3 point) {
	vec4 noise = texture(cloudTexture, point * 0.00005f + vec3(skyUBO.time) * 0.0000005f);
	//vec4 noise = texture(cloudTexture, point * 0.0001f);
	//vec4 noise = texture(cloudTexture, vec3(point.x * 0.0001f, getHeightFractionForPoint(point, cloudMinMax), point.z * 0.0001f));

	float fbmValue = noise.g * 0.625f + noise.b * 0.25f + noise.a * 0.125f;

	float baseCloud = remap(noise.r, -(1.0f - fbmValue), 1.0f, 0.0f, 1.0f);
	baseCloud = noise.r;

	return baseCloud;
}

float henyeyGreenstein(vec3 lightDirection, vec3 viewDirection, float g) {
	float cosi = dot(normalize(lightDirection), normalize(viewDirection));

	return ((1.0f - g * g) / pow((1.0f + g * g - 2.0f * g * cosi), 1.5f)) / 4.0f * M_PI;
}

float beerPowder(float opticalDepth) {
	float powder = 1.0f - exp(-opticalDepth * 2.0f);
	float beer = exp(-opticalDepth);

	return 2.0f * beer * powder;
}

float shadowRay(vec3 position, vec3 lightDirection) {
	float cloudMaxRadius = earthRadius + cloudMinMax.y;
	float t = 0.0f;

	intersect(position, lightDirection, cloudMaxRadius, t);

	vec3 endPosition = position + lightDirection * t;
	
	float stepLength = length(endPosition - position) / cloudShadowSampleCount;
	stepLength = (cloudMinMax.y - cloudMinMax.x) / cloudSampleCount;
	vec3 stepVec = lightDirection * stepLength;

	vec3 shadowPosition = position;

	float transmittance = 1.0f;

	for (int i = 0; i < cloudShadowSampleCount; i++, shadowPosition += stepVec) {
		float opticalDepth = sampleCloudDensity(shadowPosition) / 50;

		if (opticalDepth <= 0.0f) {
			continue;
		}

		transmittance *= exp(-opticalDepth * stepLength);
	}

	return transmittance;
}

vec3 cloudScattering(vec3 position, vec3 direction, vec3 lightDirection, float opticalDepth, vec3 inColor) {
	float beerPowderValue = beerPowder(opticalDepth);
	float hg = henyeyGreenstein(lightDirection, direction, cloudG);

	float sunLight = shadowRay(position, lightDirection);

	return vec3(beerPowderValue * hg * sunLight);
}

vec3 cloudLi(vec3 inColor) {
	vec3 origin = skyUBO.cameraPosition.xyz;
	origin.y += earthRadius;
	vec3 sunDirection = skyUBO.lightDirection.xyz;
	vec3 direction = normalize(fragDirection);

	float cloudMinRadius = earthRadius + cloudMinMax.x;
	float cloudMaxRadius = earthRadius + cloudMinMax.y;

	float tCloudMin = 0.0f;
	float tCloudMax = 0.0f;

	if (!intersect(origin, direction, cloudMinRadius, tCloudMin)) {
		return vec3(1.0f, 0.0f, 0.0f);
	}

	if (!intersect(origin, direction, cloudMaxRadius, tCloudMax)) {
		return vec3(0.0f, 1.0f, 0.0f);
	}

	vec3 startPoint = origin + direction * tCloudMin;
	vec3 endPosition = origin + direction * tCloudMax;

	float stepLength = length(endPosition - startPoint) / cloudSampleCount;
	vec3 stepVec = direction * stepLength;

	vec3 position = startPoint;
	
	vec3 li = vec3(0.0f);
	float transmittance = 1.0f;

	for (int i = 0; i < cloudSampleCount; i++, position += stepVec) {
		float opticalDepth = sampleCloudDensity(position);

		if (opticalDepth <= 0.0f) {
			continue;
		}

		li += cloudScattering(position, direction, sunDirection, opticalDepth, inColor) * transmittance;
		transmittance *= exp(-opticalDepth * stepLength);
	}
	
	return inColor * transmittance + li;
}

void main() {
	outColor = vec4(atmosphericScattering(), 1.0f);

	if (length(outColor.xyz) == 0.0f) {
		outColor.xyz = vec3(0.001f, 0.001f, 0.005f);
	}
	
	outColor = vec4(cloudLi(outColor.xyz), 1.0f);

	if (normalize(skyUBO.lightDirection.xyz).y < 0.0f) {
		float weight = 1.0f + normalize(skyUBO.lightDirection.xyz).y;
		weight = weight * weight * weight * weight * weight;
		if (weight >= 0.01f) {
			outColor.xyz *= weight;
		} else {
			outColor.xyz *= 0.01f;
		}
	}
}