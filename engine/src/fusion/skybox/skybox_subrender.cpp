#include "skybox_subrender.h"

#include "fusion/graphics/commands/command_buffer.h"
#include "fusion/graphics/cameras/camera.h"
#include "fusion/graphics/buffers/buffer.h"
#include "fusion/graphics/textures/texture_cube.h"
#include "fusion/devices/device_manager.h"
#include "fusion/scene/scene_manager.h"

using namespace fe;

SkyboxSubrender::SkyboxSubrender(Pipeline::Stage pipelineStage)
        : Subrender{pipelineStage}
        , pipeline{pipelineStage, {FUSION_ASSET_PATH "shaders/sky/skybox.vert", FUSION_ASSET_PATH "shaders/sky/skybox.frag"}, {{{Vertex::Component::Position}}}}
        , descriptorSet{pipeline} {

    std::vector<glm::vec3> vertices {
            //front
            {-1.f, -1.f,  1.f},
            { 1.f, -1.f,  1.f},
            { 1.f,  1.f,  1.f},
            {-1.f,  1.f,  1.f},

            //right
            { 1.f, -1.f,  1.f},
            { 1.f, -1.f, -1.f},
            { 1.f,  1.f, -1.f},
            { 1.f,  1.f,  1.f},

            //back
            { 1.f, -1.f, -1.f},
            {-1.f, -1.f, -1.f},
            {-1.f,  1.f, -1.f},
            { 1.f,  1.f, -1.f},

            //left
            {-1.f, -1.f, -1.f},
            {-1.f, -1.f,  1.f},
            {-1.f,  1.f,  1.f},
            {-1.f,  1.f, -1.f},

            //top
            {-1.f,  1.f,  1.f},
            { 1.f,  1.f,  1.f},
            { 1.f,  1.f, -1.f},
            {-1.f,  1.f, -1.f},

            //bottom
            {-1.f, -1.f, -1.f},
            { 1.f, -1.f, -1.f},
            { 1.f, -1.f,  1.f},
            {-1.f, -1.f,  1.f},
    };

    vertexBuffer = Buffer::StageToDeviceBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, sizeof(glm::vec3) * vertices.size(), vertices.data());


    std::vector<uint16_t> indices {
            2,  1,  0,		3,  2,  0,  //front
            6,  5,  4,		7,  6,  4,  //right
            10, 9,  8,		11, 10, 8,  //back
            14, 13, 12,		15, 14, 12, //left
            18, 17, 16,		19, 18, 16, //upper
            22, 21, 20,		23, 22, 20  //bottom
    };

    indexBuffer = Buffer::StageToDeviceBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, sizeof(uint16_t) * indices.size(), indices.data());

    skyboxSampler = std::make_unique<TextureCube>(FUSION_ASSET_PATH "textures/cubemap_vulkan.ktx", VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, true, true, true);
}

SkyboxSubrender::~SkyboxSubrender() {
}

void SkyboxSubrender::onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) {
    auto scene = SceneManager::Get()->getScene();
    if (!scene)
        return;

    const Camera* camera = overrideCamera ? overrideCamera : scene->getCamera();
    if (!camera)
        return;

    // Updates uniform
    pushObject.push("projection", camera->getProjectionMatrix());
    pushObject.push("view", glm::mat4{glm::mat3{camera->getViewMatrix()}});

    // Updates descriptors
    descriptorSet.push("skyboxSampler", skyboxSampler.get());
    descriptorSet.push("PushObject", pushObject);

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object
    pipeline.bindPipeline(commandBuffer);
    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    pushObject.bindPush(commandBuffer, pipeline);

    VkBuffer vertexBuffers[1] = { *vertexBuffer };
    VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, *indexBuffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(commandBuffer, 36, 1, 0, 0, 0);
}