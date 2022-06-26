#include "imgui_subrender.hpp"
#include "imgui.hpp"

#include "fusion/devices/devices.hpp"
#include "fusion/utils/file.hpp"
#include "fusion/bitmaps/bitmap.hpp"
#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/images/image2d.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/graphics/buffers/buffer.hpp"

#include <imgui/imgui.h>
#include <imguizmo/ImGuizmo.h>
#include <GLFW/glfw3.h>

using namespace fe;

ImGuiSubrender::ImGuiSubrender(const Pipeline::Stage& pipelineStage)
    : Subrender{pipelineStage}
    , pipeline{pipelineStage, {"shaders/imgui/imgui.vert", "shaders/imgui/imgui.frag"}, {ImGuis::GetVertexInput()}}
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
    // Sets flags.
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

    // Setup Dear ImGui style.
    ImGui::StyleColorsDark();
    ImGuis::SetStyleColors();

    // Dimensions.
    window = Devices::Get()->getWindow(0);
    auto& size = window->getSize();
    io.DisplaySize = ImVec2{static_cast<float>(size.x), static_cast<float>(size.y)};
    io.FontGlobalScale = scale;

    // Add character ranges and merge into the previous font.
    // The ranges array is not copied by the AddFont* functions and is used lazily.
    // so ensure it is available at the time of building or calling GetTexDataAsRGBA32().
    static const ImWchar iconsRanges[] = { 0xf000, 0xf3ff, 0 }; // Will not be copied by AddFont* so keep in scope.
    ImFontConfig config;
    config.MergeMode = true;

#if PLATFORM_ANDROID
    scale = static_cast<float>(android::screenDensity) / static_cast<float>(ACONFIGURATION_DENSITY_MEDIUM);
#endif
    // Read fonts from memory.
    File::Read("fonts/PT Sans.ttf", [&](size_t size, const void* data) {
        io.Fonts->AddFontFromMemoryTTF(const_cast<void*>(data), size, 16.0f * scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());
    });
    File::Read("fonts/fontawesome-webfont.ttf", [&](size_t size, const void* data) {
        io.Fonts->AddFontFromMemoryTTF(const_cast<void*>(data), size, 16.0f * scale, &config, iconsRanges);
    });

    // Generate font.
    io.Fonts->Build();

    // Create font texture.
    uint8_t* fontBuffer;
    int texWidth, texHeight;
    io.Fonts->GetTexDataAsRGBA32(&fontBuffer, &texWidth, &texHeight);
    auto bitmap = std::make_unique<Bitmap>(glm::uvec2{texWidth, texHeight});
    memcpy(bitmap->getData<void>(), fontBuffer, bitmap->getLength());
    font = std::make_unique<Image2d>(std::move(bitmap));
    //io.Fonts->SetTexID(font->getWriteDescriptor());

    setupEvents(true);
}

ImGuiSubrender::~ImGuiSubrender() {
    if (window == nullptr)
        return;

    setupEvents(false);
}

/** Update the command buffers to reflect UI changes */
void ImGuiSubrender::render(const CommandBuffer& commandBuffer) {
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = 0.15f; // TODO:

    auto& size = Devices::Get()->getWindow(0)->getSize();
    io.DisplaySize = ImVec2{static_cast<float>(size.x), static_cast<float>(size.y)};

    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    {
        ImGui::ShowDemoWindow();
    }

    ImGui::Render();

    // Update vertex and index buffer containing the ImGui elements
    updateBuffers();

    drawFrame(commandBuffer);
}

void ImGuiSubrender::drawFrame(const CommandBuffer& commandBuffer) {
    // Update vertex and index buffer containing the imGui elements when required.
    ImDrawData* drawData = ImGui::GetDrawData();
    if (!drawData || drawData->CmdListsCount == 0)
        return;

    // Update push constants.
    ImGuiIO& io = ImGui::GetIO();
    pushObject.push("scale", glm::vec2{ 2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y });
    pushObject.push("translate", glm::vec2{ -1.0f });

    // Updates descriptors.
    descriptorSet.push("PushObject", pushObject);
    descriptorSet.push("fontSampler", font.get());

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the objects.
    pipeline.bindPipeline(commandBuffer);
    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    pushObject.bindPush(commandBuffer, pipeline);

    static const VkDeviceSize offsets[1] = { 0 };
    VkBuffer vertexBuffers[1] = { *vertexBuffer };

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, *indexBuffer, 0, sizeof(ImDrawIdx) == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);

    int32_t vertexOffset = 0;
    int32_t indexOffset = 0;

    for (int i = 0; i < drawData->CmdListsCount; i++) {
        const ImDrawList* cmdLists = drawData->CmdLists[i];
        for (const auto& cmd : cmdLists->CmdBuffer) {
            VkRect2D scissor = vku::rect2D(
                    glm::uvec2{cmd.ClipRect.z - cmd.ClipRect.x, cmd.ClipRect.w - cmd.ClipRect.y}, // extent
                    glm::ivec2{std::max(static_cast<int>((cmd.ClipRect.x)), 0), std::max(static_cast<int>((cmd.ClipRect.y)), 0)} // offset
            );
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
            // Bind DescriptorSet with font or user texture.
            //VkDescriptorSet descriptor[1] = { static_cast<VkDescriptorSet>(pcmd.TextureId) };
            //vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipelineLayout(), 0, 1, descriptor, 0, nullptr);
            vkCmdDrawIndexed(commandBuffer, cmd.ElemCount, 1, indexOffset, vertexOffset, 0);

            indexOffset += static_cast<int32_t>(cmd.ElemCount);
        }
        vertexOffset += cmdLists->VtxBuffer.Size;
    }
}

void ImGuiSubrender::updateBuffers() {
    // Update vertex and index buffer containing the imGui elements when required.
    ImDrawData* drawData = ImGui::GetDrawData();
    if (!drawData)
        return;

    // Note: Alignment is done inside buffer creation.
    VkDeviceSize vertexBufferSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
    VkDeviceSize indexBufferSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);

    // Update buffers only if vertex or index count has been changed compared to current buffer size.
    if (vertexBufferSize == 0 || indexBufferSize == 0)
        return;

    // Update buffers only if vertex or index count has been changed compared to current buffer size.
    //Graphics::Get()->getC;

    // Vertex buffer.
    if (!vertexBuffer || (vertexCount != drawData->TotalVtxCount)) {
        vertexBuffer = std::make_unique<Buffer>(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        vertexBuffer->map();
        vertexCount = drawData->TotalVtxCount;
    }

    // Index buffer.
    if (!indexBuffer || (indexCount != drawData->TotalIdxCount)) {
        indexBuffer = std::make_unique<Buffer>(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        indexBuffer->map();
        indexCount = drawData->TotalIdxCount;
    }

    // Upload data.
    auto vtxDst = reinterpret_cast<ImDrawVert*>(vertexBuffer->getMappedMemory());
    auto idxDst = reinterpret_cast<ImDrawIdx*>(indexBuffer->getMappedMemory());

    for (int i = 0; i < drawData->CmdListsCount; i++) {
        const ImDrawList* cmdList = drawData->CmdLists[i];
        memcpy(vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtxDst += cmdList->VtxBuffer.Size;
        idxDst += cmdList->IdxBuffer.Size;
    }

    // Flush to make writes visible to GPU.
    vertexBuffer->flush();
    indexBuffer->flush();
}

void ImGuiSubrender::onMouseButtonEvent(MouseData data) {
    if (data.button >= 0 && data.button < ImGuiMouseButton_COUNT) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(data.button, data.action == Action::Press);
    }
}

void ImGuiSubrender::onMouseMotionEvent(const glm::vec2& pos) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(pos.x, pos.y);
}

void ImGuiSubrender::onMouseScrollEvent(const glm::vec2& offset) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseWheelEvent(offset.x, offset.y);
}

// Workaround: X11 seems to send spurious Leave/Enter events which would make us lose our position,
// so we back it up and restore on Leave/Enter (see https://github.com/ocornut/imgui/issues/4984)
void ImGuiSubrender::onMouseEnterEvent(bool entered) {
    ImGuiIO& io = ImGui::GetIO();
    if (entered) {
        currentWindow = window;
        io.AddMousePosEvent(lastValidMousePos.x, lastValidMousePos.y);
    } else if (currentWindow == window) {
        const auto& mouse = io.MousePos;
        lastValidMousePos = { mouse.x, mouse.y };
        currentWindow = nullptr;
        io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
    }
}

void ImGuiSubrender::onKeyEvent(KeyData data) {
    if (data.action >= Action::Repeat)
        return;

    //UpdateKeyModifiers(data.mods);

    int keycode = ImGuis::TranslateUntranslatedKey(data.key, data.scancode);

    ImGuiIO& io = ImGui::GetIO();
    ImGuiKey key = ImGuis::KeyToImGuiKey(keycode);
    io.AddKeyEvent(key, data.action == Action::Press);
    io.SetKeyEventNativeData(key, keycode, data.scancode); // To support legacy indexing (<1.87 user code)
}

void ImGuiSubrender::onCharInputEvent(uint32_t chr) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(chr);
}

void ImGuiSubrender::onFocusEvent(bool focuses) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddFocusEvent(focuses);
}

void ImGuiSubrender::setupEvents(bool connect) {
    if (connect) {
        // Set the window events callbacks.
        window->OnMouseButton().connect<&ImGuiSubrender::onMouseButtonEvent>(this);
        window->OnMouseMotion().connect<&ImGuiSubrender::onMouseMotionEvent>(this);
        window->OnMouseScroll().connect<&ImGuiSubrender::onMouseScrollEvent>(this);
        window->OnMouseEnter().connect<&ImGuiSubrender::onMouseEnterEvent>(this);
        window->OnKey().connect<&ImGuiSubrender::onKeyEvent>(this);
        window->OnCharInput().connect<&ImGuiSubrender::onCharInputEvent>(this);
        window->OnFocus().connect<&ImGuiSubrender::onFocusEvent>(this);
    } else {
        // Remove the window events callbacks.
        window->OnMouseButton().disconnect<&ImGuiSubrender::onMouseButtonEvent>(this);
        window->OnMouseMotion().disconnect<&ImGuiSubrender::onMouseMotionEvent>(this);
        window->OnMouseScroll().disconnect<&ImGuiSubrender::onMouseScrollEvent>(this);
        window->OnMouseEnter().disconnect<&ImGuiSubrender::onMouseEnterEvent>(this);
        window->OnKey().disconnect<&ImGuiSubrender::onKeyEvent>(this);
        window->OnCharInput().disconnect<&ImGuiSubrender::onCharInputEvent>(this);
        window->OnFocus().disconnect<&ImGuiSubrender::onFocusEvent>(this);
    }
}