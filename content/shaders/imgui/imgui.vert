#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;

layout (push_constant) uniform PushObject {
	vec2 scale;
	vec2 translate;
} push;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec4 outColor;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {
	outUV = inUV;
	outColor = inColor;
	gl_Position = vec4(inPosition * push.scale + push.translate, 0.0, 1.0);
}