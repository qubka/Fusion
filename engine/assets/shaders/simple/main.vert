#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;
//layout (location = 3) in vec2 inUV;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec3 outPosition;
layout (location = 2) out vec3 outNormal;

layout (binding = 0) uniform UniformObject {
    mat4 projection;
    mat4 view;
} ubo;

layout (push_constant) uniform PushObject {
    mat4 model;
    mat4 normal;
} push;

void main() {
    vec4 worldPosition = push.model * vec4(inPosition, 1.0);
    gl_Position = ubo.projection * ubo.view * worldPosition;
    outNormal = mat3(push.normal) * inNormal;
    outPosition = worldPosition.xyz;
    outColor = inColor;
}