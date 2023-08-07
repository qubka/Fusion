#version 450

layout (binding = 0) uniform samplerCube skyboxSampler;

layout (location = 0) in vec3 inUV;

layout (location = 0) out vec4 outFragColor;

void main() {
	outFragColor = texture(skyboxSampler, inUV);
}