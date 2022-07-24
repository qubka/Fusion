#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(push_constant) uniform PushScene {
	vec2 direction;
} scene;

layout(binding = 0, rgba8) uniform writeonly image2D writeColor;

layout(binding = 1) uniform sampler2D samplerColor;

layout(location = 0) in vec2 inUV;

void main() {
	vec2 sizeColor = textureSize(samplerColor, 0);

	vec4 color = vec4(0.0f);
#if BLUR_TYPE == 5
	vec2 off1 = 1.3333333333333333f * scene.direction;
	color += texture(samplerColor, inUV) * 0.29411764705882354;
	color += texture(samplerColor, inUV + (off1 / sizeColor)) * 0.35294117647058826f;
	color += texture(samplerColor, inUV - (off1 / sizeColor)) * 0.35294117647058826f;
#elif BLUR_TYPE == 9
	vec2 off1 = 1.3846153846f * scene.direction;
	vec2 off2 = 3.2307692308f * scene.direction;
	color += texture(samplerColor, inUV) * 0.2270270270f;
	color += texture(samplerColor, inUV + (off1 / sizeColor)) * 0.3162162162f;
	color += texture(samplerColor, inUV - (off1 / sizeColor)) * 0.3162162162f;
	color += texture(samplerColor, inUV + (off2 / sizeColor)) * 0.0702702703f;
	color += texture(samplerColor, inUV - (off2 / sizeColor)) * 0.0702702703f;
#elif BLUR_TYPE == 13
	vec2 off1 = 1.411764705882353f * scene.direction;
	vec2 off2 = 3.2941176470588234f * scene.direction;
	vec2 off3 = 5.176470588235294f * scene.direction;
	color += texture(samplerColor, inUV) * 0.1964825501511404f;
	color += texture(samplerColor, inUV + (off1 / sizeColor)) * 0.2969069646728344f;
	color += texture(samplerColor, inUV - (off1 / sizeColor)) * 0.2969069646728344f;
	color += texture(samplerColor, inUV + (off2 / sizeColor)) * 0.09447039785044732f;
	color += texture(samplerColor, inUV - (off2 / sizeColor)) * 0.09447039785044732f;
	color += texture(samplerColor, inUV + (off3 / sizeColor)) * 0.010381362401148057f;
	color += texture(samplerColor, inUV - (off3 / sizeColor)) * 0.010381362401148057f;
#endif

	imageStore(writeColor, ivec2(inUV * imageSize(writeColor)), color);
}
