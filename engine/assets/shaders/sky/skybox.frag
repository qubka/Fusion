#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 0) uniform samplerCube skyboxSampler;

layout (location = 0) in vec3 inUV;

layout (location = 0) out vec4 outFragColor;

void main() {
	outFragColor = texture(skyboxSampler, inUV);
}