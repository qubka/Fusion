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
using namespace fe;

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
    io.DisplaySize = ImVec2{static_cast<float>(createInfo.size.width), static_cast<float>(createInfo.size.height)};
    io.FontGlobalScale = scale;

    prepareEvents();
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
    io.Fonts->AddFontDefault();
    std::string filename = getAssetPath() + "/fonts/fontawesome-webfont.ttf"; //TODO: Replace
    io.Fonts->AddFontFromFileTTF(filename.c_str(), 16.0f, &config, icons_ranges); // Merge into first font
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

    // Offscreen frame images
    for (auto& framebuffer : createInfo.framebuffers) {
        auto& color = framebuffer.colors[0];
        frameImages.push_back(addTexture(color.sampler, color.view, vk::ImageLayout::eShaderReadOnlyOptimal));
    }
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
        pipelineBuilder.loadShader(fe::getAssetPath() + "/shaders/base/uioverlay.vert.spv", vk::ShaderStageFlagBits::eVertex);
        pipelineBuilder.loadShader(fe::getAssetPath() + "/shaders/base/uioverlay.frag.spv", vk::ShaderStageFlagBits::eFragment);
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

/** Set the window events callbacks */
void UIOverlay::prepareEvents() {
    assert(createInfo.window);

    createInfo.window->FramebufferEvent.connect([](const glm::ivec2& size) {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2{static_cast<float>(size.x), static_cast<float>(size.y)};
    });

    createInfo.window->MouseButtonEvent.connect([](MouseData data) {
        if (data.button >= 0 && data.button < ImGuiMouseButton_COUNT) {
            ImGuiIO& io = ImGui::GetIO();
            io.AddMouseButtonEvent(data.button, data.action == Action::Press);
        }
    });

    createInfo.window->MouseMotionEvent.connect([](const glm::vec2& pos) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMousePosEvent(pos.x, pos.y);
    });

    createInfo.window->MouseScrollEvent.connect([](const glm::vec2& offset) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseWheelEvent(offset.x, offset.y);
    });

    createInfo.window->KeyEvent.connect([](KeyData data) {
        if (data.action >= Action::Repeat)
            return;

        //UpdateKeyModifiers(data.mods);

        int keycode = TranslateUntranslatedKey(data.key, data.scancode);

        ImGuiIO& io = ImGui::GetIO();
        ImGuiKey imgui_key = KeyToImGuiKey(keycode);
        io.AddKeyEvent(imgui_key, data.action == Action::Press);
        io.SetKeyEventNativeData(imgui_key, keycode,  data.scancode); // To support legacy indexing (<1.87 user code)
    });

    createInfo.window->CharInputEvent.connect([](uint32_t c) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(c);
    });

    createInfo.window->FocusEvent.connect([](bool focuses) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddFocusEvent(focuses);
    });

    // Workaround: X11 seems to send spurious Leave/Enter events which would make us lose our position,
    // so we back it up and restore on Leave/Enter (see https://github.com/ocornut/imgui/issues/4984)
    createInfo.window->MouseEnterEvent.connect([&](bool entered) {
        ImGuiIO& io = ImGui::GetIO();
        if (entered) {
            currentWindow = createInfo.window;
            io.AddMousePosEvent(lastValidMousePos.x, lastValidMousePos.y);
        } else if (currentWindow == createInfo.window) {
            lastValidMousePos = io.MousePos;
            currentWindow = nullptr;
            io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
        }
    });
}

/** Update the command buffers to reflect UI changes */
void UIOverlay::draw(const vk::CommandBuffer& commandBuffer) {
    ImDrawData* imDrawData = ImGui::GetDrawData();
    int32_t vertexOffset = 0;
    int32_t indexOffset = 0;

    if (!imDrawData || imDrawData->CmdListsCount == 0) {
        return;
    }

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

    ImGuiIO& io = ImGui::GetIO();
    pushConstBlock.scale = glm::vec2{2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y};
    pushConstBlock.translate = glm::vec2{-1.0f};
    commandBuffer.pushConstants<PushConstBlock>(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, pushConstBlock);

    commandBuffer.bindVertexBuffers(0, vertexBuffer.buffer, { 0 });
    commandBuffer.bindIndexBuffer(indexBuffer.buffer, 0, sizeof(ImDrawIdx) == 2 ? vk::IndexType::eUint16 : vk::IndexType::eUint32);

    for (int32_t i = 0; i < imDrawData->CmdListsCount; i++) {
        const ImDrawList* cmd_list = imDrawData->CmdLists[i];
        for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++) {
            const ImDrawCmd& pcmd = cmd_list->CmdBuffer[j];
            vk::Rect2D scissorRect;
            scissorRect.offset.x = std::max(static_cast<int32_t>((pcmd.ClipRect.x)), 0);
            scissorRect.offset.y = std::max(static_cast<int32_t>((pcmd.ClipRect.y)), 0);
            scissorRect.extent.width = static_cast<uint32_t>(pcmd.ClipRect.z - pcmd.ClipRect.x);
            scissorRect.extent.height = static_cast<uint32_t>((pcmd.ClipRect.w - pcmd.ClipRect.y));
            commandBuffer.setScissor(0, scissorRect);

            // Bind DescriptorSet with font or user texture
            vk::DescriptorSet descriptor[1] = { vk::DescriptorSet(static_cast<VkDescriptorSet>(pcmd.TextureId)) };
            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, descriptor, 0, nullptr);

            commandBuffer.drawIndexed(pcmd.ElemCount, 1, indexOffset, vertexOffset, 0);

            indexOffset += pcmd.ElemCount;
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
            vertexBuffer.destroy();
        }
		if (vertexCount) {
			vertexBuffer = context.createBuffer(vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eHostVisible, vertexBufferSize);
			vertexBuffer.map();
			updateCmdBuffers = true;
		}
    }

    // Index buffer
    if (!indexBuffer || (indexCount != imDrawData->TotalIdxCount)) {
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
    auto* vtxDst = reinterpret_cast<ImDrawVert*>(vertexBuffer.mapped);
    auto* idxDst = reinterpret_cast<ImDrawIdx*>(indexBuffer.mapped);

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

#if defined GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void UIOverlay::UpdateKeyModifiers(int mods) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(ImGuiKey_ModCtrl, (mods & GLFW_MOD_CONTROL) != 0);
    io.AddKeyEvent(ImGuiKey_ModShift, (mods & GLFW_MOD_SHIFT) != 0);
    io.AddKeyEvent(ImGuiKey_ModAlt, (mods & GLFW_MOD_ALT) != 0);
    io.AddKeyEvent(ImGuiKey_ModSuper, (mods & GLFW_MOD_SUPER) != 0);
}

int UIOverlay::TranslateUntranslatedKey(int key, int scancode) {
#if GLFW_HAS_GET_KEY_NAME && !defined(__EMSCRIPTEN__)
    // GLFW 3.1+ attempts to "untranslate" keys, which goes the opposite of what every other framework does, making using lettered shortcuts difficult.
    // (It had reasons to do so: namely GLFW is/was more likely to be used for WASD-type game controls rather than lettered shortcuts, but IHMO the 3.1 change could have been done differently)
    // See https://github.com/glfw/glfw/issues/1502 for details.
    // Adding a workaround to undo this (so our keys are translated->untranslated->translated, likely a lossy process).
    // This won't cover edge cases but this is at least going to cover common cases.
    if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_EQUAL)
        return key;
    const char* key_name = glfwGetKeyName(key, scancode);
    if (key_name && key_name[0] != 0 && key_name[1] == 0)
    {
        const char char_names[] = "`-=[]\\,;\'./";
        const int char_keys[] = { GLFW_KEY_GRAVE_ACCENT, GLFW_KEY_MINUS, GLFW_KEY_EQUAL, GLFW_KEY_LEFT_BRACKET, GLFW_KEY_RIGHT_BRACKET, GLFW_KEY_BACKSLASH, GLFW_KEY_COMMA, GLFW_KEY_SEMICOLON, GLFW_KEY_APOSTROPHE, GLFW_KEY_PERIOD, GLFW_KEY_SLASH, 0 };
        IM_ASSERT(IM_ARRAYSIZE(char_names) == IM_ARRAYSIZE(char_keys));
        if (key_name[0] >= '0' && key_name[0] <= '9')               { key = GLFW_KEY_0 + (key_name[0] - '0'); }
        else if (key_name[0] >= 'A' && key_name[0] <= 'Z')          { key = GLFW_KEY_A + (key_name[0] - 'A'); }
        else if (const char* p = strchr(char_names, key_name[0]))   { key = char_keys[p - char_names]; }
    }
    // if (action == GLFW_PRESS) printf("key %d scancode %d name '%s'\n", key, scancode, key_name);
#else
    IM_UNUSED(scancode);
#endif
    return key;
}

ImGuiKey UIOverlay::KeyToImGuiKey(int key) {
    switch (key)
    {
        case GLFW_KEY_TAB: return ImGuiKey_Tab;
        case GLFW_KEY_LEFT: return ImGuiKey_LeftArrow;
        case GLFW_KEY_RIGHT: return ImGuiKey_RightArrow;
        case GLFW_KEY_UP: return ImGuiKey_UpArrow;
        case GLFW_KEY_DOWN: return ImGuiKey_DownArrow;
        case GLFW_KEY_PAGE_UP: return ImGuiKey_PageUp;
        case GLFW_KEY_PAGE_DOWN: return ImGuiKey_PageDown;
        case GLFW_KEY_HOME: return ImGuiKey_Home;
        case GLFW_KEY_END: return ImGuiKey_End;
        case GLFW_KEY_INSERT: return ImGuiKey_Insert;
        case GLFW_KEY_DELETE: return ImGuiKey_Delete;
        case GLFW_KEY_BACKSPACE: return ImGuiKey_Backspace;
        case GLFW_KEY_SPACE: return ImGuiKey_Space;
        case GLFW_KEY_ENTER: return ImGuiKey_Enter;
        case GLFW_KEY_ESCAPE: return ImGuiKey_Escape;
        case GLFW_KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
        case GLFW_KEY_COMMA: return ImGuiKey_Comma;
        case GLFW_KEY_MINUS: return ImGuiKey_Minus;
        case GLFW_KEY_PERIOD: return ImGuiKey_Period;
        case GLFW_KEY_SLASH: return ImGuiKey_Slash;
        case GLFW_KEY_SEMICOLON: return ImGuiKey_Semicolon;
        case GLFW_KEY_EQUAL: return ImGuiKey_Equal;
        case GLFW_KEY_LEFT_BRACKET: return ImGuiKey_LeftBracket;
        case GLFW_KEY_BACKSLASH: return ImGuiKey_Backslash;
        case GLFW_KEY_RIGHT_BRACKET: return ImGuiKey_RightBracket;
        case GLFW_KEY_GRAVE_ACCENT: return ImGuiKey_GraveAccent;
        case GLFW_KEY_CAPS_LOCK: return ImGuiKey_CapsLock;
        case GLFW_KEY_SCROLL_LOCK: return ImGuiKey_ScrollLock;
        case GLFW_KEY_NUM_LOCK: return ImGuiKey_NumLock;
        case GLFW_KEY_PRINT_SCREEN: return ImGuiKey_PrintScreen;
        case GLFW_KEY_PAUSE: return ImGuiKey_Pause;
        case GLFW_KEY_KP_0: return ImGuiKey_Keypad0;
        case GLFW_KEY_KP_1: return ImGuiKey_Keypad1;
        case GLFW_KEY_KP_2: return ImGuiKey_Keypad2;
        case GLFW_KEY_KP_3: return ImGuiKey_Keypad3;
        case GLFW_KEY_KP_4: return ImGuiKey_Keypad4;
        case GLFW_KEY_KP_5: return ImGuiKey_Keypad5;
        case GLFW_KEY_KP_6: return ImGuiKey_Keypad6;
        case GLFW_KEY_KP_7: return ImGuiKey_Keypad7;
        case GLFW_KEY_KP_8: return ImGuiKey_Keypad8;
        case GLFW_KEY_KP_9: return ImGuiKey_Keypad9;
        case GLFW_KEY_KP_DECIMAL: return ImGuiKey_KeypadDecimal;
        case GLFW_KEY_KP_DIVIDE: return ImGuiKey_KeypadDivide;
        case GLFW_KEY_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
        case GLFW_KEY_KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
        case GLFW_KEY_KP_ADD: return ImGuiKey_KeypadAdd;
        case GLFW_KEY_KP_ENTER: return ImGuiKey_KeypadEnter;
        case GLFW_KEY_KP_EQUAL: return ImGuiKey_KeypadEqual;
        case GLFW_KEY_LEFT_SHIFT: return ImGuiKey_LeftShift;
        case GLFW_KEY_LEFT_CONTROL: return ImGuiKey_LeftCtrl;
        case GLFW_KEY_LEFT_ALT: return ImGuiKey_LeftAlt;
        case GLFW_KEY_LEFT_SUPER: return ImGuiKey_LeftSuper;
        case GLFW_KEY_RIGHT_SHIFT: return ImGuiKey_RightShift;
        case GLFW_KEY_RIGHT_CONTROL: return ImGuiKey_RightCtrl;
        case GLFW_KEY_RIGHT_ALT: return ImGuiKey_RightAlt;
        case GLFW_KEY_RIGHT_SUPER: return ImGuiKey_RightSuper;
        case GLFW_KEY_MENU: return ImGuiKey_Menu;
        case GLFW_KEY_0: return ImGuiKey_0;
        case GLFW_KEY_1: return ImGuiKey_1;
        case GLFW_KEY_2: return ImGuiKey_2;
        case GLFW_KEY_3: return ImGuiKey_3;
        case GLFW_KEY_4: return ImGuiKey_4;
        case GLFW_KEY_5: return ImGuiKey_5;
        case GLFW_KEY_6: return ImGuiKey_6;
        case GLFW_KEY_7: return ImGuiKey_7;
        case GLFW_KEY_8: return ImGuiKey_8;
        case GLFW_KEY_9: return ImGuiKey_9;
        case GLFW_KEY_A: return ImGuiKey_A;
        case GLFW_KEY_B: return ImGuiKey_B;
        case GLFW_KEY_C: return ImGuiKey_C;
        case GLFW_KEY_D: return ImGuiKey_D;
        case GLFW_KEY_E: return ImGuiKey_E;
        case GLFW_KEY_F: return ImGuiKey_F;
        case GLFW_KEY_G: return ImGuiKey_G;
        case GLFW_KEY_H: return ImGuiKey_H;
        case GLFW_KEY_I: return ImGuiKey_I;
        case GLFW_KEY_J: return ImGuiKey_J;
        case GLFW_KEY_K: return ImGuiKey_K;
        case GLFW_KEY_L: return ImGuiKey_L;
        case GLFW_KEY_M: return ImGuiKey_M;
        case GLFW_KEY_N: return ImGuiKey_N;
        case GLFW_KEY_O: return ImGuiKey_O;
        case GLFW_KEY_P: return ImGuiKey_P;
        case GLFW_KEY_Q: return ImGuiKey_Q;
        case GLFW_KEY_R: return ImGuiKey_R;
        case GLFW_KEY_S: return ImGuiKey_S;
        case GLFW_KEY_T: return ImGuiKey_T;
        case GLFW_KEY_U: return ImGuiKey_U;
        case GLFW_KEY_V: return ImGuiKey_V;
        case GLFW_KEY_W: return ImGuiKey_W;
        case GLFW_KEY_X: return ImGuiKey_X;
        case GLFW_KEY_Y: return ImGuiKey_Y;
        case GLFW_KEY_Z: return ImGuiKey_Z;
        case GLFW_KEY_F1: return ImGuiKey_F1;
        case GLFW_KEY_F2: return ImGuiKey_F2;
        case GLFW_KEY_F3: return ImGuiKey_F3;
        case GLFW_KEY_F4: return ImGuiKey_F4;
        case GLFW_KEY_F5: return ImGuiKey_F5;
        case GLFW_KEY_F6: return ImGuiKey_F6;
        case GLFW_KEY_F7: return ImGuiKey_F7;
        case GLFW_KEY_F8: return ImGuiKey_F8;
        case GLFW_KEY_F9: return ImGuiKey_F9;
        case GLFW_KEY_F10: return ImGuiKey_F10;
        case GLFW_KEY_F11: return ImGuiKey_F11;
        case GLFW_KEY_F12: return ImGuiKey_F12;
        default: return ImGuiKey_None;
    }
}

#endif