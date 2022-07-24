#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(push_constant) uniform PushScene {
	float innerRadius;
	float outerRadius;
	float opacity;
} scene;

layout(binding = 0, rgba8) uniform writeonly image2D writeColor;

layout(binding = 1) uniform sampler2D samplerColor;

layout(location = 0) in vec2 inUV;

void main() {
	vec4 textureColor = texture(samplerColor, inUV);
	vec4 color = textureColor;
	color.rgb *= 1.0f - smoothstep(scene.innerRadius, scene.outerRadius, length(inUV - 0.5f));
	color = mix(textureColor, color, scene.opacity);
	
	imageStore(writeColor, ivec2(inUV * imageSize(writeColor)), color);
}
