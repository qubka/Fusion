#include <Fusion.hpp>
#include "Fusion/Core/EntryPoint.hpp"

#include <imgui/imgui.h>

using namespace Fusion;

class ExampleLayer : public Layer {
public:
    ExampleLayer() : Layer("ExampleLayer"), info(getpid()) {
    }

    void onUpdate() override {
    }

    void onRender() override {
    }

    void onImGui() override {
        /*const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        const int totalMemory = glxGpuTotalMemory();
        const int availMemory = glxGpuAvailMemory();

        ImGui::Begin("Engine Stats:");

        Window& window = Application::Instance().getWindow();

        ImGui::Text("CPU: %f%%", info.getProcessCpuUsage());
        ImGui::Text("Mem: %fMB", info.getProcessMemoryUsed());
        ImGui::Text("Threads: %lu", info.getProcessThreadCount());
        ImGui::Text("Video Mem: %d%% %d/%d", static_cast<int>((totalMemory - availMemory) / static_cast<float>(totalMemory) * 100), (totalMemory - availMemory) / 1024, totalMemory / 1024);
        ImGui::Text("Display: %dx%d (%s)", window.getWidth(), window.getHeight(), vendor);
        ImGui::Text(renderer);
        ImGui::Text(version);
        ImGui::Text("FPS: %d", Time::FramesPerSecond());
        //ImGui::Text("XYZ: " + glm::to_string(camera.position());
        ImGui::Text("Mouse Position: %s", glm::to_string(mouseInput.mousePosition()).c_str());
        ImGui::Text("Mouse Delta: %s", glm::to_string(mouseInput.mouseDelta()).c_str());
        ImGui::Text("Mouse Scroll: %s", glm::to_string(mouseInput.mouseScroll()).c_str());

        // ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        // ImGui::Text("Quads: %d", stats.QuadCount);
        //ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        //ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        auto& colors = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        const auto& buttonActive = colors[ImGuiCol_ButtonActive];
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

        ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        float size = ImGui::GetWindowHeight() - 4.0f;
        Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
        if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
        {
            if (m_SceneState == SceneState::Edit)
                OnScenePlay();
            else if (m_SceneState == SceneState::Play)
                OnSceneStop();
        }
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);
        ImGui::End();*/
    }

    ProcessInfo info;
    MouseInput mouseInput{{Mouse::ButtonLeft, Mouse::ButtonRight, Mouse::ButtonMiddle}};
    KeyInput keyInput{{Key::Escape}};
};

class Sandbox : public Application {
public:
    Sandbox(CommandLineArgs args) : Application("Sandbox", args) {
        pushLayer(new ExampleLayer());
    }
    ~Sandbox() override {

    }
};

Fusion::Application* Fusion::CreateApplication(CommandLineArgs args) {
    return new Sandbox(args);
}