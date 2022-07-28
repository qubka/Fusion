#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//layout(constant_id = 0) const bool ANIMATED = false;
//layout(constant_id = 1) const int MAX_JOINTS = 64;
//layout(constant_id = 2) const int MAX_WEIGHTS = 64;

layout (binding = 0) uniform UniformScene {
    mat4 projection;
    mat4 view;
    vec3 cameraPos;
} scene;

layout (push_constant) uniform PushObject {
    mat4 model;
    mat4 normal;
} push;

#if ANIMATED
layout(binding = 2) buffer BufferAnimation {
    mat4 jointTransforms[];
} animation;
#endif

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
#if ANIMATED
layout(location = 3) in ivec3 inJointIds;
layout(location = 4) in vec3 inWeights;
#endif

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;

void main() {
#if ANIMATED
    vec4 position = vec4(0.0f);
    vec4 normal = vec4(0.0f);

    for (int i = 0; i < MAX_WEIGHTS; i++) {
        mat4 jointTransform = animation.jointTransforms[inJointIds[i]];
        vec4 posePosition = jointTransform * vec4(inPosition, 1.0f);
        position += posePosition * inWeights[i];

        vec4 worldNormal = jointTransform * vec4(inNormal, 0.0f);
        normal += worldNormal * inWeights[i];
    }
#else
    vec4 position = vec4(inPosition, 1.0f);
    vec4 normal = vec4(inNormal, 0.0f);
#endif

    vec4 worldPosition = push.model * position;
    gl_Position = scene.projection * scene.view * worldPosition;
    outPosition = worldPosition.xyz;
    outNormal = mat3(push.normal) * normalize(normal.xyz);
    outUV = inUV;
}