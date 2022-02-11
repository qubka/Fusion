#version 450

layout (location = 0) in vec2 fragTexCoord;

layout (location = 0) out vec4 outColor;

layout (set = 1, binding = 0) uniform sampler2D texSampler;

layout (push_constant) uniform Push {
    vec4 color;
} push;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(texSampler, fragTexCoord).r);
    outColor = push.color * sampled;
}