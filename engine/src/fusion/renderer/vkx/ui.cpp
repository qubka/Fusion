/*
* UI overlay class using ImGui
*
* Copyright (C) 2017 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "ui.hpp"

#include "imgui.h"

#include "helpers.hpp"
#include "pipelines.hpp"

using namespace vkx;
using namespace vkx::ui;

void UIOverlay::create(const UIOverlayCreateInfo& createInfo) {
    this->createInfo = createInfo;
#if defined(__ANDROID__)
    // Screen density
    if (vkx::android::screenDensity >= ACONFIGURATION_DENSITY_XXXHIGH) {
        scale = 4.5f;
    } else if (vkx::android::screenDensity >= ACONFIGURATION_DENSITY_XXHIGH) {
        scale = 3.5f;
    } else if (vkx::android::screenDensity >= ACONFIGURATION_DENSITY_XHIGH) {
        scale = 2.5f;
    } else if (vkx::android::screenDensity >= ACONFIGURATION_DENSITY_HIGH) {
        scale = 2.0f;
    };
    vkx::logMessage(vkx::LogLevel::LOG_DEBUG, "Android UI scale %f", scale);
#endif
    // Flags
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    setStyleColors();

    // Dimensions
    //io.DisplaySize = ImVec2{static_cast<float>(createInfo.size.width), static_cast<float>(createInfo.size.height)};
    io.FontGlobalScale = scale;

    prepareResources();
    if (createInfo.renderPass) {
        renderPass = createInfo.renderPass;
    } else {
        prepareRenderPass();
    }
    preparePipeline();
}

/** Free up all Vulkan resources acquired by the UI overlay */
UIOverlay::~UIOverlay() {
}

void UIOverlay::destroy() {
    if (commandPool) {
        vertexBuffer.destroy();
        indexBuffer.destroy();
        font.destroy();
        context.device.destroyDescriptorSetLayout(descriptorSetLayout);
        context.device.destroyDescriptorPool(descriptorPool);
        context.device.destroyPipelineLayout(pipelineLayout);
        context.device.destroyPipeline(pipeline);
        if (!createInfo.renderPass) {
            context.device.destroyRenderPass(renderPass);
        }
        context.device.destroyCommandPool(commandPool);
        context.device.destroyFence(fence);
    }

    if (ImGui::GetCurrentContext()) {
        ImGui::DestroyContext();
    }
}

/** Prepare all vulkan resources required to render the UI overlay */
void UIOverlay::prepareResources() {
    ImGuiIO& io = ImGui::GetIO();

    // Create font texture
    std::vector<uint8_t> fontData;
    int texWidth, texHeight;

    // Add character ranges and merge into the previous font
    // The ranges array is not copied by the AddFont* functions and is used lazily
    // so ensure it is available at the time of building or calling GetTexDataAsRGBA32().
    static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 }; // Will not be copied by AddFont* so keep in scope.
    ImFontConfig config;
    config.MergeMode = true;

#if defined(__ANDROID__)
    float scale = android::screenDensity / (float)ACONFIGURATION_DENSITY_MEDIUM;
		AAsset* asset = AAssetManager_open(androidApp->activity->assetManager, "fonts/Roboto-Black.ttf", AASSET_MODE_STREAMING);
		if (asset) {
			size_t size = AAsset_getLength(asset);
			assert(size > 0);
			char *fontAsset = new char[size];
			AAsset_read(asset, fontAsset, size);
			AAsset_close(asset);
			io.Fonts->AddFontFromMemoryTTF(fontAsset, size, 14.0f * scale);
			delete[] fontAsset;
		}
        AAsset* asset = AAssetManager_open(androidApp->activity->assetManager, "fonts/fontawesome-webfont.ttf", AASSET_MODE_STREAMING);
		if (asset) {
			size_t size = AAsset_getLength(asset);
			assert(size > 0);
			char *fontAsset = new char[size];
			AAsset_read(asset, fontAsset, size);
			AAsset_close(asset);
			io.Fonts->AddFontFromMemoryTTF(fontAsset, size, 14.0f * scale, &config, icons_ranges);
			delete[] fontAsset;
		}
        io.Fonts->Build();
#else
    std::string filename = fe::getAssetPath() + "fonts/Roboto-Black.ttf";
    io.Fonts->AddFontFromFileTTF(filename.c_str(), 14.0f);
    filename = fe::getAssetPath() + "fonts/fontawesome-webfont.ttf"; //TODO: Replace
    io.Fonts->AddFontFromFileTTF(filename.c_str(), 14.0f, &config, icons_ranges); // Merge into first font
    io.Fonts->Build();
#endif

    {
        unsigned char* fontBuffer;
        io.Fonts->GetTexDataAsRGBA32(&fontBuffer, &texWidth, &texHeight);
        vk::DeviceSize uploadSize = texWidth * texHeight * 4 * sizeof(char);
        fontData.resize(uploadSize);
        memcpy(fontData.data(), fontBuffer, uploadSize);
    }

    // Create target image for copy
    vk::ImageCreateInfo imageInfo;
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.format = vk::Format::eR8G8B8A8Unorm;
    imageInfo.extent.width = texWidth;
    imageInfo.extent.height = texHeight;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.usage = vk::ImageUsageFlagBits::eSampled;

    font = context.stageToDeviceImage(imageInfo, fontData);

    // Image view
    vk::ImageViewCreateInfo viewInfo;
    viewInfo.image = font.image;
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = vk::Format::eR8G8B8A8Unorm;
    viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;
    font.view = context.device.createImageView(viewInfo);

    // Font texture Sampler
    vk::SamplerCreateInfo samplerInfo;
    samplerInfo.magFilter = vk::Filter::eLinear;
    samplerInfo.minFilter = vk::Filter::eLinear;
    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.borderColor = vk::BorderColor::eFloatOpaqueWhite;
    font.sampler = context.device.createSampler(samplerInfo);

    // Command buffer

    vk::CommandPoolCreateInfo cmdPoolInfo;
    cmdPoolInfo.queueFamilyIndex = context.queueIndices.graphics;
    cmdPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    commandPool = context.device.createCommandPool(cmdPoolInfo);

    // Descriptor pool
    vk::DescriptorPoolSize poolSize;
    poolSize.type = vk::DescriptorType::eCombinedImageSampler;
    poolSize.descriptorCount = 1;
    descriptorPool = context.device.createDescriptorPool({ {}, 3, 1, &poolSize });

    // Descriptor set layout
    vk::DescriptorSetLayoutBinding setLayoutBinding{ 0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment };

    descriptorSetLayout = context.device.createDescriptorSetLayout({ {}, 1, &setLayoutBinding });

    // Descriptor set
    ImTextureID fontDescriptorSet = addTexture(font.sampler, font.view, vk::ImageLayout::eShaderReadOnlyOptimal);
    io.Fonts->SetTexID(fontDescriptorSet);

    // Pipeline layout
    // Push constants for UI rendering parameters
    vk::PushConstantRange pushConstantRange{ vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstBlock) };
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{ {}, 1, &descriptorSetLayout, 1, &pushConstantRange };
    pipelineLayout = context.device.createPipelineLayout(pipelineLayoutCreateInfo);

    // Command buffer execution fence
    fence = context.device.createFence(vk::FenceCreateInfo{});
}

/** Prepare a separate pipeline for the UI overlay rendering decoupled from the main application */
void UIOverlay::preparePipeline() {
    // Setup graphics pipeline for UI rendering
    vkx::pipelines::GraphicsPipelineBuilder pipelineBuilder(context.device, pipelineLayout, renderPass);
    pipelineBuilder.depthStencilState = { false };
    pipelineBuilder.rasterizationState.cullMode = vk::CullModeFlagBits::eNone;

    // Enable blending
    pipelineBuilder.colorBlendState.blendAttachmentStates.resize(createInfo.attachmentCount);
    for (uint32_t i = 0; i < createInfo.attachmentCount; i++) {
        auto& blendAttachmentState = pipelineBuilder.colorBlendState.blendAttachmentStates[i];
        blendAttachmentState.blendEnable = VK_TRUE;
        blendAttachmentState.colorWriteMask = vkx::util::fullColorWriteMask();
        blendAttachmentState.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        blendAttachmentState.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        blendAttachmentState.colorBlendOp = vk::BlendOp::eAdd;
        blendAttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        blendAttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eZero;
        blendAttachmentState.alphaBlendOp = vk::BlendOp::eAdd;
    }

    pipelineBuilder.multisampleState.rasterizationSamples = createInfo.rasterizationSamples;

    // Load default shaders if not specified by example
    if (!createInfo.shaders.empty()) {
        pipelineBuilder.shaderStages = createInfo.shaders;
    } else {
        pipelineBuilder.loadShader(fe::getAssetPath() + "shaders/base/uioverlay.vert.spv", vk::ShaderStageFlagBits::eVertex);
        pipelineBuilder.loadShader(fe::getAssetPath() + "shaders/base/uioverlay.frag.spv", vk::ShaderStageFlagBits::eFragment);
    }

    // Vertex bindings an attributes based on ImGui vertex definition
    pipelineBuilder.vertexInputState.bindingDescriptions = { { 0, sizeof(ImDrawVert), vk::VertexInputRate::eVertex } };
    pipelineBuilder.vertexInputState.attributeDescriptions = {
        { 0, 0, vk::Format::eR32G32Sfloat, offsetof(ImDrawVert, pos) },   // Location 0: Position
        { 1, 0, vk::Format::eR32G32Sfloat, offsetof(ImDrawVert, uv) },    // Location 1: UV
        { 2, 0, vk::Format::eR8G8B8A8Unorm, offsetof(ImDrawVert, col) },  // Location 2: Color
    };
    pipeline = pipelineBuilder.create(context.pipelineCache);
}

/** Prepare a separate render pass for rendering the UI as an overlay */
void UIOverlay::prepareRenderPass() {
    std::array<vk::AttachmentDescription, 2> attachments;

    // Color attachment
    attachments[0].format = createInfo.colorformat;
    attachments[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachments[0].initialLayout = vk::ImageLayout::ePresentSrcKHR;
    attachments[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;

    // Depth attachment
    attachments[1].format = createInfo.depthformat;
    attachments[1].loadOp = vk::AttachmentLoadOp::eDontCare;
    attachments[1].storeOp = vk::AttachmentStoreOp::eDontCare;
    attachments[1].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachments[1].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachments[1].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentReference colorReference{ 0, vk::ImageLayout::eColorAttachmentOptimal };
    vk::AttachmentReference depthReference{ 1, vk::ImageLayout::eDepthStencilAttachmentOptimal };
    std::array<vk::SubpassDependency, 2> subpassDependencies;

    // Transition from final to initial (VK_SUBPASS_EXTERNAL refers to all commmands executed outside of the actual renderpass)
    subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    subpassDependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
    subpassDependencies[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
    subpassDependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;

    // Transition from initial to final
    subpassDependencies[1].srcSubpass = 0;
    subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpassDependencies[1].dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    subpassDependencies[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
    subpassDependencies[1].dstAccessMask = vk::AccessFlagBits::eMemoryRead;
    subpassDependencies[1].dependencyFlags = vk::DependencyFlagBits::eByRegion;

    vk::SubpassDescription subpassDescription;
    subpassDescription.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pDepthStencilAttachment = &depthReference;

    vk::RenderPassCreateInfo renderPassInfo;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = createInfo.subpassCount;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
    renderPassInfo.pDependencies = subpassDependencies.data();

    renderPass = context.device.createRenderPass(renderPassInfo);
}

/** Update the command buffers to reflect UI changes */
void UIOverlay::draw(const vk::CommandBuffer& cmdBuffer) {
    ImDrawData* imDrawData = ImGui::GetDrawData();
    int32_t vertexOffset = 0;
    int32_t indexOffset = 0;

    if (!imDrawData || imDrawData->CmdListsCount == 0) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    //cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSet, {});

    pushConstBlock.scale = glm::vec2{2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y};
    pushConstBlock.translate = glm::vec2{-1.0f};
    cmdBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, vk::ArrayProxy<const PushConstBlock>{ pushConstBlock });

    cmdBuffer.bindVertexBuffers(0, vertexBuffer.buffer, { 0 });
    cmdBuffer.bindIndexBuffer(indexBuffer.buffer, 0, sizeof(ImDrawIdx) == 2 ? vk::IndexType::eUint16 : vk::IndexType::eUint32);

    for (int32_t i = 0; i < imDrawData->CmdListsCount; i++) {
        const ImDrawList* cmd_list = imDrawData->CmdLists[i];
        for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++) {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
            vk::Rect2D scissorRect;
            scissorRect.offset.x = std::max(static_cast<int32_t>((pcmd->ClipRect.x)), 0);
            scissorRect.offset.y = std::max(static_cast<int32_t>((pcmd->ClipRect.y)), 0);
            scissorRect.extent.width = static_cast<uint32_t>(pcmd->ClipRect.z - pcmd->ClipRect.x);
            scissorRect.extent.height = static_cast<uint32_t>((pcmd->ClipRect.w - pcmd->ClipRect.y));
            cmdBuffer.setScissor(0, scissorRect);

            // Bind DescriptorSet with font or user texture
            vk::DescriptorSet descriptor[1] = { vk::DescriptorSet((VkDescriptorSet)pcmd->TextureId) };
            cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, descriptor, 0, nullptr);

            cmdBuffer.drawIndexed(pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);

            indexOffset += pcmd->ElemCount;
        }
        vertexOffset += cmd_list->VtxBuffer.Size;
    }
}

/** Update vertex and index buffer containing the imGui elements when required */
bool UIOverlay::update() {
    ImDrawData* imDrawData = ImGui::GetDrawData();
    bool updateCmdBuffers = false;

    if (!imDrawData) {
        return false;
    };

    // Note: Alignment is done inside buffer creation
    vk::DeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
    vk::DeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

    // Update buffers only if vertex or index count has been changed compared to current buffer size
    if ((vertexBufferSize == 0) || (indexBufferSize == 0)) {
        return false;
    }

    // Update buffers only if vertex or index count has been changed compared to current buffer size

    // Vertex buffer
    if (!vertexBuffer || (vertexCount != imDrawData->TotalVtxCount)) {
        vertexCount = imDrawData->TotalVtxCount;
        if (vertexBuffer) {
            vertexBuffer.unmap();
            context.trash<vkx::Buffer>(vertexBuffer);
            vertexBuffer = vkx::Buffer();
        }
		if (vertexCount) {
			vertexBuffer = context.createBuffer(vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible, vertexBufferSize);
			vertexBuffer.map();
			updateCmdBuffers = true;
		}
    }

    // Index buffer
    vk::DeviceSize indexSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);
    if (!indexBuffer || (indexCount < imDrawData->TotalIdxCount)) {
        indexCount = imDrawData->TotalIdxCount;
        if (indexBuffer) {
            indexBuffer.unmap();
            indexBuffer.destroy();
        }
		if (indexCount) {
			indexBuffer = context.createBuffer(vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eHostVisible, indexBufferSize);
			indexBuffer.map();
			updateCmdBuffers = true;
		}
    }

    // Upload data
    auto* vtxDst = (ImDrawVert*)vertexBuffer.mapped;
    auto* idxDst = (ImDrawIdx*)indexBuffer.mapped;

    for (int n = 0; n < imDrawData->CmdListsCount; n++) {
        const ImDrawList* cmd_list = imDrawData->CmdLists[n];
        memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtxDst += cmd_list->VtxBuffer.Size;
        idxDst += cmd_list->IdxBuffer.Size;
    }

    // Flush to make writes visible to GPU
	if (vertexBuffer) {
		vertexBuffer.flush();
    }
	if (indexBuffer) {
		indexBuffer.flush();
	}

    return updateCmdBuffers;
}

bool UIOverlay::header(const char* caption) const {
    return ImGui::CollapsingHeader(caption, ImGuiTreeNodeFlags_DefaultOpen);
}

bool UIOverlay::checkBox(const char* caption, bool* value) const {
    return ImGui::Checkbox(caption, value);
}

bool UIOverlay::checkBox(const char* caption, int32_t* value) const {
    bool val = (*value == 1);
    bool res = ImGui::Checkbox(caption, &val);
    *value = val;
    return res;
}

bool UIOverlay::inputFloat(const char* caption, float* value, float step, uint32_t precision) const {
    return ImGui::InputFloat(caption, value, step, step * 10.0f);
}

bool UIOverlay::sliderFloat(const char* caption, float* value, float min, float max) const {
    return ImGui::SliderFloat(caption, value, min, max);
}

bool UIOverlay::sliderInt(const char* caption, int32_t* value, int32_t min, int32_t max) const {
    return ImGui::SliderInt(caption, value, min, max);
}

bool UIOverlay::comboBox(const char* caption, int32_t* itemindex, const std::vector<std::string>& items) const {
    if (items.empty()) {
        return false;
    }
    std::vector<const char*> charitems;
    charitems.reserve(items.size());
    for (const auto& item : items) {
        charitems.push_back(item.c_str());
    }
    uint32_t itemCount = static_cast<uint32_t>(charitems.size());
    return ImGui::Combo(caption, itemindex, &charitems[0], itemCount, itemCount);
}

bool UIOverlay::button(const char* caption) const {
    return ImGui::Button(caption);
}

void UIOverlay::text(const char* formatstr, ...) const {
    va_list args;
    va_start(args, formatstr);
    ImGui::TextV(formatstr, args);
    va_end(args);
}

void UIOverlay::setStyleColors() {
    // Color scheme
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0 };
}

// Register a texture
// FIXME: This is experimental in the sense that we are unsure how to best design/tackle this problem, please post to https://github.com/ocornut/imgui/pull/914 if you have suggestions.
ImTextureID UIOverlay::addTexture(const vk::Sampler& sampler, const vk::ImageView& view, const vk::ImageLayout& layout) const {
    vk::DescriptorSet descriptorSet = context.device.allocateDescriptorSets({ descriptorPool, 1, &descriptorSetLayout })[0];
    vk::DescriptorImageInfo imageInfo { sampler, view, layout };
    vk::WriteDescriptorSet writeDescriptorSet{ descriptorSet, 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &imageInfo };
    context.device.updateDescriptorSets(writeDescriptorSet, {});
    return static_cast<VkDescriptorSet>(descriptorSet);
}