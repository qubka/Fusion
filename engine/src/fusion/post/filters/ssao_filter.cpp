#include "ssao_filter.hpp"

#include "fusion/graphics/cameras/camera.hpp"
#include "fusion/graphics/textures/texture2d.hpp"
#include "fusion/scene/scene_manager.hpp"

using namespace fe;

static const uint32_t SSAO_NOISE_DIM = 4;
static const uint32_t SSAO_KERNEL_SIZE = 64;
static const float SSAO_RADIUS = 0.5f;

SsaoFilter::SsaoFilter(Pipeline::Stage pipelineStage) :
    PostFilter{pipelineStage, {"engine/assets/shaders/post/default.vert", "engine/assets/shaders/post/ssao.frag"}, getDefines()}
    //, noise{Resources::Get()->getThreadPool().enqueue(computeNoise, SSAO_NOISE_DIM)}
    , kernel(SSAO_KERNEL_SIZE) {
    /*for (uint32_t i = 0; i < SSAO_KERNEL_SIZE; ++i) {
        glm::vec3 sample(Random(-1.0f, 1.0f), Random(-1.0f, 1.0f), Random(0.0f, 1.0f));
        sample = glm::normalize();
        sample *= Random(0.0f, 1.0f);
        auto scale = static_cast<float>(i) / static_cast<float>(SSAO_KERNEL_SIZE);
        scale = glm::lerp(0.1f, 1.0f, scale * scale);
        kernel[i] = sample * scale;
    }*/
}

void SsaoFilter::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    auto scene = SceneManager::Get()->getScene();
    if (!scene)
        return;

    const Camera* camera = overrideCamera ? overrideCamera : scene->getCamera();
    if (!camera)
        return;

    // Updates uniforms.
    uniformScene.push("kernel", *kernel.data(), sizeof(glm::vec3) * SSAO_KERNEL_SIZE);
    uniformScene.push("projection", camera->getProjectionMatrix());
    uniformScene.push("view", camera->getViewMatrix());
    uniformScene.push("cameraPos", camera->getEyePoint());

    // Updates descriptors.
    descriptorSet.push("UniformScene", uniformScene);
    descriptorSet.push("writeColor", getAttachment("writeColor", "resolved"));
    descriptorSet.push("samplerPosition", getAttachment("samplerPosition", "position"));
    descriptorSet.push("samplerNormal", getAttachment("samplerNormal", "normals"));
    descriptorSet.push("samplerNoise", noise.get().get());

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object.
    pipeline.bindPipeline(commandBuffer);

    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

std::vector<Shader::Define> SsaoFilter::getDefines() const {
    return {
        {"SSAO_KERNEL_SIZE", std::to_string(SSAO_KERNEL_SIZE)},
        {"SSAO_RADIUS", std::to_string(SSAO_RADIUS)},
        {"RANGE_CHECK", "1"}
    };
}

std::shared_ptr<Texture2d> SsaoFilter::computeNoise(uint32_t size) {
    /*std::vector<glm::vec3> ssaoNoise(size * size);

    for (uint32_t i = 0; i < size * size; i++) {
        glm::vec3 noise{Random(-1.0f, 1.0f), Random(-1.0f, 1.0f), 0.0f}; // glm::vec3{float(i) / float(size * size), 0.0f, 0.0f};
        noise = glm::normalize(noise);
        ssaoNoise[i] = noise;
    }

    auto noiseImage = std::make_shared<Texture2d>(std::make_unique<Bitmap>(ssaoNoise, glm::uvec2{size}), VK_FORMAT_R32G32B32_SFLOAT,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_FILTER_NEAREST);*/

#ifdef FUSION_DEBUG
    // Saves the noise Image.
    /*auto filename = "SSAO_Noise.png";
    Vector2ui extent;
    auto pixels = noiseImage->GetPixels(extent, 1);
    Image::WritePixels(filename, pixels.get(), extent);*/
#endif

    //return noiseImage;
    return nullptr;
}
