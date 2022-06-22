#include "editor_layer.hpp"
#include "editor_app.hpp"

#include "fusion/scene/components.hpp"
#include "fusion/input/input.hpp"
#include "fusion/utils/math.hpp"

#include <portable-file-dialogs/portable-file-dialogs.h>

#include <imgui/imgui.h>
#include <imguizmo/ImGuizmo.h>
#include <IconsFontAwesome4.h>

using namespace fe;

// TODO: Fix editorScene -> activeScene

EditorLayer::EditorLayer() : Layer{"EditorLayer"} {
}

EditorLayer::~EditorLayer() {
}

void EditorLayer::onAttach() {
    editorCamera = EditorCamera{30, 1.778f, 0.1f, 1000};
    activeScene = std::make_shared<Scene>();

    /*auto commandLineArgs = Application::Instance().getCommandLineArgs();
    if (commandLineArgs.count > 1) {
        auto sceneFilePath = commandLineArgs[1];
        SceneSerializer serializer{activeScene};
        serializer.deserialize(sceneFilePath);
    }*/

    sceneHierarchyPanel.setContext(activeScene);
}

void EditorLayer::onDetach() {

}

void EditorLayer::onUpdate(float dt) {
    switch (sceneState) {
        case Edit: {
            if (viewportFocused)
                editorCamera.update(dt);
            activeScene->onUpdateEditor(dt);
            break;
        }

        case Play: {
            activeScene->onUpdateRuntime(dt);
            break;
        }
    }

    bool control = Input::GetKey(Key::LeftControl) || Input::GetKey(Key::RightControl);
    bool shift = Input::GetKey(Key::LeftShift) || Input::GetKey(Key::RightShift);

    if (Input::GetKeyDown(Key::N)) {
        if (control)
            newScene();
    } else if (Input::GetKeyDown(Key::O)) {
        if (control)
            openScene();
    } else if (Input::GetKeyDown(Key::S)) {
        if (control) {
            if (shift)
                saveSceneAs();
            else
                saveScene();
        }
    }/* else if (Input::GetKeyDown(Key::D)) {
        if (control)
            duplicateEntity();
    }*/

    if (!ImGuizmo::IsUsing()) {
        if (Input::GetKeyDown(Key::Q)) {
            gizmoType = -1;
        } else if (Input::GetKeyDown(Key::W)) {
            gizmoType = ImGuizmo::TRANSLATE;
        } else if (Input::GetKeyDown(Key::E)) {
            gizmoType = ImGuizmo::ROTATE;
        } else if (Input::GetKeyDown(Key::R)) {
            gizmoType = ImGuizmo::SCALE;
        } else if (Input::GetKeyDown(Key::T)) {
            gizmoType = ImGuizmo::UNIVERSAL;
        } else if (Input::GetKeyDown(Key::Y)) {
            gizmoType = ImGuizmo::BOUNDS;
        }
    }
}

void EditorLayer::onRender(Renderer& renderer) {
    // update
    /*GlobalUniformObject ubo{};
    ubo.projectionMatrix = editorCamera.getProjection();
    ubo.viewMatrix = editorCamera.getView();

    auto& buffer = renderer.getCurrentUniformBuffer();
    buffer.copy(ubo);
    buffer.flush();

    // TEMP
    if (!renderer.getOffscreen().active) {
        auto size = Application::Instance().getWindow()->getSize();
        if (size != viewportSize) {
            viewportSize = size;
            onViewportResize();
        }
        viewportFocused = true;
        viewportHovered = true;
    }*/

    switch (sceneState) {
        case Edit:
            activeScene->onRenderEditor(editorCamera);
            break;

        case Play: {
            activeScene->onRenderRuntime();
            break;
        }
    }

    //onOverlayRender();
}

void EditorLayer::onImGui() {
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
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen_persistant)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace = ImGui::GetID("HUB_DockSpace");
        ImGui::DockSpace(dockspace, { 0, 0 }, dockspace_flags);
    }

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

            //if (ImGui::MenuItem("Screenshot", "F12"))
            //    renderer.getSwapChain()->saveScreenshot("assets/screenshot.png");

            //if (ImGui::MenuItem("Exit"))
            //    window.shouldClose(true);

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    sceneHierarchyPanel.onImGui();
    contentBrowserPanel.onImGui();

    ImGui::Begin((ICON_FA_NEWSPAPER_O + "  Stats"s).c_str());

    //std::string name = "None";
    //if (hoveredEntity)
    //    name = hoveredEntity.GetComponent<TagComponent>();
    //ImGui::Text("Hovered Entity: %s", name.c_str());
    //ImGui::Text("CPU: %f%%", info.getProcessCpuUsage());
    //ImGui::Text("Mem: %fMB", info.getProcessMemoryUsed());
    //ImGui::Text("Threads: %lu", info.getProcessThreadCount());
    //ImGui::Text("Video Mem: %d%% %d/%d", static_cast<int>((totalMemory - availMemory) / static_cast<float>(totalMemory) * 100), (totalMemory - availMemory) / 1024, totalMemory / 1024);
    //ImGui::Text("Display: %dx%d", window.getWidth(), window.getHeight());
    //ImGui::Text(renderer);
    //ImGui::Text(version);
    //ImGui::Text("FPS: %d", Time::FramesPerSecond());
    //ImGui::Text("XYZ: " + glm::to_string(camera.position());
    ImGui::Text("Mouse Position: %s", glm::to_string(Input::MousePosition()).c_str());
    ImGui::Text("Mouse Normalized Position: %s", glm::to_string(Input::MouseNormalizedPosition()).c_str());
    ImGui::Text("Mouse Delta: %s", glm::to_string(Input::MousePositionDelta()).c_str());
    ImGui::Text("Mouse Scroll: %s", glm::to_string(Input::MouseScroll()).c_str());
    ImGui::Text("Viewport Focused: %s", viewportFocused ? "TRUE" : "FALSE");
    ImGui::Text("Viewport Hovered: %s", viewportHovered ? "TRUE" : "FALSE");

    //ImGui::Text("Renderer Stats:");
    //ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    //ImGui::Text("Quads: %d", stats.QuadCount);
    //ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    //ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
    //static glm::vec3 color;
    //ImGui::ColorEdit3("Background", glm::value_ptr(color));

    ImGui::End();

    //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
    ImGui::Begin((ICON_FA_LINODE + "  Scene"s).c_str());

    auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
    auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    auto viewportOffset = ImGui::GetWindowPos();
    glm::vec2 minBounds { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
    glm::vec2 maxBounds { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

    ImVec2 viewportPanelSize{ ImGui::GetContentRegionAvail() };

    if (viewportPanelSize.x != viewportSize.x || viewportPanelSize.y != viewportSize.y) {
        viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
        onViewportResize();
    }

    viewportFocused = ImGui::IsWindowFocused();
    viewportHovered = ImGui::IsWindowHovered();

    //context.getUI().blockEvents(!viewportFocused && !viewportHovered);

    /*ImGui::Button("Tools");
    ImGui::SameLine();
    ImGui::Button("Camera");
    ImGui::SameLine();
    ImGui::Button("Gizmos");*/

    //ImGui::Image(context.getRenderer().getCurrentFrameImage(), viewportPanelSize, { 0, 1 }, { 1, 0 });

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
            openScene(std::filesystem::path{ reinterpret_cast<const char*>(payload->Data) });
        }
        ImGui::EndDragDropTarget();
    }

    // Gizmos
    entt::entity selectedEntity = sceneHierarchyPanel.getSelectedEntity();
    if (gizmoType != -1 && activeScene->registry.valid(selectedEntity)) {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        ImGuizmo::SetRect(minBounds.x, minBounds.y, maxBounds.x - minBounds.x, maxBounds.y - minBounds.y);

        // Camera
        // Runtime camera from entity
        const glm::mat4& cameraView = editorCamera.getView();
        const glm::mat4& cameraProjection = editorCamera.getProjection();

        // Entity transform
        auto& transform = activeScene->registry.get<TransformComponent>(selectedEntity);

        // Snapping
        bool snap = Input::GetKey(Key::LeftControl);
        float snapValue = 0.5f; // Snap to 0.5m for translation/scale
        // Snap to 45 degrees for rotation
        if (gizmoType == ImGuizmo::ROTATE)
            snapValue = 45.0f;
        glm::vec3 snapValues{ snapValue };

        // Bounding
        auto bounds = gizmoType == ImGuizmo::BOUNDS;
        glm::vec3 boundsSnap{ 0.1f };  // Snap to 0.1m for bound change
        static glm::mat2x3 boundsValues = { glm::vec3{-1.0f}, glm::vec3{1.0f} };

        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                             static_cast<ImGuizmo::OPERATION>(gizmoType), ImGuizmo::WORLD, glm::value_ptr(transform.model),
                             nullptr, snap ? glm::value_ptr(snapValues) : nullptr,
                             bounds ? glm::value_ptr(boundsValues) : nullptr, bounds ? glm::value_ptr(boundsSnap) : nullptr);

        //ImGuizmo::ViewManipulate(glm::value_ptr(cameraView), editorCamera.getDistance(), {maxBounds.x - 128, minBounds.y}, {128, 128}, 0x10101010);

        if (ImGuizmo::IsUsing()) {
            glm::vec3 position, rotation, scale;
            glm::decompose(transform.model, position, rotation, scale);

            switch (gizmoType) {
                case ImGuizmo::TRANSLATE:
                    transform.position = position;
                    break;
                case ImGuizmo::ROTATE:
                    transform.rotation = rotation;
                    break;
                case ImGuizmo::SCALE:
                    transform.scale = scale;
                    break;
                default:
                    transform.position = position;
                    transform.rotation = rotation;
                    transform.scale = scale;
                    break;
            }

            activeScene->registry.patch<TransformComponent>(selectedEntity);
        }
    }

    //ImGui::PopStyleVar();
    ImGui::End();

    UI_Toolbar();

    ImGui::End();
}

void EditorLayer::newScene() {
    activeScene = std::make_shared<Scene>();
    activeScene->onViewportResize(viewportSize);
    sceneHierarchyPanel.setContext(activeScene);
    editorScenePath = std::filesystem::path{};
}

void EditorLayer::openScene() {
    auto filepath = pfd::open_file("Choose scene file", getAssetPath(), { "Scene File (.scene)", "*.scene" }, pfd::opt::none).result();
    if (!filepath.empty()) {
        // Validate that file inside working directory
        if (filepath[0].find(std::filesystem::current_path()) != std::string::npos) {
            openScene(filepath[0]);
        } else {
            pfd::message("File Location", "The selected file should be inside the project directory.", pfd::choice::ok, pfd::icon::error);
        }
    }
}

void EditorLayer::openScene(const std::filesystem::path& file) {
    if (sceneState != Edit)
        onSceneStop();

    if (file.extension().string() != ".scene") {
        LOG_WARNING << "Could not load " << file.filename() << " - not a scene file";
        return;
    }

    auto newScene = std::make_shared<Scene>();
    SceneSerializer serializer{ newScene };
    if (serializer.deserialize(file)) {
        editorScene = newScene;
        editorScene->onViewportResize(viewportSize);
        sceneHierarchyPanel.setContext(editorScene);

        activeScene = editorScene;
        editorScenePath = file;
    }

    LOG_INFO << "Scene " << file.filename() << " was loaded !";
}

void EditorLayer::saveScene() {
    if (!editorScenePath.empty()) {
        SceneSerializer serializer{ activeScene };
        serializer.serialize(editorScenePath);
    } else
        saveSceneAs();
}

void EditorLayer::saveSceneAs() {
    auto filepath = pfd::save_file("Choose scene file", getAssetPath(), { "Scene Files (.scene)", "*.scene" }, pfd::opt::force_overwrite).result();
    if (!filepath.empty()) {
        SceneSerializer serializer{ activeScene };
        serializer.serialize(filepath);
        editorScenePath = filepath;
    }
}

void EditorLayer::onViewportResize() {
    editorCamera.setViewport(viewportSize);
    activeScene->onViewportResize(viewportSize);
    viewportOrtho = glm::ortho(0.0f, viewportSize.x, 0.0f, viewportSize.y);
}

void EditorLayer::UI_Toolbar() {
    ImGui::Begin((ICON_FA_COGS + "  Toolbar"s).c_str(), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 2.0f });
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 0.0f, 0.0f });
    ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
    auto& colors = ImGui::GetStyle().Colors;
    const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f });
    const auto& buttonActive = colors[ImGuiCol_ButtonActive];
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { buttonActive.x, buttonActive.y, buttonActive.z, 0.5f });

    float size = ImGui::GetWindowHeight() - 4.0f;
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
    if (ImGui::Button(sceneState == Edit ? ICON_FA_PLAY : ICON_FA_STOP, {size, size})) {
        if (sceneState == Edit)
            onScenePlay();
        else if (sceneState == Play)
            onSceneStop();
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    ImGui::End();
}

void EditorLayer::onScenePlay() {
    sceneState = Play;

    activeScene = std::make_shared<Scene>(*editorScene);
    activeScene->onRuntimeStart();

    sceneHierarchyPanel.setContext(activeScene);
}

void EditorLayer::onSceneStop() {
    sceneState = Edit;

    activeScene->onRuntimeStop();
    activeScene = editorScene;

    sceneHierarchyPanel.setContext(activeScene);
}