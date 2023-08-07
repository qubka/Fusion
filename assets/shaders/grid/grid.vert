#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

// regards to http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/

// Grid position are in xy clipped space
const vec2 gridPlane[6] = vec2[](
    vec2(-1.0, 1.0),
    vec2(-1.0, -1.0),
    vec2(1.0, 1.0),
    vec2(1.0, -1.0),
    vec2(1.0, 1.0),
    vec2(-1.0, -1.0)
);

layout (push_constant) uniform PushObject {
    mat4 projection;
    mat4 view;
} push;

layout (location = 0) out vec3 outNearPoint;
layout (location = 1) out vec3 outFarPoint;

vec3 UnprojectPoint(float x, float y, float z) {
    mat4 viewInv = inverse(push.view);
    mat4 projInv = inverse(push.projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    vec2 position = gridPlane[gl_VertexIndex];
    outNearPoint = UnprojectPoint(position.x, position.y, 0.0).xyz; // unprojecting on the near plane
    outFarPoint = UnprojectPoint(position.x, position.y, 1.0).xyz; // unprojecting on the far plane
    gl_Position = vec4(position, 0.0, 1.0); // using directly the clipped coordinates
}