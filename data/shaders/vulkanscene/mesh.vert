#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec4 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inColor;

layout (binding = 0) uniform UniformBufferObject {
	mat4 projection;
	mat4 view;
	vec3 lightpos;
} ubo;

layout (push_constant) uniform PushConstants {
	mat4 model;
	mat4 normal;
} push;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outColor;
layout (location = 3) out vec3 outEyePos;
layout (location = 4) out vec3 outLightVec;

void main() {
	outUV = inTexCoord.st;
	outNormal = normalize(mat3(push.normal) * inNormal);
	outColor = inColor;
	mat4 modelView = ubo.view * push.model;
	vec4 pos = modelView * inPos;	
	gl_Position = ubo.projection * pos;
	outEyePos = vec3(modelView * pos);
	vec4 lightPos = vec4(ubo.lightpos, 1.0) * modelView;
	outLightVec = normalize(lightPos.xyz - outEyePos);
}
