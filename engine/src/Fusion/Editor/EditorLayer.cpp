#include "EditorLayer.hpp"
#include "Fusion/Core/Application.hpp"
#include "Fusion/Core/Time.hpp"
#include "Fusion/Input/Input.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imguizmo/ImGuizmo.h>

using namespace Fusion;

EditorLayer::EditorLayer() : Layer("EditorLayer") {
}

EditorLayer::~EditorLayer() {

}


void EditorLayer::onAttach() {
    activeScene = std::make_shared<Scene>();
    editorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

    auto commandLineArgs = Application::Instance().getCommandLineArgs();
    if (commandLineArgs.count > 1) {
        auto sceneFilePath = commandLineArgs[1];
        SceneSerializer serializer(activeScene);
        serializer.deserialize(sceneFilePath);
    }

    sceneHierarchyPanel.setContext(activeScene);
}

void EditorLayer::onDetach() {

}

void EditorLayer::onUpdate() {
    editorCamera.onUpdate();
}

void EditorLayer::onImGui() {


    /*ImGuiIO& io = ImGui::GetIO();
    float viewManipulateRight = io.DisplaySize.x;
    float viewManipulateTop = 0;
    static ImGuiWindowFlags gizmoWindowFlags = 0;
    bool useWindow = false;


    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    glm::mat4 objectMatrix{1};
    glm::mat4{1};

    auto& cameraView = editorCamera.getView();
    auto& cameraProjection = editorCamera.getProjection();

    // Snapping
    bool snap = Input::GetKey(Key::LeftControl);
    float snapValue = 0.5f; // Snap to 0.5m for translation/scale
    // Snap to 45 degrees for rotation
    //if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
    //    snapValue = 45.0f;

    glm::mat4 transoform = glm::mat4{1};

    float snapValues[3] = { snapValue, snapValue, snapValue };

    ImGuizmo::DrawGrid(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), glm::value_ptr(transoform), 1000.0f);
    ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                         ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(transoform),
                         nullptr, snap ? snapValues : nullptr);*/


    /*ImGui::Begin("Settings");
    ImGui::ColorEdit3("Sky Color", glm::value_ptr(Application::Instance().getRenderer().getColor()));
    ImGui::End();*/

    /*if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            // ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
            if (ImGui::MenuItem("New", "Ctrl+N"))
                newScene();

            if (ImGui::MenuItem("Open...", "Ctrl+O"))
                openScene();

            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                saveSceneAs();

            if (ImGui::MenuItem("Exit"))
                Application::Instance().getWindow().shouldClose(true);

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::Separator();

    sceneHierarchyPanel.onImGui();
    contentBrowserPanel.onImGui();

    ImGui::Begin("Stats");

    std::string name = "None";
    if (m_HoveredEntity)
        name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
    ImGui::Text("Hovered Entity: %s", name.c_str());

    ImGui::Text("CPU: %f%%", info.getProcessCpuUsage());
    ImGui::Text("Mem: %fMB", info.getProcessMemoryUsed());
    ImGui::Text("Threads: %lu", info.getProcessThreadCount());
    //ImGui::Text("Video Mem: %d%% %d/%d", static_cast<int>((totalMemory - availMemory) / static_cast<float>(totalMemory) * 100), (totalMemory - availMemory) / 1024, totalMemory / 1024);
    //ImGui::Text("Display: %dx%d (%s)", window.getWidth(), window.getHeight(), vendor);
    //ImGui::Text(renderer);
    //ImGui::Text(version);
    ImGui::Text("FPS: %d", Time::FramesPerSecond());
    //ImGui::Text("XYZ: " + glm::to_string(camera.position());
    ImGui::Text("Mouse Position: %s", glm::to_string(mouseInput.mousePosition()).c_str());
    ImGui::Text("Mouse Delta: %s", glm::to_string(mouseInput.mouseDelta()).c_str());
    ImGui::Text("Mouse Scroll: %s", glm::to_string(mouseInput.mouseScroll()).c_str());
    //ImGui::Text("Renderer Stats:");
    //ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    //ImGui::Text("Quads: %d", stats.QuadCount);
    //ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    //ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    ImGui::End();*/
}

void EditorLayer::openScene() {

}

void EditorLayer::newScene() {

}

void EditorLayer::saveSceneAs() {

}

void EditorLayer::UI_Toolbar() {
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
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
    if (ImGui::Button(sceneState == SceneState::Edit ? "\uF07C" : "\uF15C", ImVec2(size, size))) {
        /*if (sceneState == SceneState::Edit)
            onScenePlay();
        else if (sceneState == SceneState::Play)
            onSceneStop();*/
    }
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    ImGui::End();
}
