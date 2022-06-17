#pragma once

#include "fusion/renderer/vkx/context.hpp"
#include "fusion/renderer/vkx/model.hpp"
#include "fusion/renderer/vkx/texture.hpp"

namespace fe {
    class Renderer;
    class MeshRenderer {
        struct PushConstantData {
            alignas(16) glm::mat4 model{ 1.0f };
            alignas(16) glm::mat4 normal{ 1.0f };
        };

        struct Material {
            alignas(16) glm::vec4 ambient{ 0.0f };
            alignas(16) glm::vec4 diffuse{ 0.0f };
            alignas(16) glm::vec4 emission{ 0.0f };
            alignas(16) glm::vec3 specular{ 0.0f };
            alignas(4) float shininess{ 0.0f };
        };

        struct Parameters {
            alignas(8) glm::vec2 flowMapsEnabled{ 0.0f };
            alignas(4) float sunPosition{ 0.5f };
            alignas(4) uint32_t normalMapsEnabled{ 0 };
            alignas(4) uint32_t fresnelEnabled{ 0 };
            alignas(4) uint32_t rimLightEnabled{ 0 };
            alignas(4) uint32_t blinnPhongEnabled{ 0 };
            alignas(4) uint32_t celShadingEnabled{ 0 };
            alignas(4) uint32_t specularOnly{ 0 };
            alignas(4) uint32_t isParticle{ 0 };
            alignas(4) uint32_t isWater{ 0 };
        };

        struct Light {
            alignas(16) glm::vec4 color{ 1.0f };
            alignas(16) glm::vec4 ambient{ 1.0f };
            alignas(16) glm::vec4 diffuse{ 1.0f };
            alignas(16) glm::vec4 specular{ 1.0f };
            alignas(16) glm::vec4 position{ 0.0f };
            alignas(16) glm::vec3 spotDirection{ 0.0f, -1.0f, 0.0f };
            alignas(4) float spotExponent{ 0.0f };
            alignas(4) float spotCutoff{ 0.0f };
            alignas(4) float spotCosCutoff{ 0.0f };
            alignas(4) float constantAttenuation{ 0.0f };
            alignas(4) float linearAttenuation{ 0.0f };
            alignas(4) float quadraticAttenuation{ 0.0f };
            alignas(16) glm::vec3 attenuation{ 0.0f };
            alignas(16) glm::mat4 shadowViewMatrix{ 1.0f };
        };

        struct Lights {
            Light light[1];
        };

    public:
        MeshRenderer(const vkx::Context& context, Renderer& renderer)
            : context{context}, renderer{renderer} {
            assert(!instance && "Model Renderer already exists!");
            if (instance == nullptr) {
                create();
                instance = this;
            }
        }

        ~MeshRenderer() {
            if (instance != nullptr) {
                destroy();
            }
            instance = nullptr;
        };

        void begin();
        void draw(const vkx::Model& model, glm::mat4 transform);
        void end();

        static MeshRenderer& Instance() { assert(instance && "Model Renderer was not initialized!"); return *instance; }

        vkx::Model* loadModel(const std::string& filename);

    private:
        void create() {
            createUniformBuffers();
            createDescriptorSets();
            createPipelineLayout();
            createPipeline();
        }
        void destroy() {
            context.device.destroyPipelineLayout(pipelineLayout);
            context.device.destroyPipeline(pipeline);

            for (auto [path, model] : models) {
                model.destroy();
            }
            models.clear();

            materialBuffer.destroy();
            parametersBuffer.destroy();
            lightsBuffer.destroy();

            albedoTexture.destroy();
            normalTexture.destroy();
            metalnessTexture.destroy();
            roughnessTexture.destroy();

            envTexture.destroy();
            irmapTexture.destroy();
            spBRDF_LUT.destroy();
        }

        const vkx::Context& context;
        Renderer& renderer;

        void createUniformBuffers();
        void createDescriptorSets();
        void createPipelineLayout();
        void createPipeline();

        vk::Pipeline pipeline;
        vk::PipelineLayout pipelineLayout;
        vk::CommandBuffer* commandBuffer{ nullptr };

        vk::DescriptorSet uniformSet;
        vkx::Buffer materialBuffer;
        vkx::Buffer parametersBuffer;
        vkx::Buffer lightsBuffer;
        vk::DescriptorSetLayout uniformDescriptorSetLayout;

        vk::DescriptorSet textureSet;
        vkx::Texture2D albedoTexture;
        vkx::Texture2D normalTexture;
        vkx::Texture2D metalnessTexture;
        vkx::Texture2D roughnessTexture;

        vkx::TextureCubeMap envTexture;
        vkx::TextureCubeMap irmapTexture;
        vkx::Texture2D spBRDF_LUT;

        vk::DescriptorSetLayout textureDescriptorSetLayout;

        std::unordered_map<std::string, vkx::Model> models;

        static MeshRenderer* instance;
    };
}
