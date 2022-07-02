#version 450

layout (location = 0) out vec3 outWorldRay;

layout (binding = 0) uniform UniformBufferObject {
    mat4 projection;
    mat4 view;
} ubo;

// Quad position are in xyz clipped space
vec3 quadPlane[6] = vec3[](
    vec3(-1, 1, -1), vec3(1, 1, -1), vec3(-1, -1, -1),
    vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1)
);

void main() {
    vec3 position = quadPlane[gl_VertexIndex];

    vec3 projRay = vec3(inverse(ubo.projection) * vec4(position, 1.0));
    outWorldRay = vec3(inverse(ubo.view) * vec4(projRay, 0.0));

    gl_Position = vec4(position, 1.0);
    gl_Position = gl_Position.xyww;
}