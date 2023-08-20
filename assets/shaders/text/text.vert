#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec3 inTexCoord;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec3 outTexCoord;

layout (push_constant) uniform PushObject {
    mat4 projection;
    mat4 view;
} push;

void main() {
    outColor = inColor;
    outTexCoord = inTexCoord;

    gl_Position = push.projection * push.view * vec4(inPosition, 1.0);
}
