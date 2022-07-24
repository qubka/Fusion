#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(push_constant) uniform PushScene {
	vec4 screenColor;
	float curveAmountX;
	float curveAmountY;
	float scanLineSize;
	float scanIntensity;
	float moveTime;
} scene;

layout(binding = 0, rgba8) uniform writeonly image2D writeColor;

layout(binding = 1) uniform sampler2D samplerColor;

layout(location = 0) in vec2 inUV;

void main() {
	vec2 tc = vec2(inUV.x, inUV.y);

	// Distance from the center
	float dx = abs(0.5f - tc.x);
	float dy = abs(0.5f - tc.y);

	// Square it to smooth the edges
	dx *= dx;
	dy *= dy;

	tc.x -= 0.5f;
	tc.x *= 1.0f + (dy * scene.curveAmountX);
	tc.x += 0.5f;

	tc.y -= 0.5f;
	tc.y *= 1.0f + (dx * scene.curveAmountY);
	tc.y += 0.5f;

	// Get texel, and add in scanline if need be
	vec4 color = texture(samplerColor, tc);
	color.rgb += sin((tc.y + scene.moveTime) * scene.scanLineSize) * scene.scanIntensity;

	// Cutoff
	if (tc.y > 1.0f || tc.x < 0.0f || tc.x > 1.0f || tc.y < 0.0f) {
		color = vec4(0.0f);
	}

	float grey = dot(color.rgb, vec3(0.299f, 0.587f, 0.114f));
	color = vec4(scene.screenColor.rgb * grey, 1.0f);

	imageStore(writeColor, ivec2(inUV * imageSize(writeColor)), color);
}
