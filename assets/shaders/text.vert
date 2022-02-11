#version 450
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>

layout (location = 0) out vec2 fragTexCoord;

layout (binding = 0) uniform Ubo {
    mat4 perspective;
    mat4 orthogonal;
} ubo;

void main()
{
    gl_Position = ubo.orthogonal * vec4(vertex.xy, 0.0, 1.0);
    fragTexCoord = vertex.zw;
}