#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0, rgba8) uniform writeonly image2D writeColor;

layout(binding = 1) uniform sampler2D samplerColor;

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 outColor;

void main() {
	outColor = texture(samplerColor, inUV);

	imageStore(writeColor, ivec2(inUV * imageSize(writeColor)), outColor);
}
