#version 450

// Grid position are in xy clipped space
const vec2 gridPlane[6] = vec2[](
    vec2(-1.0, 1.0),
    vec2(-1.0, -1.0),
    vec2(1.0, 1.0),
    vec2(1.0, -1.0),
    vec2(1.0, 1.0),
    vec2(-1.0, -1.0)
);

layout (location = 0) out vec3 outWorldRay;

layout (push_constant) uniform PushObject {
    mat4 projection;
    mat4 view;
} push;

void main() {
    vec4 position = vec4(gridPlane[gl_VertexIndex], -1.0, 1.0);

    vec3 projRay = vec3(inverse(push.projection) * position);
    outWorldRay = vec3(inverse(push.view) * vec4(projRay, 0.0));

    gl_Position = position;
    gl_Position = gl_Position.xyww;
}