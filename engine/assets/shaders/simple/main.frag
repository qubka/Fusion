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
    vec3 cameraPos;
} ubo;

/*layout (binding = 1) uniform Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} material;

layout (binding = 2) uniform Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} light;*/

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} material;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} light;

void main() {

    vec3 lightColor = vec3(2.0f, 0.7f, 1.3f);
    light.position = vec3(0.0);
    light.ambient = lightColor * 0.5f;
    light.diffuse = lightColor * 0.2f;
    light.specular = vec3(1.0f);

    material.ambient = vec3(1.0f, 0.5f, 0.31f);
    material.diffuse = vec3(1.0f, 0.5f, 0.31f);
    material.specular = vec3(0.5f);
    material.shininess = 32.0f;

    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse
    vec3 norm = normalize(inNormal);
    vec3 lightDir = normalize(light.position - inPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // specualr
    vec3 viewDir = normalize(ubo.cameraPos - inPosition);
    vec3 refrectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, refrectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = (ambient + diffuse + specular) * inColor;
    outColor = vec4(result, 1.0);
}