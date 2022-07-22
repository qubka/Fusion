#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec4 inColor;

layout (push_constant) uniform PushObject {
	mat4 view_projection;
	mat4 transform;
} push;

layout (location = 0) out vec4 outColor;

void main() {
	outColor = inColor;
	gl_Position = push.view_projection * push.transform * vec4(inPosition, 1.0);
}
