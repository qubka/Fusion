#include "fusion/core/offscreen_application.hpp"
#include "fusion/core/entry_point.hpp"
#include "editor_layer.hpp"

using namespace Fusion;

// Vertex layout for this example
vkx::model::VertexLayout vertexLayout{ {
    vkx::model::Component::VERTEX_COMPONENT_POSITION,
    vkx::model::Component::VERTEX_COMPONENT_UV,
    vkx::model::Component::VERTEX_COMPONENT_COLOR,
    vkx::model::Component::VERTEX_COMPONENT_NORMAL,
} };

class EditorApp : public OffscreenApplication {
public:
    struct {
        vkx::texture::Texture2D colorMap;
    } textures;

    struct {
        vkx::model::Model example;
        vkx::model::Model plane;
    } meshes;

    struct {
        vkx::Buffer vsShared;
        vkx::Buffer vsMirror;
        vkx::Buffer vsOffScreen;
    } uniformData;

    struct UBO {
        glm::mat4 projection;
        glm::mat4 model;
        glm::vec4 lightPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    };

    struct {
        UBO vsShared;
    } ubos;

    struct {
        vk::Pipeline shaded;
        vk::Pipeline mirror;
    } pipelines;

    struct {
        vk::PipelineLayout quad;
        vk::PipelineLayout offscreen;
    } pipelineLayouts;

    struct {
        vk::DescriptorSet mirror;
        vk::DescriptorSet model;
        vk::DescriptorSet offscreen;
    } descriptorSets;

    vk::DescriptorSetLayout descriptorSetLayout;

    glm::vec3 meshPos = glm::vec3(0.0f, -1.5f, 0.0f);

    EditorCamera camera{60, 0.1f, 1.0f, 2.0f};

    EditorApp(const CommandLineArgs& args) : OffscreenApplication{"Vulkan Example - Offscreen rendering", args} {
        camera.setPerspective(60.0f, static_cast<float>(size.width) / size.height, 0.1f, 1000.0f);
        camera.setRotation(glm::quat{glm::vec3{ -2.5f, 0.0f, 0.0f }});
        camera.setPosition({ 0.0f, 1.0f, 0.0f });
    }

    ~EditorApp() {
        // Clean up used Vulkan resources
        // Note : Inherited destructor cleans up resources stored in base class

        // Textures
        //textureTarget.destroy();
        textures.colorMap.destroy();

        device.destroyPipeline(pipelines.shaded);
        device.destroyPipeline(pipelines.mirror);
        device.destroyPipelineLayout(pipelineLayouts.offscreen);
        device.destroyPipelineLayout(pipelineLayouts.quad);

        device.destroyDescriptorSetLayout(descriptorSetLayout);

        // Meshes
        meshes.example.destroy();
        meshes.plane.destroy();

        // Uniform buffers
        uniformData.vsShared.destroy();
        uniformData.vsMirror.destroy();
        uniformData.vsOffScreen.destroy();
    }

    // The command buffer to copy for rendering
    // the offscreen scene and blitting it into
    // the texture target is only build once
    // and gets resubmitted
    void buildOffscreenCommandBuffer() override {
        vk::ClearValue clearValues[2];
        clearValues[0].color = vkx::util::clearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
        clearValues[1].depthStencil = vk::ClearDepthStencilValue{ 1.0f, 0 };

        vk::RenderPassBeginInfo renderPassBeginInfo;
        renderPassBeginInfo.renderPass = offscreen.renderPass;
        renderPassBeginInfo.framebuffer = offscreen.framebuffers[0].framebuffer;
        renderPassBeginInfo.renderArea.extent.width = offscreen.size.x;
        renderPassBeginInfo.renderArea.extent.height = offscreen.size.y;
        renderPassBeginInfo.clearValueCount = 2;
        renderPassBeginInfo.pClearValues = clearValues;

        vk::CommandBufferBeginInfo cmdBufInfo;
        cmdBufInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
        offscreen.cmdBuffer.begin(cmdBufInfo);
        offscreen.cmdBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
        offscreen.cmdBuffer.setViewport(0, vkx::util::viewport(offscreen.size));
        offscreen.cmdBuffer.setScissor(0, vkx::util::rect2D(offscreen.size));
        offscreen.cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayouts.offscreen, 0, descriptorSets.offscreen, nullptr);
        offscreen.cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines.shaded);
        offscreen.cmdBuffer.bindVertexBuffers(0, meshes.example.vertices.buffer, { 0 });
        offscreen.cmdBuffer.bindIndexBuffer(meshes.example.indices.buffer, 0, vk::IndexType::eUint32);
        offscreen.cmdBuffer.drawIndexed(meshes.example.indexCount, 1, 0, 0, 0);
        offscreen.cmdBuffer.endRenderPass();
        offscreen.cmdBuffer.end();
    }

    void onUpdateDrawCommandBuffer(const vk::CommandBuffer& cmdBuffer) override {
        vk::DeviceSize offsets = 0;
        cmdBuffer.setViewport(0, vkx::util::viewport(size));
        cmdBuffer.setScissor(0, vkx::util::rect2D(size));

        // Reflection plane
        cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayouts.quad, 0, descriptorSets.mirror, nullptr);
        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines.mirror);
        cmdBuffer.bindVertexBuffers(0, meshes.plane.vertices.buffer, offsets);
        cmdBuffer.bindIndexBuffer(meshes.plane.indices.buffer, 0, vk::IndexType::eUint32);
        cmdBuffer.drawIndexed(meshes.plane.indexCount, 1, 0, 0, 0);

        // Model
        cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayouts.quad, 0, descriptorSets.model, nullptr);
        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines.shaded);
        cmdBuffer.bindVertexBuffers(0, meshes.example.vertices.buffer, offsets);
        cmdBuffer.bindIndexBuffer(meshes.example.indices.buffer, 0, vk::IndexType::eUint32);
        cmdBuffer.drawIndexed(meshes.example.indexCount, 1, 0, 0, 0);
    }

    void onLoadAssets() override {
        meshes.plane.loadFromFile(context, getAssetPath() + "models/plane.obj", vertexLayout, 0.4f);
        meshes.example.loadFromFile(context, getAssetPath() + "models/chinesedragon.dae", vertexLayout, 0.3f);
        std::string filename;
        vk::Format format;
        if (context.deviceFeatures.textureCompressionBC) {
            filename = "textures/darkmetal_bc3_unorm.ktx";
            format = vk::Format::eBc3UnormBlock;
        } else if (context.deviceFeatures.textureCompressionASTC_LDR) {
            filename = "textures/darkmetal_astc_8x8_unorm.ktx";
            format = vk::Format::eAstc8x8UnormBlock;
        } else if (context.deviceFeatures.textureCompressionETC2) {
            filename = "textures/darkmetal_etc2_unorm.ktx";
            format = vk::Format::eEtc2R8G8B8UnormBlock;
        } else {
            throw std::runtime_error("Device does not support any compressed texture format!");
        }
        textures.colorMap.loadFromFile(context, getAssetPath() + filename, format);
    }

    void setupDescriptorPool() {
        std::vector<vk::DescriptorPoolSize> poolSizes{
                { vk::DescriptorType::eUniformBuffer, 6 },
                { vk::DescriptorType::eCombinedImageSampler, 8 },
        };
        descriptorPool = device.createDescriptorPool({ {}, 5, static_cast<uint32_t>(poolSizes.size()), poolSizes.data() });
    }

    void setupDescriptorSetLayout() {
        // Textured quad pipeline layout
        std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings{
                // Binding 0 : Vertex shader uniform buffer
                { 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex },
                // Binding 1 : Fragment shader image sampler
                { 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment },
                // Binding 2 : Fragment shader image sampler
                { 2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment },
        };

        descriptorSetLayout = device.createDescriptorSetLayout({ {}, static_cast<uint32_t>(setLayoutBindings.size()), setLayoutBindings.data() });
        pipelineLayouts.quad = device.createPipelineLayout({ {}, 1, &descriptorSetLayout });
        // Offscreen pipeline layout
        pipelineLayouts.offscreen = device.createPipelineLayout({ {}, 1, &descriptorSetLayout });
    }

    void setupDescriptorSet() {
        // Mirror plane descriptor set
        vk::DescriptorSetAllocateInfo allocInfo{ descriptorPool, 1, &descriptorSetLayout };
        descriptorSets.mirror = device.allocateDescriptorSets(allocInfo)[0];

        // vk::Image descriptor for the offscreen mirror texture
        vk::DescriptorImageInfo texDescriptorMirror{ offscreen.framebuffers[0].colors[0].sampler, offscreen.framebuffers[0].colors[0].view,
                                                     vk::ImageLayout::eShaderReadOnlyOptimal };
        // vk::Image descriptor for the color map
        vk::DescriptorImageInfo texDescriptorColorMap{ textures.colorMap.sampler, textures.colorMap.view, vk::ImageLayout::eGeneral };

        std::vector<vk::WriteDescriptorSet> writeDescriptorSets{
                // Binding 0 : Vertex shader uniform buffer
                { descriptorSets.mirror, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &uniformData.vsMirror.descriptor },
                // Binding 1 : Fragment shader texture sampler
                { descriptorSets.mirror, 1, 0, 1, vk::DescriptorType::eCombinedImageSampler, &texDescriptorMirror },
                // Binding 2 : Fragment shader texture sampler
                { descriptorSets.mirror, 2, 0, 1, vk::DescriptorType::eCombinedImageSampler, &texDescriptorColorMap },
        };
        device.updateDescriptorSets(writeDescriptorSets, {});

        // Model
        // No texture
        descriptorSets.model = device.allocateDescriptorSets(allocInfo)[0];
        std::vector<vk::WriteDescriptorSet> modelWriteDescriptorSets{
                // Binding 0 : Vertex shader uniform buffer
                { descriptorSets.model, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &uniformData.vsShared.descriptor },
        };
        device.updateDescriptorSets(modelWriteDescriptorSets, {});

        // Offscreen
        descriptorSets.offscreen = device.allocateDescriptorSets(allocInfo)[0];
        std::vector<vk::WriteDescriptorSet> offscreenWriteDescriptorSets{
                // Binding 0 : Vertex shader uniform buffer
                { descriptorSets.offscreen, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &uniformData.vsOffScreen.descriptor },
        };
        device.updateDescriptorSets(offscreenWriteDescriptorSets, {});
    }

    void preparePipelines() {
        // Solid rendering pipeline
        vkx::pipelines::GraphicsPipelineBuilder pipelineBuilder{ device, pipelineLayouts.quad, renderPass };
        pipelineBuilder.rasterizationState.frontFace = vk::FrontFace::eClockwise;
        pipelineBuilder.vertexInputState.appendVertexLayout(vertexLayout);
        pipelineBuilder.loadShader(getAssetPath() + "shaders/offscreen/mirror.vert.spv", vk::ShaderStageFlagBits::eVertex);
        pipelineBuilder.loadShader(getAssetPath() + "shaders/offscreen/mirror.frag.spv", vk::ShaderStageFlagBits::eFragment);
        pipelines.mirror = pipelineBuilder.create(context.pipelineCache);
        pipelineBuilder.destroyShaderModules();

        // Solid shading pipeline
        pipelineBuilder.loadShader(getAssetPath() + "shaders/offscreen/offscreen.vert.spv", vk::ShaderStageFlagBits::eVertex);
        pipelineBuilder.loadShader(getAssetPath() + "shaders/offscreen/offscreen.frag.spv", vk::ShaderStageFlagBits::eFragment);
        pipelineBuilder.layout = pipelineLayouts.offscreen;
        pipelines.shaded = pipelineBuilder.create(context.pipelineCache);
    }

    // Prepare and initialize uniform buffer containing shader uniforms
    void prepareUniformBuffers() {
        // Mesh vertex shader uniform buffer block
        uniformData.vsShared = context.createUniformBuffer(ubos.vsShared);
        // Mirror plane vertex shader uniform buffer block
        uniformData.vsMirror = context.createUniformBuffer(ubos.vsShared);
        // Offscreen vertex shader uniform buffer block
        uniformData.vsOffScreen = context.createUniformBuffer(ubos.vsShared);

        updateUniformBuffers();
        updateUniformBufferOffscreen();
    }

    void updateUniformBuffers() {
        // Mesh
        ubos.vsShared.projection = camera.getViewProjection();
        ubos.vsShared.model = glm::translate(glm::mat4{1}, meshPos);
        uniformData.vsShared.copy(ubos.vsShared);

        // Mirror
        ubos.vsShared.model = glm::mat4{1};
        uniformData.vsMirror.copy(ubos.vsShared);
    }

    void updateUniformBufferOffscreen() {
        ubos.vsShared.projection = camera.getViewProjection();
        ubos.vsShared.model = glm::mat4{1};
        ubos.vsShared.model = glm::scale(ubos.vsShared.model, glm::vec3(1.0f, -1.0f, 1.0f));
        ubos.vsShared.model = glm::translate(ubos.vsShared.model, meshPos);
        uniformData.vsOffScreen.copy(ubos.vsShared);
    }

    void prepare() override {
        offscreen.size = glm::uvec2(512);
        OffscreenApplication::prepare();
        prepareUniformBuffers();
        setupDescriptorSetLayout();
        preparePipelines();
        setupDescriptorPool();
        setupDescriptorSet();
        buildOffscreenCommandBuffer();
        buildCommandBuffers();
    }

    void render() override {
        draw();

        /*

        static float yaw = 0.0f;
        static float pitch = 0.0f;

        glm::vec2 delta = Input::MouseDelta() * 0.0001f;
        yaw -= delta.x;
        pitch -= delta.y;

        static constexpr float limit = glm::radians(89.0f);
        if (pitch > limit) {
            pitch = limit;
        }
        if (pitch < -limit) {
            pitch = -limit;
        }

        camera.rotation = glm::degrees(glm::eulerAngles(glm::quat{ glm::vec3(pitch, yaw, 0) }));*/

        camera.update(frameTimer);
        updateUniformBuffers();
        updateUniformBufferOffscreen();
    }

    void onViewChanged() override {
        updateUniformBuffers();
        updateUniformBufferOffscreen();
    }
};

Fusion::Application* Fusion::CreateApplication(CommandLineArgs args) {
    return new EditorApp{args};
}