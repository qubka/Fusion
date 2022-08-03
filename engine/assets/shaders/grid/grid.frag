#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

// regards to http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/

layout (push_constant) uniform PushObject {
    mat4 projection;
    mat4 view;
} push;

layout (location = 0) in vec3 inNearPoint;
layout (location = 1) in vec3 inFarPoint;

layout (location = 0) out vec4 outFragColor;
//layout (location = 1) out float gl_FragDepth;

vec4 grid(vec3 fragPos, float scale) {
    vec2 coord = fragPos.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimum_z = min(derivative.y, 1);
    float minimum_x = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    // z axis
    if (fragPos.x > -0.1 * minimum_x && fragPos.x < 0.1 * minimum_x)
        color.z = 1.0;
    // x axis
    if (fragPos.z > -0.1 * minimum_z && fragPos.z < 0.1 * minimum_z)
        color.x = 1.0;
    return color;
}

float computeDepth(vec3 pos) {
    vec4 clipSpacePos = push.projection * push.view * vec4(pos, 1.0);
    return (clipSpacePos.z / clipSpacePos.w);
}

float computeLinearDepth(float depth, float near, float far) {
    float clipSpaceDepth = depth * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * near * far) / (far + near - clipSpaceDepth * (far - near)); // get linear value between 0.01 and 100
    return linearDepth / far; // normalize
}

void main() {
    float t = -inNearPoint.y / (inFarPoint.y - inNearPoint.y);
    vec3 fragPos = inNearPoint + t * (inFarPoint - inNearPoint);

    float depth = computeDepth(fragPos);
    float fading = max(0, (0.5 - computeLinearDepth(depth, 0.1, 100.0)));

    outFragColor = (grid(fragPos, 10) + grid(fragPos, 1)) * float(t > 0); // adding multiple resolution for the grid
    outFragColor.a *= fading;

    gl_FragDepth = depth;
}