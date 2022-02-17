#version 450

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 fragTexCoord;

layout (set = 0, binding = 0) uniform Ubo {
    mat4 projection;
    mat4 view;
    vec3 lightDirection;
} ubo;

layout (push_constant) uniform Push {
    mat4 model;
    mat4 normal;
} push;

const float AMBIENT = 0.02;

void main() {
    gl_Position = ubo.projection * ubo.view * push.model * vec4(position, 1.0);

    vec3 normalWorldSpace = normalize(mat3(push.normal) * normal);

    float lightIntensity = AMBIENT + max(dot(normalWorldSpace, ubo.lightDirection), 0);

    fragColor = lightIntensity * vec3(1.0, 1.0, 1.0);
    fragTexCoord = uv;
}