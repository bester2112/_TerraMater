#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int8 : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 sunDirection;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in uint8_t inNormalID;
layout(location = 3) in uint8_t inTextureID;
layout(location = 4) in uint8_t inAmbientOcclusionValue;
layout(location = 5) in uint8_t inLightLevel;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out uint8_t fragNormalID;
layout(location = 2) out uint8_t fragTextureID;
layout(location = 3) out float fragAmbientOcclusionValue;
layout(location = 4) out uint8_t fragLightLevel;

float ambientOcclusionValues[4] = {
    0.25f,
    0.5f,
    0.75f,
    1.0f
    };

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragTexCoord = inTexCoord;
    fragNormalID = inNormalID;
    fragTextureID = inTextureID;
    fragAmbientOcclusionValue = ambientOcclusionValues[int(inAmbientOcclusionValue)];
    fragLightLevel = inLightLevel;
}