#include "EditorLayer.hpp"
#include "Fusion/Core/Application.hpp"
#include "Fusion/Core/Time.hpp"
#include "Fusion/Input/Input.hpp"
#include "Fusion/ImGui/ImGuiLayer.hpp"
#include "Fusion/Renderer/Texture.hpp"
#include "Fusion/Renderer/Renderer.hpp"
#include "Fusion/Renderer/AllocatedBuffer.hpp"

#include <backends/imgui_impl_vulkan.h>
#include <portable-file-dialogs/portable-file-dialogs.h>

#if _WIN32
#define DEFAULT_PATH "C:\\"
#else
#define DEFAULT_PATH "/tmp"
#endif

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imguizmo/ImGuizmo.h"

using namespace Fusion;

EditorLayer::EditorLayer(Vulkan& vulkan, Renderer& renderer) : Layer{"EditorLayer"}, vulkan{vulkan}, renderer{renderer}, meshRenderer{vulkan, renderer} {
}

EditorLayer::~EditorLayer() {

}

void EditorLayer::onAttach() {
    activeScene = std::make_shared<Scene>();
    editorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

    auto commandLineArgs = Application::Instance().getCommandLineArgs();
    if (commandLineArgs.count > 1) {
        auto sceneFilePath = commandLineArgs[1];
        SceneSerializer serializer{activeScene};
        serializer.deserialize(sceneFilePath);
    }

    sceneHierarchyPanel.setContext(activeScene);
}

void EditorLayer::onDetach() {

}

void EditorLayer::onUpdate() {
    switch (sceneState) {
        case SceneState::Edit: {
            editorCamera.onUpdate();
            activeScene->onUpdateEditor();
            break;
        }

        case SceneState::Play: {
            activeScene->onUpdateRuntime();
            break;
        }
    }
}

void EditorLayer::onRender() {
    // update
    UniformBufferObject ubo{};
    ubo.perspective = editorCamera.getViewProjection();
    ubo.orthogonal = glm::ortho(0, vulkan.getWindow().getWidth(), 0, vulkan.getWindow().getHeight());
    auto& buffer = renderer.getUniformBuffers(renderer.getFrameIndex());
    buffer->writeToBuffer(&ubo);
    buffer->flush();

    switch (sceneState) {
        case SceneState::Edit: {
            activeScene->onRenderEditor(meshRenderer);
            break;
        }

        case SceneState::Play: {
            activeScene->onRenderRuntime(meshRenderer);
            break;
        }
    }
}

void EditorLayer::onImGui() {
    // Note: Switch this to true to enable dockspace
    static bool dockspaceOpen = true;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize({viewport->Size.x, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
    ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar(3);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1
            if (ImGui::MenuItem("New", "Ctrl+N"))
                newScene();

            if (ImGui::MenuItem("Open...", "Ctrl+O"))
                openScene();

            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                saveSceneAs();

            if (ImGui::MenuItem("Exit")) Application::Instance().getWindow().shouldClose(true);
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    sceneHierarchyPanel.onImGui();
    contentBrowserPanel.onImGui();

    ImGui::Begin("Stats");

    /*std::string name = "None";
    if (m_HoveredEntity)
        name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
    ImGui::Text("Hovered Entity: %s", name.c_str());*/
    ImGui::Text("CPU: %f%%", info.getProcessCpuUsage());
    ImGui::Text("Mem: %fMB", info.getProcessMemoryUsed());
    ImGui::Text("Threads: %lu", info.getProcessThreadCount());
    //ImGui::Text("Video Mem: %d%% %d/%d", static_cast<int>((totalMemory - availMemory) / static_cast<float>(totalMemory) * 100), (totalMemory - availMemory) / 1024, totalMemory / 1024);
    //ImGui::Text("Display: %dx%d (%s)", window.getWidth(), window.getHeight(), vendor);
    //ImGui::Text(renderer);
    //ImGui::Text(version);
    ImGui::Text("FPS: %d", Time::FramesPerSecond());
    //ImGui::Text("XYZ: " + glm::to_string(camera.position());
    ImGui::Text("Mouse Position: %s", glm::to_string(Input::MousePosition()).c_str());
    ImGui::Text("Mouse Delta: %s", glm::to_string(Input::MouseDelta()).c_str());
    ImGui::Text("Mouse Scroll: %s", glm::to_string(Input::MouseScroll()).c_str());
    //ImGui::Text("Renderer Stats:");
    //ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    //ImGui::Text("Quads: %d", stats.QuadCount);
    //ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    //ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
    ImGui::ColorEdit3("Background", glm::value_ptr(Application::Instance().getRenderer().getColor()));

    ImGui::End();
















    /*ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Viewport");
    auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
    auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    auto viewportOffset = ImGui::GetWindowPos();
    viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
    viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

    viewportFocused = ImGui::IsWindowFocused();
    viewportHovered = ImGui::IsWindowHovered();
    //Application::Instance().getImGuiLayer()->BlockEvents(!viewportFocused && !viewportHovered);

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

    static ImTextureID textureId;
    static bool f = false;
    if (!f) {
        texture = std::make_shared<Texture>(Application::Instance().getVulkan(), "assets/textures/texture.jpg",
                                            vk::Format::eR8G8B8A8Srgb
                                            );
        textureId = (ImTextureID) ImGui_ImplVulkan_AddTexture(texture->getSampler(), swapChain->currentImage(0), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        f = true;
    }

    ImGui::Image(textureId, ImVec2{viewportSize.x, viewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
            const auto* path = static_cast<const wchar_t*>(payload->Data);
            openScene(std::filesystem::path(AssetPath) / path);
        }
        ImGui::EndDragDropTarget();
    }*/

    // Gizmos
    /*auto selectedEntity = sceneHierarchyPanel.getSelectedEntity();
    if (selectedEntity != entt::null && gizmoType != -1) {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        ImGuizmo::SetRect(viewportBounds[0].x, viewportBounds[0].y, viewportBounds[1].x - viewportBounds[0].x, viewportBounds[1].y - viewportBounds[0].y);

        // Camera

        // Runtime camera from entity
        // auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
        // const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
        // const glm::mat4& cameraProjection = camera.GetProjection();
        // glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

        // Editor camera
        const glm::mat4& cameraProjection = editorCamera.getProjection();
        glm::mat4 cameraView = editorCamera.getView();

        // Entity transform
        auto& tc = selectedEntity.GetComponent<TransformComponent>();
        glm::mat4 transform = tc.GetTransform();

        // Snapping
        bool snap = Input::IsKeyPressed(Key::LeftControl);
        float snapValue = 0.5f; // Snap to 0.5m for translation/scale
        // Snap to 45 degrees for rotation
        if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
            snapValue = 45.0f;

        float snapValues[3] = { snapValue, snapValue, snapValue };

        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                             (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
                             nullptr, snap ? snapValues : nullptr);

        if (ImGuizmo::IsUsing()) {
            glm::vec3 translation, rotation, scale;
            Math::DecomposeTransform(transform, translation, rotation, scale);

            glm::vec3 deltaRotation = rotation - tc.Rotation;
            tc.Translation = translation;
            tc.Rotation += deltaRotation;
            tc.Scale = scale;
        }
    }*/

    //ImGui::End();
    //ImGui::PopStyleVar();

    //UI_Toolbar();

    ImGui::End();
}

void EditorLayer::newScene() {
    activeScene = std::make_shared<Scene>();
    //activeScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    sceneHierarchyPanel.setContext(activeScene);
}

void EditorLayer::openScene() {
    auto filepath = pfd::open_file("Choose scene file", DEFAULT_PATH, { "Scene Files (.scene)", "*.scene", "All Files", "*" }, pfd::opt::none).result();
    if (!filepath.empty())
        openScene(filepath[0]);
}

void EditorLayer::openScene(const std::filesystem::path& path) {
    if (path.extension().string() != ".scene") {
        FE_LOG_WARNING << "Could not load " << path.filename().string() << " - not a scene file";
        return;
    }

    auto newScene = std::make_shared<Scene>();
    SceneSerializer serializer{newScene};
    if (serializer.deserialize(path)) {
        activeScene = newScene;
        //activeScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        sceneHierarchyPanel.setContext(activeScene);
    }
}

void EditorLayer::saveSceneAs() {
    auto filepath = pfd::save_file("Choose scene file", DEFAULT_PATH, { "Scene Files (.scene)", "*.scene", "All Files", "*" }, pfd::opt::force_overwrite).result();
    if (!filepath.empty()) {
        SceneSerializer serializer{activeScene};
        serializer.serialize(filepath);
    }
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
