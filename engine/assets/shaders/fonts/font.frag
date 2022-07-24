#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0) uniform UniformObject {
	mat4 modelView;
	float alpha;

	vec4 color;
	float scale;
} object;

layout(binding = 2) uniform sampler2DArray samplerMsdf;

layout(location = 0) in vec3 inUV;

layout(location = 0) out vec4 outColor;

float median(float r, float g, float b) {
	return max(min(r, g), min(max(r, g), b));
}

void main() {
	vec3 msdfSample = texture(samplerMsdf, inUV).rgb;
	float dist = object.scale * (median(msdfSample.r, msdfSample.g, msdfSample.b) - 0.5f);
	float o = clamp(dist + 0.5f, 0.0f, 1.0f);

	outColor = object.color;
	outColor.a *= o * object.alpha;

	if (outColor.a < 0.05f) {
		outColor = vec4(0.0f);
		discard;
	}
}
