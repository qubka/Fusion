#include "imgui_subrender.hpp"
#include "imgui.hpp"

#include "fusion/devices/device_manager.hpp"
#include "fusion/utils/file.hpp"
#include "fusion/bitmaps/bitmap.hpp"

#include <imgui/imgui.h>
#include <imguizmo/ImGuizmo.h>
#include <GLFW/glfw3.h>

using namespace fe;

ImguiSubrender::ImguiSubrender(const Pipeline::Stage& pipelineStage)
    : Subrender{pipelineStage}
    , pipeline{pipelineStage, {"shaders/imgui/imgui.vert", "shaders/imgui/imgui.frag"}, {VertexImgui::GetVertexInput()}, {},
               PipelineGraphics::Mode::Polygon, PipelineGraphics::Depth::None, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT}
{
    ImGui::SetCurrentContext(ImGui::CreateContext());

    float scale = 1.0f;
#if PLATFORM_ANDROID
    // Screen density
    if (android::screenDensity >= ACONFIGURATION_DENSITY_XXXHIGH) {
        scale = 4.5f;
    } else if (android::screenDensity >= ACONFIGURATION_DENSITY_XXHIGH) {
        scale = 3.5f;
    } else if (android::screenDensity >= ACONFIGURATION_DENSITY_XHIGH) {
        scale = 2.5f;
    } else if (android::screenDensity >= ACONFIGURATION_DENSITY_HIGH) {
        scale = 2.0f;
    };
    LOG_DEBUG << "Android UI scale "<< scale;
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
    auto& size = Devices::Get()->getWindow(0)->getSize();
    io.DisplaySize = ImVec2{static_cast<float>(size.x), static_cast<float>(size.y)};
    io.FontGlobalScale = scale;

    // Add character ranges and merge into the previous font
    // The ranges array is not copied by the AddFont* functions and is used lazily
    // so ensure it is available at the time of building or calling GetTexDataAsRGBA32().
    static const ImWchar iconsRanges[] = { 0xf000, 0xf3ff, 0 }; // Will not be copied by AddFont* so keep in scope.
    ImFontConfig config;
    config.MergeMode = true;

#if PLATFORM_ANDROID
    scale = static_cast<float>(android::screenDensity) / static_cast<float>(ACONFIGURATION_DENSITY_MEDIUM);
#endif
    File::Read("fonts/PT Sans.ttf", [&](size_t size, const void* data) {
        io.Fonts->AddFontFromMemoryTTF(const_cast<void*>(data), size, 16.0f * scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    });
    File::Read("fonts/fontawesome-webfont.ttf", [&](size_t size, const void* data) {
        io.Fonts->AddFontFromMemoryTTF(const_cast<void*>(data), size, 16.0f * scale, &config, iconsRanges);
    });

    // Generate font
    io.Fonts->Build();

    // Create font texture
    uint8_t* fontBuffer;
    int texWidth, texHeight;
    io.Fonts->GetTexDataAsRGBA32(&fontBuffer, &texWidth, &texHeight);

    auto bitmap = std::make_unique<Bitmap>(glm::uvec2{texWidth, texHeight});
    memcpy(bitmap->getData<void>(), fontBuffer, bitmap->getLength());

    font = std::make_unique<Image2d>(std::move(bitmap));

    // Descriptor set
    //ImTextureID fontDescriptorSet = addTexture(font.sampler, font.view, vk::ImageLayout::eShaderReadOnlyOptimal);
    //io.Fonts->SetTexID(fontDescriptorSet);
}

/** Update the command buffers to reflect UI changes */
void ImguiSubrender::render(const CommandBuffer& commandBuffer) {
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = 0.15f; // TODO:

    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    {
        //ImGui::ShowDemoWindow();
        // Note: Switch this to true to enable dockspace
        static bool opt_fullscreen_persistant = true;
        static ImGuiDockNodeFlags dockspace_flags = 0;//ImGuiDockNodeFlags_PassthruCentralNode;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen_persistant) {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
            window_flags |= ImGuiWindowFlags_NoBackground;
        }

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen_persistant)
            ImGui::PopStyleVar(2);

        // DockSpace
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace = ImGui::GetID("HUB_DockSpace");
            ImGui::DockSpace(dockspace, { 0, 0 }, dockspace_flags);
        }

        ImGui::Begin("Test sds");

        ImGui::TextUnformatted("Some text");
        ImGui::Button("Some text");

        ImGui::End();

        ImGui::End();
    }

    ImGui::Render();
    updateBuffers();

    ImDrawData* imDrawData = ImGui::GetDrawData();
    if (!imDrawData || imDrawData->CmdListsCount == 0)
        return;

    // Update push constants.
    pushObject.push("scale", glm::vec2{ 2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y });
    pushObject.push("translate", glm::vec2{ -1.0f });

    // Updates descriptors.
    descriptorSet.push("PushObject", pushObject);
    descriptorSet.push("fontSampler", font.get());

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the object.
    pipeline.bindPipeline(commandBuffer);

    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    pushObject.bindPush(commandBuffer, pipeline);

    static const VkDeviceSize offsets[1] = { 0 };
    VkBuffer vertexBuffers[1] = { *vertexBuffer };

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, *indexBuffer, 0, sizeof(ImDrawIdx) == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);

    int32_t vertexOffset = 0;
    int32_t indexOffset = 0;

    for (int32_t i = 0; i < imDrawData->CmdListsCount; i++) {
        const ImDrawList* cmdLists = imDrawData->CmdLists[i];
        for (const auto& cmd : cmdLists->CmdBuffer) {
            VkRect2D scissor = vku::rect2D(
                    glm::uvec2{cmd.ClipRect.z - cmd.ClipRect.x, cmd.ClipRect.w - cmd.ClipRect.y}, // extent
                    glm::ivec2{std::max(static_cast<int>((cmd.ClipRect.x)), 0), std::max(static_cast<int>((cmd.ClipRect.y)), 0)} // offset
            );
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

            // Bind DescriptorSet with font or user texture
            //VkDescriptorSet descriptor[1] = { static_cast<VkDescriptorSet>(pcmd.TextureId) };
            //vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipelineLayout(), 0, 1, descriptor, 0, nullptr);

            vkCmdDrawIndexed(commandBuffer, cmd.ElemCount, 1, indexOffset, vertexOffset, 0);

            indexOffset += static_cast<int32_t>(cmd.ElemCount);
        }
        vertexOffset += cmdLists->VtxBuffer.Size;
    }
}

/** Update vertex and index buffer containing the imGui elements when required */
bool ImguiSubrender::updateBuffers() {
    ImDrawData* imDrawData = ImGui::GetDrawData();
    if (!imDrawData)
        return false;

    // Note: Alignment is done inside buffer creation.
    VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
    VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

    // Update buffers only if vertex or index count has been changed compared to current buffer size
    if (vertexBufferSize == 0 || indexBufferSize == 0)
        return false;

    // Update buffers only if vertex or index count has been changed compared to current buffer size.
    bool updateCmdBuffers = false;

    // Vertex buffer.
    if (!vertexBuffer || (vertexCount != imDrawData->TotalVtxCount)) {
        vertexBuffer = std::make_unique<Buffer>(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        vertexBuffer->map();
        vertexCount = imDrawData->TotalVtxCount;
        updateCmdBuffers = true;
    }

    // Index buffer.
    if (!indexBuffer || (indexCount != imDrawData->TotalIdxCount)) {
        indexBuffer = std::make_unique<Buffer>(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        indexBuffer->map();
        indexCount = imDrawData->TotalIdxCount;
        updateCmdBuffers = true;
    }

    // Upload data.
    auto vtxDst = reinterpret_cast<ImDrawVert*>(vertexBuffer->getMappedMemory());
    auto idxDst = reinterpret_cast<ImDrawIdx*>(indexBuffer->getMappedMemory());

    for (int n = 0; n < imDrawData->CmdListsCount; n++) {
        const ImDrawList* cmdList = imDrawData->CmdLists[n];
        memcpy(vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtxDst += cmdList->VtxBuffer.Size;
        idxDst += cmdList->IdxBuffer.Size;
    }

    // Flush to make writes visible to GPU.
    vertexBuffer->flush();
    indexBuffer->flush();

    return updateCmdBuffers;
}

void ImguiSubrender::setStyleColors() {
    // Color scheme
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = { 0.1f, 0.105f, 0.11f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = { 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = { 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_HeaderActive] = { 0.15f, 0.1505f, 0.151f, 1.0f };

    // Buttons
    colors[ImGuiCol_Button] = { 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = { 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_ButtonActive] = { 0.15f, 0.1505f, 0.151f, 1.0f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = { 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = { 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = { 0.15f, 0.1505f, 0.151f, 1.0f };

    // Tabs
    colors[ImGuiCol_Tab] = { 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabHovered] = { 0.38f, 0.3805f, 0.381f, 1.0f };
    colors[ImGuiCol_TabActive] = { 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = { 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = { 0.2f, 0.205f, 0.21f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = { 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = { 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = { 0.15f, 0.1505f, 0.151f, 1.0 };
}