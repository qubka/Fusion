#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout (constant_id = 0) const bool blinnPhongEnabled = true;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec3 inBitangent;
layout (location = 4) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform UniformObject {
    mat4 projection;
    mat4 view;
    vec3 cameraPos;
    int lightsCount;
} ubo;

layout (push_constant) uniform PushObject {
    mat4 model;
    mat4 normal;
} push;

#define DIFFUSE int(push.normal[0].w)
#define SPECULAR int(push.normal[1].w)
#define NORMAL int(push.normal[2].w)
#define SHININESS push.normal[3].w
#define COLOR push.normal[3].xyz

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

layout (binding = 2) uniform sampler2D textures[];

// Calculates the color when using a directional light.
vec3 CalcDirLight(Light light, vec3 baseDiffuse, vec3 baseSpecular, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    float spec;
    if (blinnPhongEnabled) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), SHININESS);
    } else {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), SHININESS);
    }

    // Combine results
    vec3 ambient = light.ambient * baseDiffuse;
    vec3 diffuse = light.diffuse * diff * baseDiffuse;
    vec3 specular = light.specular * spec * baseSpecular;

    return (ambient + diffuse + specular);
}

// Calculates the color when using a point light.
vec3 CalcPointLight(Light light, vec3 baseDiffuse, vec3 baseSpecular, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    float spec;
    if (blinnPhongEnabled) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), SHININESS);
    } else {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), SHININESS);
    }

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Combine results
    vec3 ambient = light.ambient * baseDiffuse;
    vec3 diffuse = light.diffuse * diff * baseDiffuse;
    vec3 specular = light.specular * spec * baseSpecular;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight(Light light, vec3 baseDiffuse, vec3 baseSpecular, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    float spec;
    if (blinnPhongEnabled) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), SHININESS);
    } else {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), SHININESS);
    }

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Combine results
    vec3 ambient = light.ambient * baseDiffuse;
    vec3 diffuse = light.diffuse * diff * baseDiffuse;
    vec3 specular = light.specular * spec * baseSpecular;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}

void main() {
    // Properties
    vec3 viewDir = normalize(ubo.cameraPos - inPosition);

    // Diffuse mapping
    vec3 diffuse;
    if (DIFFUSE == -1) {
        diffuse = COLOR;
    } else {
        diffuse = texture(textures[DIFFUSE], inUV).rgb;
    }

    // Specular mapping
    vec3 specular;
    if (SPECULAR == -1) {
        specular = vec3(1.0);
    } else {
        specular = texture(textures[SPECULAR], inUV).rgb;
    }

    // Normal mapping
    vec3 normal;
    mat3 matrixNormal = mat3(push.normal);
    if (NORMAL == -1) {
        normal = normalize(matrixNormal * inNormal);
    } else {
        vec3 N = normalize(matrixNormal * inNormal);
        vec3 T = normalize(matrixNormal * inTangent);
        vec3 B = normalize(matrixNormal * inBitangent);
        vec3 tangentNormal = texture(textures[NORMAL], inUV).rgb * 2.0f - 1.0f;
        normal = mat3(T, B, N) * tangentNormal;
    }

    // Directional lighting
    vec3 result = vec3(0.0);

    // Point lights
    for (uint i = 0; i < ubo.lightsCount; i++) {
        Light light = bufferLights.lights[i];
        if (dot(light.direction, light.direction) == 0.0) {
            result += CalcPointLight(light, diffuse, specular, normal, inPosition, viewDir);
        } else if (light.cutOff < 0.0f && light.outerCutOff < 0.0f) {
            result += CalcDirLight(light, diffuse, specular, normal, viewDir);
        } else {
            result += CalcSpotLight(light, diffuse, specular, normal, inPosition, viewDir);
        }
    }

    outColor = vec4(result, 1.0);
}