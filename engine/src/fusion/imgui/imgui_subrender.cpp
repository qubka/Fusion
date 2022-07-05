#include "imgui_subrender.hpp"
#include "imgui_utilities.hpp"

#include "fusion/core/engine.hpp"
#include "fusion/core/time.hpp"
#include "fusion/devices/devices.hpp"
#include "fusion/filesystem/file_system.hpp"
#include "fusion/bitmaps/bitmap.hpp"
#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/commands/command_buffer.hpp"
#include "fusion/input/codes.hpp"

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

    LOG_INFO << "ImGui Version: " << IMGUI_VERSION;
#ifdef IMGUI_USER_CONFIG
    LOG_INFO << "ImConfig File: " << std::quoted(IMGUI_USER_CONFIG);
#endif

    fontSize = 14.0f;

#if PLATFORM_ANDROID
    // Screen density
    if (android::screenDensity >= ACONFIGURATION_DENSITY_XXXHIGH) {
        fontScale = 4.5f;
    } else if (android::screenDensity >= ACONFIGURATION_DENSITY_XXHIGH) {
        fontScale = 3.5f;
    } else if (android::screenDensity >= ACONFIGURATION_DENSITY_XHIGH) {
        fontScale = 2.5f;
    } else if (android::screenDensity >= ACONFIGURATION_DENSITY_HIGH) {
        fontScale = 2.0f;
    };
    LOG_DEBUG << "Android UI scale "<< fontScale;
#elif PLATFORM_IOS
    fontScale = 2.0f;
#else
    fontScale = 1.0f;
#endif

    // Sets flags
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
#ifdef PLATFORM_ANDROID
    io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
#endif
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    window = Devices::Get()->getWindow(0);
    auto& size = window->getSize();
    io.DisplaySize = ImVec2{static_cast<float>(size.x), static_cast<float>(size.y)};

    setupKeyCodes();
    setupStyle();

    setupEvents(true);

    io.ClipboardUserData = window;
    io.SetClipboardTextFn = SetClipboardText;
    io.GetClipboardTextFn = GetClipboardText;
}

ImGuiSubrender::~ImGuiSubrender() {
    if (window == nullptr)
        return;

    setupEvents(false);
}

void ImGuiSubrender::onUpdate() {
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = Time::DeltaTime();

    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    Engine::Get()->getApp()->onImGui();

    ImGui::Render();
}

void ImGuiSubrender::onRender(const CommandBuffer& commandBuffer) {
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
    descriptorSet.push("fontSampler", canvasObject.getFont());

    if (!descriptorSet.update(pipeline))
        return;

    // Draws the canvas
    pipeline.bindPipeline(commandBuffer);
    descriptorSet.bindDescriptor(commandBuffer, pipeline);
    pushObject.bindPush(commandBuffer, pipeline);

    canvasObject.cmdRender(commandBuffer);
}

void ImGuiSubrender::onMouseButtonEvent(MouseButton button, InputAction action, bitmask::bitmask<InputMod> mods) {
    if (button >= MouseButton::Button0 && button < MouseButton::Button5) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(static_cast<int>(button), action == InputAction::Press);
    }
}

void ImGuiSubrender::onMouseMotionEvent(const glm::vec2& pos) {
    if (!window->isCursorHidden()) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMousePosEvent(pos.x, pos.y);
    }
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

    ImGuiIO& io = ImGui::GetIO();

    io.KeyCtrl = (mods & InputMod::Control) ? true : false;
    io.KeyShift = (mods & InputMod::Shift) ? true : false;
    io.KeyAlt = (mods & InputMod::Alt) ? true : false;
    io.KeySuper = (mods & InputMod::Super) ? true : false;

    io.AddKeyEvent(KeyToImGuiKey(key), action == InputAction::Press);
}

void ImGuiSubrender::onCharInputEvent(uint32_t chr) {
    if (chr > 0 && chr < 0x10000) {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(chr);
    }
}

void ImGuiSubrender::onFocusEvent(bool focuses) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddFocusEvent(focuses);
}

void ImGuiSubrender::onSizeChange(const glm::uvec2& size) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2{static_cast<float>(size.x), static_cast<float>(size.y)};
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
        window->OnResize().connect<&ImGuiSubrender::onSizeChange>(this);
    } else {
        // Remove the window events callbacks.
        window->OnMouseButton().disconnect<&ImGuiSubrender::onMouseButtonEvent>(this);
        window->OnMouseMotion().disconnect<&ImGuiSubrender::onMouseMotionEvent>(this);
        window->OnMouseScroll().disconnect<&ImGuiSubrender::onMouseScrollEvent>(this);
        window->OnMouseEnter().disconnect<&ImGuiSubrender::onMouseEnterEvent>(this);
        window->OnKey().disconnect<&ImGuiSubrender::onKeyEvent>(this);
        window->OnCharInput().disconnect<&ImGuiSubrender::onCharInputEvent>(this);
        window->OnFocus().disconnect<&ImGuiSubrender::onFocusEvent>(this);
        window->OnResize().disconnect<&ImGuiSubrender::onSizeChange>(this);
    }
}

void ImGuiSubrender::setupKeyCodes() {
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    /*io.KeyMap[ImGuiKey_Tab] = static_cast<int>(Key::Tab);
    io.KeyMap[ImGuiKey_LeftArrow] = static_cast<int>(Key::Left);
    io.KeyMap[ImGuiKey_RightArrow] = static_cast<int>(Key::Right);
    io.KeyMap[ImGuiKey_UpArrow] = static_cast<int>(Key::Up);
    io.KeyMap[ImGuiKey_DownArrow] = static_cast<int>(Key::Down);
    io.KeyMap[ImGuiKey_PageUp] = static_cast<int>(Key::PageUp);
    io.KeyMap[ImGuiKey_PageDown] = static_cast<int>(Key::PageDown);
    io.KeyMap[ImGuiKey_Home] = static_cast<int>(Key::Home);
    io.KeyMap[ImGuiKey_End] = static_cast<int>(Key::End);
    io.KeyMap[ImGuiKey_Insert] = static_cast<int>(Key::Insert);
    io.KeyMap[ImGuiKey_Delete] = static_cast<int>(Key::Delete);
    io.KeyMap[ImGuiKey_Backspace] = static_cast<int>(Key::Backspace);
    io.KeyMap[ImGuiKey_Space] = static_cast<int>(Key::Space);
    io.KeyMap[ImGuiKey_Enter] = static_cast<int>(Key::Enter);
    io.KeyMap[ImGuiKey_Escape] = static_cast<int>(Key::Escape);
    io.KeyMap[ImGuiKey_A] = static_cast<int>(Key::A);
    io.KeyMap[ImGuiKey_C] = static_cast<int>(Key::C);
    io.KeyMap[ImGuiKey_V] = static_cast<int>(Key::V);
    io.KeyMap[ImGuiKey_X] = static_cast<int>(Key::X);
    io.KeyMap[ImGuiKey_Y] = static_cast<int>(Key::Y);
    io.KeyMap[ImGuiKey_Z] = static_cast<int>(Key::Z);*/
    io.KeyRepeatDelay = 0.400f;
    io.KeyRepeatRate = 0.05f;
}

void ImGuiSubrender::setupStyle() {
    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    io.FontGlobalScale = fontScale;

    ImFontConfig config;
    config.MergeMode = false;
    config.PixelSnapH = true;
    config.OversampleH = config.OversampleV = 1;
    config.GlyphMinAdvanceX = 4.0f;
    config.SizePixels = 12.0f;

    static const ImWchar iconsRanges[] = { 0xf000, 0xf3ff, 0 };

    std::vector<uint8_t> textFont = FileSystem::ReadBytes("fonts/PT Sans.ttf");
    io.Fonts->AddFontFromMemoryTTF(textFont.data(), static_cast<int>(textFont.size()), fontSize * fontScale, nullptr, io.Fonts->GetGlyphRangesCyrillic());

    std::vector<uint8_t> iconFont = FileSystem::ReadBytes("fonts/fontawesome-webfont.ttf");
    io.Fonts->AddFontFromMemoryTTF(iconFont.data(), static_cast<int>(iconFont.size()), fontSize * fontScale, &config, iconsRanges);

    io.Fonts->TexGlyphPadding = 1;
    for (int n = 0; n < io.Fonts->ConfigData.Size; n++) {
        io.Fonts->ConfigData[n].RasterizerMultiply = 1.0f;
    }

    io.Fonts->Build();

    uint8_t* fontBuffer;
    int texWidth, texHeight;
    io.Fonts->GetTexDataAsRGBA32(&fontBuffer, &texWidth, &texHeight);
    auto bitmap = std::make_unique<Bitmap>(glm::uvec2{texWidth, texHeight});
    memcpy(bitmap->getData<void>(), fontBuffer, bitmap->getLength());
    canvasObject.setFont(std::make_unique<Image2d>(std::move(bitmap)));
    //io.Fonts->SetTexID((ImTextureID)(&*font));

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowPadding = ImVec2{ 5, 5 };
    style.FramePadding = ImVec2{ 4, 4 };
    style.ItemSpacing = ImVec2{ 6, 2 };
    style.ItemInnerSpacing = ImVec2{ 2, 2 };
    style.IndentSpacing = 6.0f;
    style.TouchExtraPadding = ImVec2{ 4, 4 };

    style.ScrollbarSize = 10;

    style.WindowBorderSize = 0;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 3;
    style.FrameBorderSize = 0.0f;

    const int roundingAmount = 2;
    style.PopupRounding = roundingAmount;
    style.WindowRounding = roundingAmount;
    style.ChildRounding = 0;
    style.FrameRounding = roundingAmount;
    style.ScrollbarRounding = roundingAmount;
    style.GrabRounding = roundingAmount;
    style.WindowMinSize = ImVec2{ 200.0f, 200.0f };

#ifdef IMGUI_HAS_DOCK
    style.TabBorderSize = 1.0f;
    style.TabRounding = roundingAmount; // + 4;

    if(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = roundingAmount;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
#endif

    ImGuiUtilities::SetTheme(ImGuiUtilities::Theme::Dark);
}

const char* ImGuiSubrender::GetClipboardText(void* userData) {
    return reinterpret_cast<Window*>(userData)->getClipboard();
}

void ImGuiSubrender::SetClipboardText(void* userData, const char* text) {
    reinterpret_cast<Window*>(userData)->setClipboard(text);
}

int ImGuiSubrender::KeyToImGuiKey(Key key) {
    switch (key) {
        case Key::Space: return ImGuiKey_Space;
        case Key::Apostrophe: return ImGuiKey_Apostrophe;
        case Key::Comma: return ImGuiKey_Comma;
        case Key::Minus: return ImGuiKey_Minus;
        case Key::Period: return ImGuiKey_Period;
        case Key::Slash: return ImGuiKey_Slash;
        case Key::D0: return ImGuiKey_0;
        case Key::D1: return ImGuiKey_1;
        case Key::D2: return ImGuiKey_2;
        case Key::D3: return ImGuiKey_3;
        case Key::D4: return ImGuiKey_4;
        case Key::D5: return ImGuiKey_5;
        case Key::D6: return ImGuiKey_6;
        case Key::D7: return ImGuiKey_7;
        case Key::D8: return ImGuiKey_8;
        case Key::D9: return ImGuiKey_9;
        case Key::Semicolon: return ImGuiKey_Semicolon;
        case Key::Equal: return ImGuiKey_Equal;
        case Key::A: return ImGuiKey_A;
        case Key::B: return ImGuiKey_B;
        case Key::C: return ImGuiKey_C;
        case Key::D: return ImGuiKey_D;
        case Key::E: return ImGuiKey_E;
        case Key::F: return ImGuiKey_F;
        case Key::G: return ImGuiKey_G;
        case Key::H: return ImGuiKey_H;
        case Key::I: return ImGuiKey_I;
        case Key::J: return ImGuiKey_J;
        case Key::K: return ImGuiKey_K;
        case Key::L: return ImGuiKey_L;
        case Key::M: return ImGuiKey_M;
        case Key::N: return ImGuiKey_N;
        case Key::O: return ImGuiKey_O;
        case Key::P: return ImGuiKey_P;
        case Key::Q: return ImGuiKey_Q;
        case Key::R: return ImGuiKey_R;
        case Key::S: return ImGuiKey_S;
        case Key::T: return ImGuiKey_T;
        case Key::U: return ImGuiKey_U;
        case Key::V: return ImGuiKey_V;
        case Key::W: return ImGuiKey_W;
        case Key::X: return ImGuiKey_X;
        case Key::Y: return ImGuiKey_Y;
        case Key::Z: return ImGuiKey_Z;
        case Key::LeftBracket: return ImGuiKey_LeftBracket;
        case Key::Backslash: return ImGuiKey_Backslash;
        case Key::RightBracket: return ImGuiKey_RightBracket;
        case Key::GraveAccent: return ImGuiKey_GraveAccent;
        //case Key::World1: return ImGuiKey_World1;
        //case Key::World2: return ImGuiKey_World2;
        case Key::Escape: return ImGuiKey_Escape;
        case Key::Enter: return ImGuiKey_Enter;
        case Key::Tab: return ImGuiKey_Tab;
        case Key::Backspace: return ImGuiKey_Backspace;
        case Key::Insert: return ImGuiKey_Insert;
        case Key::Delete: return ImGuiKey_Delete;
        case Key::Right: return ImGuiKey_RightArrow;
        case Key::Left: return ImGuiKey_LeftArrow;
        case Key::Down: return ImGuiKey_DownArrow;
        case Key::Up: return ImGuiKey_UpArrow;
        case Key::PageUp: return ImGuiKey_PageUp;
        case Key::PageDown: return ImGuiKey_PageDown;
        case Key::Home: return ImGuiKey_Home;
        case Key::End: return ImGuiKey_End;
        case Key::CapsLock: return ImGuiKey_CapsLock;
        case Key::ScrollLock: return ImGuiKey_ScrollLock;
        case Key::NumLock: return ImGuiKey_NumLock;
        case Key::PrintScreen: return ImGuiKey_PrintScreen;
        case Key::Pause: return ImGuiKey_Pause;
        case Key::F1: return ImGuiKey_F1;
        case Key::F2: return ImGuiKey_F2;
        case Key::F3: return ImGuiKey_F3;
        case Key::F4: return ImGuiKey_F4;
        case Key::F5: return ImGuiKey_F5;
        case Key::F6: return ImGuiKey_F6;
        case Key::F7: return ImGuiKey_F7;
        case Key::F8: return ImGuiKey_F8;
        case Key::F9: return ImGuiKey_F9;
        case Key::F10: return ImGuiKey_F10;
        case Key::F11: return ImGuiKey_F11;
        case Key::F12: return ImGuiKey_F12;
        /*case Key::F13: return ImGuiKey_F13;
        case Key::F14: return ImGuiKey_F14;
        case Key::F15: return ImGuiKey_F15;
        case Key::F16: return ImGuiKey_F16;
        case Key::F17: return ImGuiKey_F17;
        case Key::F18: return ImGuiKey_F18;
        case Key::F19: return ImGuiKey_F19;
        case Key::F20: return ImGuiKey_F20;
        case Key::F21: return ImGuiKey_F21;
        case Key::F22: return ImGuiKey_F22;
        case Key::F23: return ImGuiKey_F23;
        case Key::F24: return ImGuiKey_F24;
        case Key::F25: return ImGuiKey_F25;*/
        case Key::KP0: return ImGuiKey_Keypad0;
        case Key::KP1: return ImGuiKey_Keypad1;
        case Key::KP2: return ImGuiKey_Keypad2;
        case Key::KP3: return ImGuiKey_Keypad3;
        case Key::KP4: return ImGuiKey_Keypad4;
        case Key::KP5: return ImGuiKey_Keypad5;
        case Key::KP6: return ImGuiKey_Keypad6;
        case Key::KP7: return ImGuiKey_Keypad7;
        case Key::KP8: return ImGuiKey_Keypad8;
        case Key::KP9: return ImGuiKey_Keypad9;
        case Key::KPDecimal: return ImGuiKey_KeypadDecimal;
        case Key::KPDivide: return ImGuiKey_KeypadDivide;
        case Key::KPMultiply: return ImGuiKey_KeypadMultiply;
        case Key::KPSubtract: return ImGuiKey_KeypadSubtract;
        case Key::KPAdd: return ImGuiKey_KeypadAdd;
        case Key::KPEnter: return ImGuiKey_KeypadEnter;
        case Key::KPEqual: return ImGuiKey_KeypadEqual;
        case Key::LeftShift: return ImGuiKey_LeftShift;
        case Key::LeftControl: return ImGuiKey_LeftCtrl;
        case Key::LeftAlt: return ImGuiKey_LeftAlt;
        case Key::LeftSuper: return ImGuiKey_LeftSuper;
        case Key::RightShift: return ImGuiKey_RightShift;
        case Key::RightControl: return ImGuiKey_RightCtrl;
        case Key::RightAlt: return ImGuiKey_RightAlt;
        case Key::RightSuper: return ImGuiKey_RightSuper;
        case Key::Menu: return ImGuiKey_Menu;
    }
    return ImGuiKey_None;
}