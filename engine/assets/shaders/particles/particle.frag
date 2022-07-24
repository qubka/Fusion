#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 1) uniform sampler2D samplerColor;

layout(location = 0) in vec2 inCoords1;
layout(location = 1) in vec2 inCoords2;
layout(location = 2) in vec4 inColorOffset;
layout(location = 3) in float inBlendFactor;
layout(location = 4) in float inTransparency;

layout(location = 0) out vec4 outColor;

void main() {
	vec4 color1 = texture(samplerColor, inCoords1);
	vec4 color2 = texture(samplerColor, inCoords2);

	outColor = mix(color1, color2, inBlendFactor);
	outColor *= inColorOffset;
	outColor.a *= inTransparency;
}
