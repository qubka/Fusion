#include "imgui_subrender.hpp"

#include "fusion/core/time.hpp"
#include "fusion/devices/devices.hpp"
#include "fusion/filesystem/file_system.hpp"
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
    , pipeline{pipelineStage, {"shaders/imgui/imgui.vert", "shaders/imgui/imgui.frag"}, {{{Vertex::Component::Position2, Vertex::Component::UV, Vertex::Component::RGBA}}}, {}
    , PipelineGraphics::Mode::Polygon, PipelineGraphics::Depth::None, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE}
    , descriptorSet{pipeline} {
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
    // Sets flags
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    SetStyleColors();

    // Dimensions
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
    // Read fonts from memory

    // Text font
    std::vector<uint8_t> textFont { FileSystem::ReadBytes("fonts/PT Sans.ttf") };
    io.Fonts->AddFontFromMemoryTTF(textFont.data(), static_cast<int>(textFont.size()), 16.0f * scale, nullptr, io.Fonts->GetGlyphRangesCyrillic());

    // Icon font
    std::vector<uint8_t> iconFont { FileSystem::ReadBytes("fonts/fontawesome-webfont.ttf") };
    io.Fonts->AddFontFromMemoryTTF(iconFont.data(), static_cast<int>(iconFont.size()), 16.0f * scale, &config, iconsRanges);

    // Generate font
    io.Fonts->Build();

    // Create font texture
    uint8_t* fontBuffer;
    int texWidth, texHeight;
    io.Fonts->GetTexDataAsRGBA32(&fontBuffer, &texWidth, &texHeight);
    auto bitmap = std::make_unique<Bitmap>(glm::uvec2{texWidth, texHeight});
    memcpy(bitmap->getData<void>(), fontBuffer, bitmap->getLength());
    font = std::make_unique<Image2d>(std::move(bitmap));
    io.Fonts->SetTexID((ImTextureID)(&*font));

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
    io.DeltaTime = Time::DeltaTime();

    auto& size = Devices::Get()->getWindow(0)->getSize();
    io.DisplaySize = ImVec2{static_cast<float>(size.x), static_cast<float>(size.y)};

    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    onImGui.publish();

    ImGui::Render();

    updateBuffers();

    drawFrame(commandBuffer);
}

void ImGuiSubrender::drawFrame(const CommandBuffer& commandBuffer) {
    // Update vertex and index buffer containing the imGui elements when required
    ImDrawData* drawData = ImGui::GetDrawData();
    if (!drawData || drawData->CmdListsCount == 0)
        return;

    // Update push constants
    ImGuiIO& io = ImGui::GetIO();
    pushObject.push("scale", glm::vec2{ 2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y });
    pushObject.push("translate", glm::vec2{ -1.0f });

    // Updates descriptors
    descriptorSet.push("PushObject", pushObject);
    descriptorSet.push("fontSampler", font.get());

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the objects
    pipeline.bindPipeline(commandBuffer);
    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    pushObject.bindPush(commandBuffer, pipeline);

    VkBuffer vertexBuffers[1] = { *vertexBuffer };
    VkDeviceSize offsets[1] = { 0 };

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
            // Bind DescriptorSet with font or user texture
            //VkDescriptorSet descriptor[1] = { static_cast<VkDescriptorSet>(pcmd.TextureId) };
            //vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipelineLayout(), 0, 1, descriptor, 0, nullptr);
            vkCmdDrawIndexed(commandBuffer, cmd.ElemCount, 1, indexOffset, vertexOffset, 0);
            indexOffset += static_cast<int32_t>(cmd.ElemCount);
        }
        vertexOffset += cmdLists->VtxBuffer.Size;
    }
}

void ImGuiSubrender::updateBuffers() {
    // Update vertex and index buffer containing the imGui elements when required
    ImDrawData* drawData = ImGui::GetDrawData();
    if (!drawData)
        return;

    // Note: Alignment is done inside buffer creation
    VkDeviceSize vertexBufferSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
    VkDeviceSize indexBufferSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);

    // Update buffers only if vertex or index count has been changed compared to current buffer size
    if (vertexBufferSize == 0 || indexBufferSize == 0)
        return;

    // Vertex buffer
    if (!vertexBuffer || (vertexCount != drawData->TotalVtxCount)) {
        if (vertexBuffer) {
            removePool.push(std::move(vertexBuffer));
        }
        vertexBuffer = std::make_unique<Buffer>(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        vertexBuffer->map();
        vertexCount = drawData->TotalVtxCount;
    }

    // Index buffer
    if (!indexBuffer || (indexCount != drawData->TotalIdxCount)) {
        if (indexBuffer) {
            removePool.push(std::move(indexBuffer));
        }
        indexBuffer = std::make_unique<Buffer>(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        indexBuffer->map();
        indexCount = drawData->TotalIdxCount;
    }

    // Upload data
    auto vtxDst = reinterpret_cast<ImDrawVert*>(vertexBuffer->getMappedMemory());
    auto idxDst = reinterpret_cast<ImDrawIdx*>(indexBuffer->getMappedMemory());

    for (int i = 0; i < drawData->CmdListsCount; i++) {
        const ImDrawList* cmdList = drawData->CmdLists[i];
        memcpy(vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtxDst += cmdList->VtxBuffer.Size;
        idxDst += cmdList->IdxBuffer.Size;
    }

    // Flush to make writes visible to GPU
    vertexBuffer->flush();
    indexBuffer->flush();

    // Remove unused buffers from pool
    //static uint32_t imageCount = Graphics::Get()->getSwapchain(0)->getImageCount() * 2;
    while (removePool.size() > MAX_FRAMES_IN_FLIGHT * 2) {
        removePool.pop();
    }
}

void ImGuiSubrender::onMouseButtonEvent(MouseButton button, InputAction action, bitmask::bitmask<InputMod> mods) {
    if (button >= MouseButton::Button0 && button < MouseButton::Button5) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(static_cast<int>(button), action == InputAction::Press);
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

void ImGuiSubrender::onKeyEvent(Key key, InputAction action, Key scan, bitmask::bitmask<InputMod> mods) {
    if (action >= InputAction::Repeat)
        return;

    //UpdateKeyModifiers(data.mods);

    int scancode = static_cast<int>(scan);
    int keycode = TranslateUntranslatedKey(static_cast<int>(key), scancode);

    ImGuiIO& io = ImGui::GetIO();
    ImGuiKey imKey = KeyToImGuiKey(keycode);
    io.AddKeyEvent(imKey, action == InputAction::Press);
    io.SetKeyEventNativeData(imKey, keycode, scancode); // To support legacy indexing (<1.87 user code)
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

//_______________________________________________________________

void ImGuiSubrender::SetStyleColors() {
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = { 0.1f, 0.105f, 0.11f, 1.0f };

    colors[ImGuiCol_Header] = { 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = { 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_HeaderActive] = { 0.15f, 0.1505f, 0.151f, 1.0f };

    colors[ImGuiCol_Button] = { 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = { 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_ButtonActive] = { 0.15f, 0.1505f, 0.151f, 1.0f };

    colors[ImGuiCol_FrameBg] = { 0.2f, 0.205f, 0.21f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = { 0.3f, 0.305f, 0.31f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = { 0.15f, 0.1505f, 0.151f, 1.0f };

    colors[ImGuiCol_Tab] = { 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabHovered] = { 0.38f, 0.3805f, 0.381f, 1.0f };
    colors[ImGuiCol_TabActive] = { 0.28f, 0.2805f, 0.281f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = { 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = { 0.2f, 0.205f, 0.21f, 1.0f };

    colors[ImGuiCol_TitleBg] = { 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = { 0.15f, 0.1505f, 0.151f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = { 0.15f, 0.1505f, 0.151f, 1.0 };
}

void ImGuiSubrender::UpdateKeyModifiers(int mods) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(ImGuiKey_ModCtrl, (mods & GLFW_MOD_CONTROL) != 0);
    io.AddKeyEvent(ImGuiKey_ModShift, (mods & GLFW_MOD_SHIFT) != 0);
    io.AddKeyEvent(ImGuiKey_ModAlt, (mods & GLFW_MOD_ALT) != 0);
    io.AddKeyEvent(ImGuiKey_ModSuper, (mods & GLFW_MOD_SUPER) != 0);
}

int ImGuiSubrender::TranslateUntranslatedKey(int key, int scancode) {
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

int ImGuiSubrender::KeyToImGuiKey(int key) {
    switch (key) {
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