#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int8 : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 sunDirection;
} ubo;
layout(binding = 1) uniform sampler2D texSampler[32];

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) flat in int8_t fragNormal;
layout(location = 2) flat in int8_t fragTextureID;
layout(location = 3) in float fragAmbientOcclusionValue;
layout(location = 4) flat in int8_t fragLightLevel;

layout(location = 0) out vec4 outColor;

vec3 normals[6] = {
    vec3(1.0f, 0.0f, 0.0f),
    vec3(-1.0f, 0.0f, 0.0f),
    vec3(0.0f, 1.0f, 0.0f),
    vec3(0.0f, -1.0f, 0.0f),
    vec3(0.0f, 0.0f, 1.0f),
    vec3(0.0f, 0.0f, -1.0f)
    };

vec3 lightPosition = normalize(vec3(1.0f, 1.0f, 1.0f));

void main() {
    vec4 textureColor =  texture(texSampler[int(fragTextureID)], fragTexCoord);
    //float diffuse = max(dot(normals[int(fragNormal)], lightPosition), 0.0f);
    float diffuse = max(dot(normals[int(fragNormal)], normalize(ubo.sunDirection.xyz)), 0.0f);
   
    //float scalar = (fragAmbientOcclusionValue * diffuse) * (float(fragLightLevel) / 15.0f);
    float scalar = (fragAmbientOcclusionValue * diffuse) * (float(fragLightLevel) / 15.0f) * normalize(ubo.sunDirection.xyz).y;
    //if (scalar == 0.0f) {
    if (scalar <= 0.15 * fragAmbientOcclusionValue) {
        scalar = 0.15 * fragAmbientOcclusionValue;
    }

    outColor = vec4(textureColor.rgb * scalar, 1.0f);

    //UVCoordinate
    //outColor = vec4(fragTexCoord, 0.0f, 1.0f);

    //Texture
    //outColor = texture(texSampler[int(fragTextureID)], fragTexCoord);

    //Normals
    //outColor = vec4(abs(normals[int(fragNormal)]), 1.0f);

    //AmbientOcclusion
    //outColor = vec4(vec3(fragAmbientOcclusionValue), 1.0f);

    //LightLevel
    //outColor = vec4(textureColor.rgb * (float(fragLightLevel) * (1.0f / 15.f)), 1.0f);
    //outColor = vec4(vec3(float(fragLightLevel) * (1.0f / 15.f)), 1.0f);
}