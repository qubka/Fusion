#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec3 inTexCoord;

layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform sampler2D textures[];

#define DIFFUSE int(inTexCoord.z)

float screenPxRange() {
    const float pxRange = 2.0; // set to distance field's pixel range
    vec2 unitRange = vec2(pxRange) / vec2(textureSize(textures[DIFFUSE], 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(vec2(inTexCoord));
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    vec4 fontAtlas = texture(textures[DIFFUSE], vec2(inTexCoord));

    float sd = median(fontAtlas.r, fontAtlas.g, fontAtlas.b);
    float screenPxDistance = screenPxRange() * (sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    if (opacity == 0.0)
        discard;

    vec4 bgColor = vec4(0.0);
    outColor = mix(bgColor, inColor, opacity);
    if (outColor.a == 0.0)
        discard;
}