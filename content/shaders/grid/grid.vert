#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (push_constant) uniform PushObject {
    mat4 projection;
    mat4 view;
} push;

layout (location = 0) in vec2 inPosition;

layout (location = 0) out vec3 outNearPoint;
layout (location = 1) out vec3 outFarPoint;

vec3 unprojectPoint(float x, float y, float z, mat4 viewProjInv) {
    vec4 unprojectedPoint =  viewProjInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

out gl_PerVertex {
    vec4 gl_Position;
};

// normal vertice projection
void main() {
    mat4 viewProjInv = inverse(push.view) * inverse(push.projection);
    outNearPoint = unprojectPoint(inPosition.x, inPosition.y, 0.0, viewProjInv).xyz; // unprojecting on the near plane
    outFarPoint = unprojectPoint(inPosition.x, inPosition.y, 1.0, viewProjInv).xyz; // unprojecting on the far plane
    gl_Position = vec4(inPosition, 0.0, 1.0); // using directly the clipped coordinates*/
}