#include <Fusion.hpp>
#include "Fusion/Core/EntryPoint.hpp"

#include "imgui.h"
#include <unistd.h>

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
        const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        const int totalMemory = glxGpuTotalMemory();
        const int availMemory = glxGpuAvailMemory();

        ImGui::Begin("Engine Stats:");

        Window& window = Application::Instance().getWindow();
        ImGui::Text("Core:");
        ImGui::Text("CPU: %f%%", info.getProcessCpuUsage());
        ImGui::Text("Mem: %fMB", info.getProcessMemoryUsed());
        ImGui::Text("Threads: %lu", info.getProcessThreadCount());
        ImGui::Text("Graphics:");
        ImGui::Text("Video Mem: %d%% %d/%d", static_cast<int>((totalMemory - availMemory) / static_cast<float>(totalMemory) * 100), (totalMemory - availMemory) / 1024, totalMemory / 1024);
        ImGui::Text("Display: %dx%d (%s)", window.getWidth(), window.getHeight(), vendor);
        ImGui::Text(renderer);
        ImGui::Text(version);
        ImGui::Text("FPS: %d", Time::FramesPerSecond());
        //ImGui::Text("XYZ: " + glm::to_string(camera.position());
        ImGui::Text("Mouse:");
        ImGui::Text("Position: %s", glm::to_string(mouseInput.mousePosition()).c_str());
        ImGui::Text("Delta: %s", glm::to_string(mouseInput.mouseDelta()).c_str());
        ImGui::Text("Scroll: %s", glm::to_string(mouseInput.mouseScroll()).c_str());

        // ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        // ImGui::Text("Quads: %d", stats.QuadCount);
        //ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        //ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        ImGui::End();
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