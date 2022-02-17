#include "EditorLayer.hpp"
#include "Fusion/Core/Application.hpp"
#include "Fusion/Core/Time.hpp"
#include "Fusion/Input/Input.hpp"
#include "Fusion/ImGui/ImGuiLayer.hpp"
#include "Fusion/Renderer/Texture.hpp"
#include "Fusion/Renderer/Renderer.hpp"
#include "Fusion/Renderer/AllocatedBuffer.hpp"
#include "Fusion/Utils/Math.hpp"
#include "Fusion/Events/WindowEvents.hpp"
#include "Fusion/Scene/Components.hpp"

#if _WIN32
#define DEFAULT_PATH "C:\\"
#else
#define DEFAULT_PATH "/tmp"
#endif

//#include <backends/imgui_impl_vulkan.h>
#include <portable-file-dialogs/portable-file-dialogs.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imguizmo/ImGuizmo.h>

using namespace Fusion;

EditorLayer::EditorLayer(Vulkan& vulkan, Renderer& renderer) : Layer{"EditorLayer"}, vulkan{vulkan}, renderer{renderer}, meshRenderer{vulkan, renderer} {
}

EditorLayer::~EditorLayer() {

}

void EditorLayer::onAttach() {
    activeScene = std::make_shared<Scene>();
    editorCamera = EditorCamera(30, 1.778f, 0.1f, 1000);

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
    auto& window = vulkan.getWindow();
    if (auto* event = window.getEventQueue().next<WindowFramebufferSizeEvent>()) {
        editorCamera.setViewport(event->width, event->height);
    }

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

    if (!ImGuizmo::IsUsing()) {
        if (Input::GetKeyDown(Key::Q)) {
            gizmoType = -1;
        } else if (Input::GetKeyDown(Key::W)) {
            gizmoType = ImGuizmo::OPERATION::TRANSLATE;
        } else if (Input::GetKeyDown(Key::E)) {
            gizmoType = ImGuizmo::OPERATION::ROTATE;
        } else if (Input::GetKeyDown(Key::R)) {
            gizmoType = ImGuizmo::OPERATION::SCALE;
        }
    }

}

void EditorLayer::onRender() {
    // update
    UniformBufferObject ubo{};
    ubo.projection = editorCamera.getProjection();
    ubo.view = editorCamera.getView();
    ubo.lightDirection = glm::normalize(glm::vec3{1, -3, -1});
    //ubo.orthogonal = glm::ortho(0, vulkan.getWindow().getWidth(), 0, vulkan.getWindow().getHeight());
    auto& buffer = renderer.getUniformBuffers(renderer.getFrameIndex());
    buffer.writeToBuffer(&ubo);
    buffer.flush();

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
    // TODO: Find solution for viewport. Currently we use global transparent background and set the color for each window to make it non transparent.
    /// Better to force vulkan generate image before imgui render and render it as an image in non-transparent viewport but it will require a lot of additional work to implement back buffer.

    // Note: Switch this to true to enable dockspace
    static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
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
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockspace_flags);
    }

    style.WindowMinSize.x = minWinSizeX;

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

            if (ImGui::MenuItem("Screenshot", "F12"))
                renderer.getSwapChain()->saveScreenshot("assets/screenshot.png");

            if (ImGui::MenuItem("Exit"))
                Application::Instance().getWindow().shouldClose(true);

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    sceneHierarchyPanel.onImGui();
    contentBrowserPanel.onImGui();

    ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.1f, 0.105f, 0.11f, 1.0f });
    ImGui::Begin("Stats");

    std::string name = "None";
    //if (hoveredEntity)
    //    name = hoveredEntity.GetComponent<TagComponent>();
    //ImGui::Text("Hovered Entity: %s", name.c_str());
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

    ImGui::PopStyleColor();
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
    ImGui::Begin("Viewport");

    /*auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
    auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    auto viewportOffset = ImGui::GetWindowPos();
    viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
    viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

    viewportFocused = ImGui::IsWindowFocused();
    viewportHovered = ImGui::IsWindowHovered();
    Application::Instance().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

    ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });*/

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
            const auto* path = static_cast<const wchar_t*>(payload->Data);
            openScene(std::filesystem::path(AssetPath) / path);
        }
        ImGui::EndDragDropTarget();
    }

    // Gizmos
    auto selectedEntity = sceneHierarchyPanel.getSelectedEntity();
    if (selectedEntity != entt::null && gizmoType != -1) {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        ImGuizmo::SetRect(0, 0, vulkan.getWindow().getWidth(), vulkan.getWindow().getHeight());

        // Camera

        // Runtime camera from entity
        // auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
        // const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
        // const glm::mat4& cameraProjection = camera.GetProjection();
        // glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

        // Entity transform
        auto& component = activeScene->getEntityRegistry().get<TransformComponent>(selectedEntity);
        glm::mat4 transform = component;

        // Snapping
        bool snap = Input::GetKey(Key::LeftControl);
        float snapValue = 0.5f; // Snap to 0.5m for translation/scale
        // Snap to 45 degrees for rotation
        if (gizmoType == ImGuizmo::OPERATION::ROTATE)
            snapValue = 45.0f;

        float snapValues[3] = { snapValue, snapValue, snapValue };

        /* Imguimo fix for Vulkan projection */
        auto cameraView = glm::lookAtLH(editorCamera.getPosition(), -editorCamera.getForward(), -editorCamera.getUp());
        auto cameraProjection = glm::perspectiveLH(
                glm::radians(editorCamera.getFov()),
                editorCamera.getAspect(),
                editorCamera.getNearClip(),
                editorCamera.getFarClip());

        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                             (ImGuizmo::OPERATION)gizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
                             nullptr, snap ? snapValues : nullptr);

        //glm::mat4 m{1};
        //ImGuizmo::DrawGrid(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), glm::value_ptr(m), 100.0f);

        if (ImGuizmo::IsUsing()) {
            glm::vec3 translation, rotation, scale;
            glm::decompose(transform, translation, rotation, scale);

            glm::vec3 deltaRotation = rotation - component.rotation;
            component.translation = translation;
            component.rotation += deltaRotation;
            component.scale = scale;
        }
    }

    ImGui::PopStyleVar();
    ImGui::End();

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
