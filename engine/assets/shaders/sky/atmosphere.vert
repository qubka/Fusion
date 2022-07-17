#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec2 inPosition;

layout (location = 0) out vec3 outWorldRay;

layout (push_constant) uniform PushObject {
    mat4 projection;
    mat4 view;
} push;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    vec4 position = vec4(inPosition, -1.0, 1.0);

    vec3 projRay = vec3(inverse(push.projection) * position);
    outWorldRay = vec3(inverse(push.view) * vec4(projRay, 0.0));

    gl_Position = position;
    gl_Position = gl_Position.xyww;
}