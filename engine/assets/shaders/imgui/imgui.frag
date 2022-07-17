#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (push_constant) uniform PushObject {
	vec2 scale;
	vec2 translate;
	uint texture;
} push;

layout (binding = 0) uniform sampler2D fontSampler;
layout (binding = 1) uniform sampler2D sceneSampler;

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec4 inColor;

layout (location = 0) out vec4 outFragColor;

void main() {
	if (push.texture == 1) {
		outFragColor = inColor * texture(sceneSampler, inUV);
	} else {
		outFragColor = inColor * texture(fontSampler, inUV);
	}
}