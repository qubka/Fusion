#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec2 fragTexCoord;

layout (set = 0, binding = 0) uniform Ubo {
    mat4 perspective;
    mat4 orthogonal;
} ubo;

layout (push_constant) uniform Push {
    mat4 model;
} push;

void main() {
    gl_Position = ubo.perspective * push.model * vec4(position, 1.0);
    fragTexCoord = uv;
}