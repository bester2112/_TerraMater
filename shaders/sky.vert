#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int8 : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;
layout(binding = 1) uniform SkyUniformBufferObject {
    vec4 cameraPosition;
	vec4 lightDirection;
} skyUBO;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 fragDirection;

void main() {
    vec4 position = ubo.proj * mat4(mat3(ubo.view)) * vec4(inPosition, 1.0);
	gl_Position = position.xyww;
    fragDirection = inPosition;

	fragDirection.y += (skyUBO.cameraPosition.y - 64.0f) / 512.0f;
}