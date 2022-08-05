#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec4 inColor;
layout (location = 1) in float inRadius;

layout (location = 0) out vec4 outColor;

void main() {
    if (length(gl_PointCoord - vec2(0.5)) > inRadius) {
        discard;
    }
    outColor = inColor;
}