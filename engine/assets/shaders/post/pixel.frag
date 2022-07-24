#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(push_constant) uniform PushScene {
	float pixelSize;
} scene;

layout(binding = 0, rgba8) uniform writeonly image2D writeColor;

layout(binding = 1) uniform sampler2D samplerColor;

layout(location = 0) in vec2 inUV;

void main() {
	vec2 sizeColor = textureSize(samplerColor, 0);

	float dx = scene.pixelSize * (1.0f / sizeColor.x);
	float dy = scene.pixelSize * (sizeColor.x / sizeColor.y) * (1.0f / sizeColor.y);
	vec2 coord = vec2(dx * floor(inUV.x / dx), dy * floor(inUV.y / dy));

	vec4 color = vec4(texture(samplerColor, coord).rgb, 1.0f);

	imageStore(writeColor, ivec2(inUV * imageSize(writeColor)), color);
}
