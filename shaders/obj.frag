#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int8 : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 sunDirection;
} ubo;
layout(binding = 1) uniform sampler2D texSampler[4];

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) flat in int8_t fragTextureID;

layout(location = 0) out vec4 outColor;

vec3 lightPosition = normalize(vec3(1.0f, 1.0f, 1.0f));

void main() {
    vec4 textureColor =  texture(texSampler[int(fragTextureID)], fragTexCoord);
    //float diffuse = max(dot(fragNormal, lightPosition), 0.0f);
    float diffuse = max(dot(fragNormal, normalize(ubo.sunDirection.xyz)), 0.0f);

   //float scalar = diffuse + 0.25;
   float scalar = (0.25 + diffuse) * normalize(ubo.sunDirection.xyz).y;
   if (scalar <= 0.15) {
        scalar = 0.15;
    }

    outColor = vec4(textureColor.rgb * scalar, 1.0f);

    //UVCoordinate
    //outColor = vec4(fragTexCoord, 0.0f, 1.0f);

    //Texture
    //outColor = texture(texSampler[int(fragTextureID)], fragTexCoord);

    //Normals
    //outColor = vec4(abs(fragNormal), 1.0f);
}