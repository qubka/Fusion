#include "deferred_subrender.hpp"

#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/pipelines/pipeline_compute.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/graphics/buffers/push_handler.hpp"
#include "fusion/graphics/cameras/camera.hpp"
#include "fusion/scene/scene_manager.hpp"
#include "fusion/scene/components.hpp"
#include "fusion/bitmaps/bitmap.hpp"

using namespace fe;

static const uint32_t MAX_LIGHTS = 32; // TODO: Make configurable.
static const uint32_t ENV_MAP_SIZE = 1024;
static const uint32_t IRRADIANCE_MAP_SIZE = 32;
static const uint32_t BRDF_LUT_SIZE = 256;

DeferredSubrender::DeferredSubrender(Pipeline::Stage pipelineStage)
    : Subrender{pipelineStage}
    , pipeline{pipelineStage, {"engine/assets/shaders/deferred/deferred.vert", "engine/assets/shaders/deferred/deferred.frag"}, {}, {}, PipelineGraphics::Mode::Polygon, PipelineGraphics::Depth::None}
    , brdf{ComputeBRDF(BRDF_LUT_SIZE)}
    , skybox{std::make_shared<TextureCube>("engine/assets/textures/cubemap_vulkan.ktx")}
    , fog{{ 1.0f, 1.0f, 1.0f, 1.0f }, 0.00001f, 2.0f, -0.1f, 100.0f} {

    irradiance = ComputeIrradiance(skybox, IRRADIANCE_MAP_SIZE);
    prefiltered = ComputePrefiltered(skybox, ENV_MAP_SIZE);
}

void DeferredSubrender::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    auto scene = SceneManager::Get()->getScene();
    if (!scene)
        return;

    const Camera* camera = overrideCamera ? overrideCamera : scene->getCamera();
    if (!camera)
        return;

    // TODO probably use a cubemap image directly instead of scene components.
    /*std::shared_ptr<TextureCube> skybox = nullptr;
    auto meshes = Scenes::Get()->GetScene()->QueryComponents<Mesh>();
    for (const auto &mesh : meshes) {
        if (auto materialSkybox = dynamic_cast<const SkyboxMaterial *>(mesh->GetMaterial())) {
            skybox = materialSkybox->GetImage();
            break;
        }
    }*/

    // Updates uniforms.
    std::vector<DeferredLight> deferredLights(MAX_LIGHTS);
    uint32_t lightCount = 0;

    auto view = scene->getRegistry().view<TransformComponent, LightComponent>();

    for (const auto& [entity, transform, light] : view.each()) {
        DeferredLight deferredLight = {};
        deferredLight.color = light.color;
        deferredLight.position = transform.getWorldPosition();
        deferredLight.radius = light.radius;
        deferredLights[lightCount] = deferredLight;
        lightCount++;

        if (lightCount >= MAX_LIGHTS)
            break;
    }

    // Updates uniforms.
    uniformScene.push("view", camera->getViewMatrix());
    //if (auto shadows = Scenes::Get()->GetScene()->GetSystem<Shadows>())
    //    uniformScene.push("shadowSpace", shadows->GetShadowBox().GetToShadowMapSpaceMatrix()); // TODO::
    uniformScene.push("cameraPosition", camera->getEyePoint());
    uniformScene.push("lightsCount", lightCount);
    uniformScene.push("fogColor", fog.color);
    uniformScene.push("fogDensity", fog.density);
    uniformScene.push("fogGradient", fog.gradient);

    // Updates storage buffers.
    storageLights.push(deferredLights.data(), sizeof(DeferredLight) * MAX_LIGHTS);

    // Updates descriptors.
    descriptorSet.push("UniformScene", uniformScene);
    descriptorSet.push("BufferLights", storageLights);
    descriptorSet.push("samplerPosition", Graphics::Get()->getAttachment("position"));
    descriptorSet.push("samplerDiffuse", Graphics::Get()->getAttachment("diffuse"));
    descriptorSet.push("samplerNormal", Graphics::Get()->getAttachment("normal"));
    descriptorSet.push("samplerMaterial", Graphics::Get()->getAttachment("material"));
    descriptorSet.push("samplerBRDF", brdf.get());
    descriptorSet.push("samplerIrradiance", irradiance.get());
    descriptorSet.push("samplerPrefiltered", prefiltered.get());

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object.
    pipeline.bindPipeline(commandBuffer);

    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

std::unique_ptr<Texture2d> DeferredSubrender::ComputeBRDF(uint32_t size) {
    auto brdfImage = std::make_unique<Texture2d>(glm::uvec2{size}, VK_FORMAT_R16G16_SFLOAT, VK_IMAGE_LAYOUT_GENERAL);

    // Creates the pipeline.
    CommandBuffer commandBuffer{true, VK_QUEUE_COMPUTE_BIT};
    PipelineCompute compute{"engine/assets/shaders/brdf.comp"};

    // Bind the pipeline.
    compute.bindPipeline(commandBuffer);

    // Updates descriptors.
    DescriptorsHandler descriptorSet(compute);
    descriptorSet.push("LUT", brdfImage.get());
    descriptorSet.update(compute);

    // Runs the compute pipeline.
    descriptorSet.bindDescriptor(commandBuffer, compute);
    compute.cmdRender(commandBuffer, brdfImage->getSize());
    commandBuffer.submitIdle();

#ifdef FUSION_DEBUG
    // Saves the BRDF Image.
    //brdfImage->getBitmap()->write("engine/deferred/brdf.png");
#endif

    return brdfImage;
}

std::unique_ptr<TextureCube> DeferredSubrender::ComputeIrradiance(const std::shared_ptr<TextureCube>& source, uint32_t size) {
    if (!source) {
        return nullptr;
    }

    auto irradianceCubemap = std::make_unique<TextureCube>(glm::uvec2{size}, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_LAYOUT_GENERAL);

    // Creates the pipeline.
    CommandBuffer commandBuffer{true, VK_QUEUE_COMPUTE_BIT};
    PipelineCompute compute{"engine/assets/shaders/irradiance.comp"};

    // Bind the pipeline.
    compute.bindPipeline(commandBuffer);

    // Updates descriptors.
    DescriptorsHandler descriptorSet{compute};
    descriptorSet.push("outputTexture", irradianceCubemap.get());
    descriptorSet.push("inputTexture", source.get());
    descriptorSet.update(compute);

    // Runs the compute pipeline.
    descriptorSet.bindDescriptor(commandBuffer, compute);
    compute.cmdRender(commandBuffer, irradianceCubemap->getSize());
    commandBuffer.submitIdle();

#ifdef FUSION_DEBUG
    // Saves the irradiance Image.
    //irradianceCubemap->getBitmap()->write("engine/deferred/irradiance.png");
#endif

    return irradianceCubemap;
}

std::unique_ptr<TextureCube> DeferredSubrender::ComputePrefiltered(const std::shared_ptr<TextureCube>& source, uint32_t size) {
    if (!source) {
        return nullptr;
    }

    const auto& logicalDevice = Graphics::Get()->getLogicalDevice();

    auto prefilteredCubemap = std::make_unique<TextureCube>(glm::uvec2{size}, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLE_COUNT_1_BIT, true, true);

    // Creates the pipeline.
    CommandBuffer commandBuffer(true, VK_QUEUE_COMPUTE_BIT);
    PipelineCompute compute{"engine/assets/shaders/prefiltered.comp"};

    DescriptorsHandler descriptorSet{compute};
    PushHandler pushHandler{*compute.getShader().getUniformBlock("PushObject")};

    // TODO: Use image barriers between rendering (single command buffer), rework write descriptor passing. Image class also needs a restructure.
    for (uint32_t i = 0; i < prefilteredCubemap->getMipLevels(); i++) {
        VkImageView levelView = VK_NULL_HANDLE;
        Image::CreateImageView(prefilteredCubemap->getImage(), levelView, VK_IMAGE_VIEW_TYPE_CUBE, prefilteredCubemap->getFormat(), VK_IMAGE_ASPECT_COLOR_BIT, 1, i, 6, 0);

        commandBuffer.begin();
        compute.bindPipeline(commandBuffer);

        VkDescriptorImageInfo imageInfo = {};
        imageInfo.sampler = prefilteredCubemap->getSampler();
        imageInfo.imageView = levelView;
        imageInfo.imageLayout = prefilteredCubemap->getLayout();

        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = VK_NULL_HANDLE; // Will be set in the descriptor handler.
        descriptorWrite.dstBinding = *compute.getShader().getDescriptorLocation("outputTexture");
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.descriptorType = *compute.getShader().getDescriptorType(descriptorWrite.dstBinding);
        //descriptorWrite.pImageInfo = &imageInfo;
        WriteDescriptorSet writeDescriptorSet{descriptorWrite, imageInfo};

        pushHandler.push("roughness", static_cast<float>(i) / static_cast<float>(prefilteredCubemap->getMipLevels() - 1));

        descriptorSet.push("PushObject", pushHandler);
        descriptorSet.push("outputTexture", prefilteredCubemap.get(), std::move(writeDescriptorSet));
        descriptorSet.push("inputTexture", source.get());
        descriptorSet.update(compute);

        descriptorSet.bindDescriptor(commandBuffer, compute);
        pushHandler.bindPush(commandBuffer, compute);
        compute.cmdRender(commandBuffer, prefilteredCubemap->getSize() >> i);
        commandBuffer.submitIdle();

        vkDestroyImageView(logicalDevice, levelView, nullptr);
    }

/*#ifdef FUSION_DEBUG

#endif*/

    return prefilteredCubemap;
}
