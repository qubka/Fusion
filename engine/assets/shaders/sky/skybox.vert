#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inPosition;

layout (location = 0) out vec3 outUV;

layout (push_constant) uniform PushObject {
	mat4 projection;
	mat4 view;
} push;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {
	outUV = inPosition;
	gl_Position = push.projection * push.view * vec4(inPosition, 1.0);
	gl_Position = gl_Position.xyww;
}