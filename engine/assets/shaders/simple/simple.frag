#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

const float shininess = 16.0f;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform UniformObject {
    mat4 projection;
    mat4 view;
    vec3 cameraPos;
    uint lightsCount;
} ubo;

struct Light {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

layout (binding = 1) buffer BufferLights {
    Light lights[];
} bufferLights;

layout(binding = 2) uniform sampler2D samplerDiffuse;
layout(binding = 3) uniform sampler2D samplerSpecular;

// Calculates the color when using a directional light.
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    // Combine results
    vec3 ambient = light.ambient * vec3(texture(samplerDiffuse, inUV));
    vec3 diffuse = light.diffuse * diff * vec3(texture(samplerDiffuse, inUV));
    vec3 specular = light.specular * spec * vec3(texture(samplerSpecular, inUV));

    return (ambient + diffuse + specular);
}

// Calculates the color when using a point light.
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Combine results
    vec3 ambient = light.ambient * vec3(texture(samplerDiffuse, inUV));
    vec3 diffuse = light.diffuse * diff * vec3(texture(samplerDiffuse, inUV));
    vec3 specular = light.specular * spec * vec3(texture(samplerSpecular, inUV));

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
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Combine results
    vec3 ambient = light.ambient * vec3(texture(samplerDiffuse, inUV));
    vec3 diffuse = light.diffuse * diff * vec3(texture(samplerDiffuse, inUV));
    vec3 specular = light.specular * spec * vec3(texture(samplerSpecular, inUV));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}

void main() {
    // Properties
    vec3 normal = normalize(inNormal);
    vec3 viewDir = normalize(ubo.cameraPos - inPosition);

    // Directional lighting
    vec3 result = vec3(0.0);

    // Point lights
    for (uint i = 0; i < ubo.lightsCount; i++ ) {
        Light light = bufferLights.lights[i];
        if (dot(light.direction, light.direction) == 0.0) {
            result += CalcPointLight(light, normal, inPosition, viewDir);
        } else if (light.cutOff > 0.0f && light.outerCutOff > 0 && light.constant > 0.0f && light.linear > 0.0f && light.quadratic > 0.0f) {
            result += CalcSpotLight(light, normal, inPosition, viewDir);
        } else {
            result += CalcDirLight(light, normal, viewDir);
        }
    }

    outColor = vec4(result, 1.0);
}



/*
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
        Light light = bufferLights.lights[i];
        vec3 lightDir = light.position - inPosition;
        float attenuation = 1.0 / dot(lightDir, lightDir); // distance squared
        lightDir = normalize(lightDir);

        // ambient
        ambient += lightAmbient * materialAmbient * attenuation;

        // diffuse
        float diff = max(dot(inNormal, lightDir), 0.0);
        diffuse += lightDiffuse * (diff * materialDiffuse) * attenuation;

        // specualr
        vec3 viewDir = normalize(ubo.cameraPos - inPosition);
        vec3 refrectDir = reflect(-lightDir, inNormal);
        float spec = pow(max(dot(viewDir, refrectDir), 0.0), materialShininess);
        specular += lightSpecular * (spec * materialSpecular) * attenuation;
    }

    vec3 result = (ambient + diffuse + specular) * inColor;
    outColor = vec4(result, 1.0);
*/

/*void main() {
    vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 specularLight = vec3(0.0);

    vec3 viewDirection = normalize(ubo.cameraPos - inPosition);

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

/*
    vec3 shadowColor = pow(vec3(0.149, 0.220, 0.227), vec3(GAMMA));
    int shadowSamples = 2;

#if DIFFUSE_MAPPING
    vec4 diffuseTexture = texture(samplerDiffuse, inUV);
#else
    vec4 diffuseTexture = vec4(1.0);
#endif

#if PARTICLE_MAPPING
    vec4 diffuseColor = diffuseTexture * inPosition;
#else
    vec4 diffuseColor = diffuseTexture;
#endif
    diffuseColor.rgb = pow(diffuseColor.rgb, vec3(GAMMA));

    vec3 materialSpecularColor = vec3(0.5f);

    vec3 normal = normalize(inNormal);

    vec4 specularMap = vec4(0.0);//texture(p3d_Texture2, diffuseCoord);

    vec4 diffuse  = vec4(0.0, 0.0, 0.0, diffuseColor.a);
    vec4 specular = vec4(0.0, 0.0, 0.0, diffuseColor.a);

    for (uint i = 0; i < ubo.lightsCount; i++) {
        Light light = bufferLights.lights[i];
        vec3  lightDirection = light.position.xyz - inPosition.xyz * light.position.w;
        float lightDistance = length(lightDirection);

        float attenuation = 1.0 / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));
        if (attenuation <= 0.0)
            continue;

        vec3 unitLightDirection = normalize(lightDirection);
        vec3 eyeDirection       = normalize(-inPosition.xyz);
        vec3 reflectedDirection = normalize(-reflect(unitLightDirection, normal));
        vec3 halfwayDirection   = normalize(unitLightDirection + eyeDirection);

        float diffuseIntensity = dot(normal, unitLightDirection);
        if (diffuseIntensity < 0.0)
            continue;

#if CEL_SHADING_ENABLED
        diffuseIntensity = smoothstep(0.1, 0.2, diffuseIntensity);
#endif
        vec3 lightDiffuseColor = pow(light.diffuse.rgb, vec3(GAMMA));
        vec4 diffuseTemp = vec4(clamp(diffuseColor.rgb * lightDiffuseColor.rgb * diffuseIntensity, 0.0, 1.0), diffuseColor.a);

#if BLINN_PHONG_ENABLED
        float specularIntensity = clamp(dot(normal, halfwayDirection), 0.0, 1.0);
#else
        float specularIntensity = clamp(dot(eyeDirection, reflectedDirection), 0.0, 1.0);
#endif

#if CEL_SHADING_ENABLED
        specularIntensity = smoothstep(0.9, 1.0, specularIntensity);
#endif
        vec3 lightSpecularColor = pow(light.specular.rgb, vec3(GAMMA));

        vec4 materialSpecularColor = vec4(vec3(specularMap.r), diffuseColor.a);
#if FRESNEL_ENABLED
    #if BLINN_PHONG_ENABLED
        float fresnelFactor = dot(halfwayDirection, eyeDirection);
    #else
        float fresnelFactor = dot(normal, eyeDirection);
    #endif
        fresnelFactor = max(fresnelFactor, 0.0);
        fresnelFactor = 1.0 - fresnelFactor;
        fresnelFactor = pow(fresnelFactor, specularMap.b * MAX_FRESNEL_POWER);
        materialSpecularColor.rgb = mix(materialSpecularColor.rgb, vec3(1.0), clamp(fresnelFactor, 0.0, 1.0));
#endif

        vec4 specularTemp = vec4(vec3(0.0), diffuseColor.a);
#if PARTICLE_MAPPING
        //specularTemp.rgb *= 0.0f;
#else
        specularTemp.rgb  = lightSpecularColor.rgb * pow(specularIntensity, specularMap.g * MAX_SHININESS);
        specularTemp.rgb *= materialSpecularColor.rgb;
#endif
        specularTemp.rgb = clamp(specularTemp.rgb, 0.0, 1.0);

        /*float unitLightDirectionDelta = dot(normalize(light.spotDirection), -unitLightDirection);
        if (unitLightDirectionDelta < light.spotCosCutoff)
            continue;


        float spotExponent = p3d_LightSource[i].spotExponent;

        diffuseTemp.rgb *= (spotExponent <= 0.0 ? 1.0 : pow(unitLightDirectionDelta, spotExponent));

        vec2  shadowMapSize = textureSize(p3d_LightSource[i].shadowMap, 0);
        float inShadow = 0.0;

        uint count = 0;
        for (int si = -shadowSamples; si <= shadowSamples; ++si) {
            for (int sj = -shadowSamples; sj <= shadowSamples; ++sj) {
                inShadow += (1.0 - textureProj(p3d_LightSource[i].shadowMap, vertexInShadowSpaces[i] + vec4(vec2(si, sj) / shadowMapSize, vec2(0.0))));
                count += 1;
            }
        }

        inShadow /= float(count);

vec3 shadow = vec3(0.0);//mix(vec3(1.0), shadowColor, inShadow);

diffuseTemp.rgb  *= mix(shadow, vec3(1.0), 0);
specularTemp.rgb *= mix(shadow, vec3(1.0), 0);

diffuseTemp.rgb  *= attenuation;
specularTemp.rgb *= attenuation;

diffuse.rgb  += diffuseTemp.rgb;
specular.rgb += specularTemp.rgb;
}

vec4 rimLight = vec4(vec3(0.0), diffuseColor.a);
#if RIM_LIGHTING_ENABLED
rimLight.rgb = vec3(1.0 - max(0.0, dot(normalize(-inPosition.xyz), normalize(normal))));
#if CEL_SHADING_ENABLED
rimLight.rgb = smoothstep(0.3, 0.4, rimLight.rgb);
#else
rimLight.rgb = pow(rimLight.rgb, vec3(2.0)) * 1.2;
#endif
rimLight.rgb *= diffuse.rgb;
#endif

//vec3 result = (ambient + diffuse + specular) * inColor;
//outColor = vec4(result, 1.0);
vec2 ssaoBlurTexSize  = textureSize(ssaoBlurTexture, 0).xy;
vec2 ssaoBlurTexCoord = gl_FragCoord.xy / ssaoBlurTexSize;
vec3 ssao             = texture(ssaoBlurTexture, ssaoBlurTexCoord).rgb;
ssao             = mix(shadowColor, vec3(1.0), clamp(ssao.r, 0.0, 1.0));

float sunPosition = sin(SUN_POSITION * PI);
float sunMixFactor = 1.0 - (sunPosition / 2.0 + 0.5);

vec3 ambientCool = pow(vec3(0.302, 0.451, 0.471), vec3(GAMMA)) * max(0.5, sunMixFactor);
vec3 ambientWarm = pow(vec3(0.765, 0.573, 0.400), vec3(GAMMA)) * max(0.5, sunMixFactor);

vec3 skyLight    = mix(ambientCool, ambientWarm, sunMixFactor);
vec3 groundLight = mix(ambientWarm, ambientCool, sunMixFactor);

vec3 worldNormal = normalize(ubo.cameraPos - inPosition.xyz);

vec3 ambientLight = mix(groundLight, skyLight, 0.5 * (1.0 + dot(worldNormal, vec3(0, 0, 1))));

vec3 ambient = ambientLight.rgb * diffuseColor.rgb/* * ssao;

vec3 emission = vec3(1.0f, 0.5f, 0.31f) * max(0.1, pow(sunPosition, 0.4));


outColor.a = diffuseColor.a;
outColor.rgb = ambient.rgb + diffuse.rgb + rimLight.rgb + emission.rgb + specular.rgb;

*/