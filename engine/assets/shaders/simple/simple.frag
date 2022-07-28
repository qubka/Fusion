#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inPosition;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform UniformObject {
    mat4 projection;
    mat4 view;
    vec3 cameraPosition;
    uint lightsCount;
    vec4 ambientLightColor;
} ubo;

struct PointLight {
    vec4 color;
    vec3 position;
    float radius;
};

layout (binding = 1) buffer BufferLights {
    PointLight lights[];
} bufferLights;

void main() {
    vec3 lightColor = vec3(2.0f, 0.7f, 1.3f);
    vec3 lightPosition = vec3(0.0);
    vec3 lightAmbient = lightColor * 0.5f;
    vec3 lightDiffuse = lightColor * 0.2f;
    vec3 lightSpecular = vec3(1.0f);

    vec3 materialAmbient = vec3(1.0f, 0.5f, 0.31f);
    vec3 materialDiffuse = vec3(1.0f, 0.5f, 0.31f);
    vec3 materialSpecular = vec3(0.5f);
    float materialShininess = 32.0f;

    vec3 ambient = vec3(0.0);
    vec3 diffuse = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 specular = vec3(0.0);

    for (uint i = 0; i < ubo.lightsCount; i++) {
        PointLight light = bufferLights.lights[i];
        vec3 lightDir = light.position - inPosition;
        float attenuation = 1.0 / dot(lightDir, lightDir); // distance squared
        lightDir = normalize(lightDir);

        // ambient
        ambient += lightAmbient * materialAmbient * attenuation;

        // diffuse
        float diff = max(dot(inNormal, lightDir), 0.0);
        diffuse += lightDiffuse * (diff * materialDiffuse) * attenuation;

        // specualr
        vec3 viewDir = normalize(ubo.cameraPosition - inPosition);
        vec3 refrectDir = reflect(-lightDir, inNormal);
        float spec = pow(max(dot(viewDir, refrectDir), 0.0), materialShininess);
        specular += lightSpecular * (spec * materialSpecular) * attenuation;
    }

    vec3 result = (ambient + diffuse + specular) * inColor;
    outColor = vec4(result, 1.0);
}

/*void main() {
    vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 specularLight = vec3(0.0);

    vec3 viewDirection = normalize(ubo.cameraPosition - inPosition);

    for (uint i = 0; i < ubo.lightsCount; i++) {
        PointLight light = bufferLights.lights[i];
        vec3 directionToLight = light.position - inPosition;
        float attenuation = 1.0 / dot(directionToLight, directionToLight); // distance squared
        directionToLight = normalize(directionToLight);

        float cosAngIncidence = max(dot(inNormal, directionToLight), 0);
        vec3 intensity = light.color.xyz * light.color.w * attenuation;

        diffuseLight += intensity * cosAngIncidence;

        // specular lighting
        vec3 halfAngle = normalize(directionToLight + viewDirection);
        float blinnTerm = dot(inNormal, halfAngle);
        blinnTerm = clamp(blinnTerm, 0, 1);
        blinnTerm = pow(blinnTerm, 512.0); // higher values -> sharper highlight
        specularLight += intensity * blinnTerm;
    }

    outColor = vec4(diffuseLight * inColor + specularLight * inColor, 1.0);
}*/