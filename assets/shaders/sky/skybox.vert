#version 450

layout (location = 0) in vec3 inPosition;

layout (location = 0) out vec3 outUV;

layout (push_constant) uniform PushObject {
	mat4 projection;
	mat4 view;
} push;

void main() {
	outUV = inPosition;
	gl_Position = push.projection * push.view * vec4(inPosition, 1.0);
	gl_Position = gl_Position.xyww;
}