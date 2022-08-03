#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#define BLINN_PHONG_ENABLED 1

const float shininess = 32.0f;
const float gamma = 2.2f;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec3 inBitangent;
layout (location = 4) in vec2 inUV0;
layout (location = 5) in vec2 inUV1;

layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform UniformObject {
    mat4 projection;
    mat4 view;
    vec3 cameraPos;
    uint lightsCount;
} ubo;

struct Light {
    vec3 position;
    float cutOff;
    vec3 direction;
    float outerCutOff;
    vec3 ambient;
    float constant;
    vec3 diffuse;
    float linear;
    vec3 specular;
    float quadratic;
};

layout (binding = 1) buffer BufferLights {
    Light lights[];
} bufferLights;

layout(binding = 2) uniform sampler2D samplerDiffuse;
layout(binding = 3) uniform sampler2D samplerSpecular;
layout(binding = 4) uniform sampler2D samplerNormal;

// Calculates the color when using a directional light.
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
#if BLINN_PHONG_ENABLED
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
#else
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
#endif

    // Combine results
    vec3 ambient = light.ambient * vec3(texture(samplerDiffuse, inUV0));
    vec3 diffuse = light.diffuse * diff * vec3(texture(samplerDiffuse, inUV0));
    vec3 specular = light.specular * spec * vec3(texture(samplerSpecular, inUV0));

    return (ambient + diffuse + specular);
}

// Calculates the color when using a point light.
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
#if BLINN_PHONG_ENABLED
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
#else
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
#endif

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Combine results
    vec3 ambient = light.ambient * vec3(texture(samplerDiffuse, inUV0));
    vec3 diffuse = light.diffuse * diff * vec3(texture(samplerDiffuse, inUV0));
    vec3 specular = light.specular * spec * vec3(texture(samplerSpecular, inUV0));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
#if BLINN_PHONG_ENABLED
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
#else
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
#endif

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Combine results
    vec3 ambient = light.ambient * vec3(texture(samplerDiffuse, inUV0));
    vec3 diffuse = light.diffuse * diff * vec3(texture(samplerDiffuse, inUV0));
    vec3 specular = light.specular * spec * vec3(texture(samplerSpecular, inUV0));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}

void main() {
    // Properties
    vec3 viewDir = normalize(ubo.cameraPos - inPosition);
    vec3 tangentNormal = texture(samplerNormal, inUV1).rgb * 2.0f - 1.0f;
    vec3 normal = mat3(inTangent, inBitangent, inNormal) * tangentNormal;
    
    // Directional lighting
    vec3 result = vec3(0.0);

    // Point lights
    for (uint i = 0; i < ubo.lightsCount; i++ ) {
        Light light = bufferLights.lights[i];
        if (dot(light.direction, light.direction) == 0.0) {
            result += CalcPointLight(light, normal, inPosition, viewDir);
        } else if (light.cutOff < 0.0f && light.outerCutOff < 0.0f) {
            result += CalcDirLight(light, normal, viewDir);
        } else {
            result += CalcSpotLight(light, normal, inPosition, viewDir);
        }
    }

    outColor = vec4(result, 1.0);
}