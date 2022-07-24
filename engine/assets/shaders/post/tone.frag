#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0, rgba8) uniform writeonly image2D writeColor;

layout(binding = 1) uniform sampler2D samplerColor;

layout(location = 0) in vec2 inUV;

const float gamma = 2.0f;
const float inverseGamma = 1.0f / gamma;

vec3 uncharted2(vec3 hdr) {
	float A = 0.15f;
	float B = 0.50f;
	float C = 0.10f;
	float D = 0.20f;
	float E = 0.02f;
	float F = 0.30f;
	return ((hdr * (A * hdr + C * B) + D * E) / (hdr * (A * hdr + B) + D * F)) - E / F;
}

void main() {
	vec3 textureColor = texture(samplerColor, inUV).rgb;
	vec4 color = vec4(pow(uncharted2(textureColor), vec3(inverseGamma)), 1.0f);
	
	imageStore(writeColor, ivec2(inUV * imageSize(writeColor)), color);
}
