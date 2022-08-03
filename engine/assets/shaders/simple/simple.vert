#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec3 inBitangent;
layout (location = 4) in vec2 inUV0;
layout (location = 5) in vec2 inUV1;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outTangent;
layout (location = 3) out vec3 outBitangent;
layout (location = 4) out vec2 outUV0;
layout (location = 5) out vec2 outUV1;

layout (binding = 0) uniform UniformObject {
    mat4 projection;
    mat4 view;
    vec3 cameraPos;
    uint lightsCount;
} ubo;

layout (push_constant) uniform PushObject {
    mat4 model;
    mat4 normal;
} push;

void main() {
    vec4 worldPosition = push.model * vec4(inPosition, 1.0);
    gl_Position = ubo.projection * ubo.view * worldPosition;
    outPosition = worldPosition.xyz;
    outNormal = normalize(mat3(push.normal) * inNormal);
    outTangent = normalize(mat3(push.normal) * inTangent);
    outBitangent = normalize(mat3(push.normal) * inBitangent);
    outUV0 = inUV0;
    outUV1 = inUV1;
}