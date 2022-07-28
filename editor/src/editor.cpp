#include "editor.hpp"
#include "editor_renderer.hpp"

#include "fusion/core/engine.hpp"
#include "fusion/devices/device_manager.hpp"
#include "fusion/input/input.hpp"
#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/cameras/camera.hpp"
#include "fusion/scene/components.hpp"
#include "fusion/scene/scene_manager.hpp"
#include "fusion/filesystem/file_format.hpp"

#include "fusion/filesystem/file_system.hpp"
#include "panels/application_info_panel.hpp"
#include "panels/console_panel.hpp"
#include "panels/content_browser_panel.hpp"
#include "panels/project_settings_panel.hpp"
#include "panels/text_edit_panel.hpp"
#include "panels/hierarchy_panel.hpp"
#include "panels/inspector_panel.hpp"
#include "panels/scene_view_panel.hpp"
#include "panels/game_view_panel.hpp"

#include <imgui/imgui.h>
#include <imguizmo/ImGuizmo.h>

#define BIND_FILEBROWSER_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

using namespace fe;

Editor::Editor(std::string name) : DefaultApplication{std::move(name)} {
}

Editor::~Editor() {
}

void Editor::onStart() {
    DefaultApplication::onStart();

    Graphics::Get()->setRenderer(std::make_unique<EditorRenderer>());

    const auto& size = DeviceManager::Get()->getWindow(0)->getSize();
    editorCamera = std::make_shared<Camera>();

    componentIconMap[type_id<TransformComponent>] = ICON_MDI_AXIS_ARROW;
    //componentIconMap[type_id<ModelComponent>] = ICON_MDI_SHAPE;
    componentIconMap[type_id<MeshComponent>] = ICON_MDI_SHAPE;
    componentIconMap[type_id<CameraComponent>] = ICON_MDI_CAMERA;
    componentIconMap[type_id<LightComponent>] = ICON_MDI_LIGHTBULB;
    /*componentIconMap[type_id<RigidBodyComponent>] = ICON_MDI_CUBE_OUTLINE;
    componentIconMap[type_id<PhysicsMaterialComponent>] = ICON_FA_TENCENT_WEIBO;
    componentIconMap[type_id<BoxColliderComponent>] = ICON_FA_SQUARE_O;
    componentIconMap[type_id<SphereColliderComponent>] = ICON_FA_CIRCLE_O;
    componentIconMap[type_id<CapsuleColliderComponent>] = ICON_FA_TOGGLE_OFF;
    componentIconMap[type_id<MaterialComponent>] = ICON_MDI_MATERIAL_UI;
    componentIconMap[type_id<PointLightComponent>] = ICON_MDI_LIGHTBULB;
    componentIconMap[type_id<DirectionalLightComponent>] = ICON_MDI_SPOTLIGHT_BEAM;
    componentIconMap[type_id<SoundComponent>] = ICON_MDI_VOLUME_HIGH;
    componentIconMap[type_id<SpriteComponent>] = ICON_MDI_IMAGE;
    componentIconMap[type_id<LuaScriptComponent>] = ICON_MDI_SCRIPT;
    componentIconMap[type_id<EnvironmentComponent>] = ICON_MDI_EARTH;*/
    componentIconMap[type_id<Editor>] = ICON_MDI_SQUARE;

    panels.push_back(std::make_unique<ApplicationInfoPanel>(this));
    panels.push_back(std::make_unique<ConsolePanel>(this));
    panels.push_back(std::make_unique<ContentBrowserPanel>(this));
    panels.push_back(std::make_unique<ProjectSettingsPanel>(this));
    panels.push_back(std::make_unique<HierarchyPanel>(this));
    panels.push_back(std::make_unique<InspectorPanel>(this));
    panels.push_back(std::make_unique<SceneViewPanel>(this));
    panels.push_back(std::make_unique<GameViewPanel>(this));

    editorSettings.showImGuiDemo = false;
}

void Editor::onUpdate() {
    auto scene = SceneManager::Get()->getScene();
    if (sceneViewActive && scene) {
        auto input = Input::Get();

        if (editorCamera)
            editorCameraController.update(*editorCamera);

        if (input->getKeyDown(Key::F)) {
            auto& registry = scene->getRegistry();
            if (registry.valid(selectedEntity)) {
                /*auto transform = registry.try_get<TransformComponent>(selectedEntity);
                if (transform)
                    FocusCamera(transform->GetWorldPosition(), 2.0f, 2.0f);*/
            }
        }

        if (input->getKey(Key::O)) {
            //focusCamera(glm::vec3(0.0f, 0.0f, 0.0f), 2.0f, 2.0f);
        }

        if (transitioningCamera) {
            float ts = Time::DeltaTime().asSeconds();

            if (cameraTransitionStartTime < 0.0f)
                cameraTransitionStartTime = ts;

            float focusProgress = std::min((ts - cameraTransitionStartTime) / cameraTransitionSpeed, 1.f);
            glm::vec3 newCameraPosition = glm::mix(cameraStartPosition, cameraDestination, focusProgress);
            editorCamera->setEyePoint(newCameraPosition);

            if (editorCamera->getEyePoint() == cameraDestination)
                transitioningCamera = false;
        }

        if (!input->getMouseButton(MouseButton::ButtonRight) && !ImGuizmo::IsUsing()) {
            if (input->getKeyDown(Key::Q)) {
                editorSettings.gizmosOperation = ImGuizmo::OPERATION::BOUNDS;
            }

            if (input->getKeyDown(Key::W)) {
                editorSettings.gizmosOperation = ImGuizmo::OPERATION::TRANSLATE;
            }

            if (input->getKeyDown(Key::E)) {
                editorSettings.gizmosOperation = ImGuizmo::OPERATION::ROTATE;
            }

            if (input->getKeyDown(Key::R)) {
                editorSettings.gizmosOperation = ImGuizmo::OPERATION::SCALE;
            }

            if (input->getKeyDown(Key::T)) {
                editorSettings.gizmosOperation = ImGuizmo::OPERATION::UNIVERSAL;
            }

            if (input->getKeyDown(Key::Y)) {
                editorSettings.snapGizmos = !editorSettings.snapGizmos;
            }
        }

        if ((input->getKey(Key::LeftSuper) || (input->getKey(Key::LeftControl)))) {
            if (input->getKeyDown(Key::S) && sceneActive) {
                scene->serialise();
            }

            if (input->getKeyDown(Key::O))
               scene->deserialise();

            if (input->getKeyDown(Key::X)) {
                copiedEntity = selectedEntity;
                cutCopyEntity = true;
            }

            if (input->getKeyDown(Key::C)) {
                copiedEntity = selectedEntity;
                cutCopyEntity = false;
            }

            if (input->getKeyDown(Key::V) && copiedEntity != entt::null) {
                scene->duplicateEntity(copiedEntity);
                if (cutCopyEntity) {
                    if (copiedEntity == selectedEntity)
                        selectedEntity = entt::null;
                   scene->destroyEntity(copiedEntity);
                }
            }

            if (input->getKeyDown(Key::D) && selectedEntity != entt::null) {
                scene->duplicateEntity(selectedEntity);
            }
        }
    } else
        editorCameraController.stopMovement();

    DefaultApplication::onUpdate();
}

void Editor::onImGuizmo() {
    if (selectedEntity == entt::null || editorSettings.gizmosOperation == UINT32_MAX)
        return;

    if (editorSettings.showGizmos && editorCamera) {
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetOrthographic(editorCamera->isOrthographic());

        auto& registry = SceneManager::Get()->getScene()->getRegistry();
        if (auto transform = registry.try_get<TransformComponent>(selectedEntity)) {
            const glm::mat4& view = editorCamera->getViewMatrix();
            const glm::mat4& proj = editorCamera->getProjectionMatrix();
            glm::mat4 model = transform->getWorldMatrix();
            glm::mat4 delta{1};

            auto gizmosType = static_cast<ImGuizmo::OPERATION>(editorSettings.gizmosOperation);

            // Snapping
            float snapValue = editorSettings.snapAmount; // Snap to 0.5m for translation/scale
            if (gizmosType == ImGuizmo::ROTATE)
                snapValue = 45.0f;
            glm::vec3 snapValues{ snapValue };

            // Bounding
            auto bounds = gizmosType == ImGuizmo::BOUNDS;
            glm::vec3 boundsSnap{ editorSettings.snapBound };  // Snap to 0.1m for bound change
            static glm::mat2x3 boundsValues = { glm::vec3{-1.0f}, glm::vec3{1.0f} };

            ImGuizmo::Manipulate(glm::value_ptr(view),
                                 glm::value_ptr(proj),
                                 gizmosType,
                                 ImGuizmo::LOCAL,
                                 glm::value_ptr(model),
                                 glm::value_ptr(delta),
                                 editorSettings.snapGizmos ? glm::value_ptr(snapValues) : nullptr,
                                 bounds ? glm::value_ptr(boundsValues) : nullptr,
                                 bounds ? glm::value_ptr(boundsSnap) : nullptr);

            if (ImGuizmo::IsUsing()) {
                model = glm::inverse(transform->getParentMatrix()) * model;

                glm::vec3 position, scale;
                glm::quat orientation;
                glm::vec3 skew;
                glm::vec4 perspective;
                glm::decompose(model, scale, orientation, position, skew, perspective);

                switch (gizmosType) {
                    case ImGuizmo::TRANSLATE:
                        transform->setLocalPosition(position);
                        break;
                    case ImGuizmo::ROTATE:
                        transform->setLocalOrientation(orientation);
                        break;
                    case ImGuizmo::SCALE:
                        transform->setLocalScale(scale);
                        break;
                    default:
                        transform->setLocalPosition(position);
                        transform->setLocalOrientation(orientation);
                        transform->setLocalScale(scale);
                        break;
                }
            }

            //transform->setLocalTransform(model);

            /*RigidBody2DComponent* rigidBody2DComponent = registry.try_get<RigidBody2DComponent>(selectedEntity);

            if (rigidBody2DComponent) {
                rigidBody2DComponent->GetRigidBody()->SetPosition( { model[3].x, model[3].y });
            } else {
                RigidBody3DComponent* rigidBody3DComponent = registry.try_get<RigidBody3DComponent>(selectedEntity);
                if (rigidBody3DComponent) {
                    rigidBody3DComponent->GetRigidBody()->SetPosition(model[3]);
                    rigidBody3DComponent->GetRigidBody()->SetOrientation(glm::eulerAngles(glm::quat_cast((model)));
                }
            }*/
        }
    }
}

void Editor::onImGui() {
    beginDockSpace(editorSettings.fullScreenOnPlay && editorState == EditorState::Play);

    drawMenuBar();

    for (auto& panel : panels) {
        if (panel->isActive()) {
            panel->onImGui();
        }
    }

    if (editorSettings.showImGuiDemo)
        ImGui::ShowDemoWindow(&editorSettings.showImGuiDemo);

    fileBrowserPanel.onImGui();

    endDockSpace();

    DefaultApplication::onImGui();
}

void Editor::beginDockSpace(bool gameFullScreen) {
    static bool open = true;
    static bool optFullscreen = true;
    static ImGuiDockNodeFlags optFlags = ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton; // ImGuiDockNodeFlags_PassthruCentralNode

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (optFullscreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the
    // pass-thru hole, so we ask Begin() to not render a background.
    if (optFlags & ImGuiDockNodeFlags_DockSpace || optFlags & ImGuiDockNodeFlags_PassthruCentralNode)
        windowFlags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("DockSpace", &open, windowFlags);
    ImGui::PopStyleVar();

    if (optFullscreen)
        ImGui::PopStyleVar(2);

    ImGuiID dockspaceID = ImGui::GetID("DockSpace");

    static std::vector<EditorPanel*> hiddenPanels;

    if (editorSettings.fullScreenSceneView != gameFullScreen) {
        editorSettings.fullScreenSceneView = gameFullScreen;

        if (editorSettings.fullScreenSceneView) {
            for (auto& panel : panels) {
                if (panel->getName() != "Game" && panel->isActive()) {
                    panel->setActive(false);
                    hiddenPanels.push_back(panel.get());
                }
            }
        } else {
            for (auto& panel: hiddenPanels) {
                panel->setActive(true);
            }

            hiddenPanels.clear();
        }
    }

    if (!ImGui::DockBuilderGetNode(dockspaceID)) {
        ImGui::DockBuilderRemoveNode(dockspaceID); // Clear out existing layout
        ImGui::DockBuilderAddNode(dockspaceID); // Add empty node
        ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetIO().DisplaySize * ImGui::GetIO().DisplayFramebufferScale);

        ImGuiID DockMain = dockspaceID;

        ImGuiID DockBottom = ImGui::DockBuilderSplitNode(DockMain, ImGuiDir_Down, 0.3f, nullptr, &DockMain);
        ImGuiID DockLeft = ImGui::DockBuilderSplitNode(DockMain, ImGuiDir_Left, 0.2f, nullptr, &DockMain);
        ImGuiID DockRight = ImGui::DockBuilderSplitNode(DockMain, ImGuiDir_Right, 0.20f, nullptr, &DockMain);

        ImGuiID DockLeftChild = ImGui::DockBuilderSplitNode(DockLeft, ImGuiDir_Down, 0.875f, nullptr, &DockLeft);
        ImGuiID DockRightChild = ImGui::DockBuilderSplitNode(DockRight, ImGuiDir_Down, 0.875f, nullptr, &DockRight);
        ImGuiID DockingLeftDownChild = ImGui::DockBuilderSplitNode(DockLeftChild, ImGuiDir_Down, 0.06f, nullptr, &DockLeftChild);
        ImGuiID DockingRightDownChild = ImGui::DockBuilderSplitNode(DockRightChild, ImGuiDir_Down, 0.06f, nullptr, &DockRightChild);

        ImGuiID DockBottomChild = ImGui::DockBuilderSplitNode(DockBottom, ImGuiDir_Down, 0.2f, nullptr, &DockBottom);
        ImGuiID DockingBottomLeftChild = ImGui::DockBuilderSplitNode(DockLeft, ImGuiDir_Down, 0.4f, nullptr, &DockLeft);
        ImGuiID DockingBottomRightChild = ImGui::DockBuilderSplitNode(DockRight, ImGuiDir_Down, 0.4f, nullptr, &DockRight);

        ImGuiID DockMiddle = ImGui::DockBuilderSplitNode(DockMain, ImGuiDir_Right, 0.8f, nullptr, &DockMain);
        ImGuiID DockBottomMiddle = ImGui::DockBuilderSplitNode(DockMiddle, ImGuiDir_Down, 0.3f, nullptr, &DockMiddle);
        ImGuiID DockMiddleLeft = ImGui::DockBuilderSplitNode(DockMiddle, ImGuiDir_Left, 0.5f, nullptr, &DockMiddle);
        ImGuiID DockMiddleRight = ImGui::DockBuilderSplitNode(DockMiddle, ImGuiDir_Right, 0.5f, nullptr, &DockMiddle);

        ImGui::DockBuilderDockWindow("###game", DockMiddleRight);
        ImGui::DockBuilderDockWindow("###scene", DockMiddleLeft);
        ImGui::DockBuilderDockWindow("###inspector", DockRight);
        ImGui::DockBuilderDockWindow("###console", DockBottomMiddle);
        ImGui::DockBuilderDockWindow("###profiler", DockingBottomLeftChild);
        ImGui::DockBuilderDockWindow("###content", DockingBottomLeftChild);
        ImGui::DockBuilderDockWindow("Dear ImGui Demo", DockLeft);
        ImGui::DockBuilderDockWindow("###projectsettings", DockingBottomRightChild);
        ImGui::DockBuilderDockWindow("###hierarchy", DockLeft);
        ImGui::DockBuilderDockWindow("###textedit", DockMiddle);
        ImGui::DockBuilderDockWindow("###editorsettings", DockingBottomRightChild);
        ImGui::DockBuilderDockWindow("###appinfo", DockingBottomRightChild);

        ImGui::DockBuilderFinish(dockspaceID);
    }

    // Dockspace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        ImGui::DockSpace(dockspaceID, ImVec2{0.0f, 0.0f}, optFlags);
}

void Editor::endDockSpace() {
    ImGui::End();
}

void Editor::drawMenuBar() {
    bool openSaveScenePopup = false;
    bool openNewScenePopup = false;
    bool openReloadScenePopup = false;
    bool openProjectLoadPopup = !projectLoaded;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open Project")) {
                reopenNewProjectPopup = false;
                openProjectLoadPopup = true;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Open File")) {
                fileBrowserPanel.setCurrentPath(projectSettings.projectRoot);
                fileBrowserPanel.setCallback(BIND_FILEBROWSER_FN(fileOpenCallback));
                fileBrowserPanel.open();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("New Scene", "CTRL+N")) {
                openNewScenePopup = true;
            }

            if (ImGui::MenuItem("Save Scene", "CTRL+S")) {
                openSaveScenePopup = true;
            }

            if (ImGui::MenuItem("Reload Scene", "CTRL+R")) {
                openReloadScenePopup = true;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit")) {
                Engine::Get()->requestClose();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            // TODO: Undo and Redo

            bool enabled = selectedEntity != entt::null;

            if (ImGui::MenuItem("Cut", "CTRL+X", false, enabled)) {
                copiedEntity = selectedEntity;
                cutCopyEntity = true;
            }

            if (ImGui::MenuItem("Copy", "CTRL+C", false, enabled)) {
                copiedEntity = selectedEntity;
                cutCopyEntity = false;
            }

            enabled = copiedEntity != entt::null;

            if (ImGui::MenuItem("Paste", "CTRL+V", false, enabled)) {
                auto scene = SceneManager::Get()->getScene();
                scene->duplicateEntity(copiedEntity);
                if (cutCopyEntity) {
                    if (copiedEntity == selectedEntity)
                        selectedEntity = entt::null;
                    scene->destroyEntity(copiedEntity);
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Panels")) {
            for (auto& panel : panels) {
                if (ImGui::MenuItem(panel->getTitle().c_str(), "", &panel->Active(), true)) {
                    panel->setActive(true);
                }
            }

            if (ImGui::MenuItem("ImGui Demo", "", &editorSettings.showImGuiDemo, true)) {
                editorSettings.showImGuiDemo = true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scenes")) {
            fs::path scenePath{ projectSettings.projectRoot / "assets" / "scenes" };
            if (fs::exists(scenePath)) {
                for (const auto& entry : fs::directory_iterator(scenePath)) {
                    const auto& path = entry.path();
                    if (FileFormat::IsSceneFile(path)) {
                        std::string sceneName{ path.filename().replace_extension().string() };
                        auto sceneManager = SceneManager::Get();
                        if (ImGui::MenuItem(sceneName.c_str(), nullptr, sceneName == sceneManager->getScene()->getName())) {
                            auto scene = std::make_unique<Scene>(sceneName);
                            scene->deserialise();
                            sceneManager->setScene(std::move(scene));
                        }
                    }
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Entity")) {
            // TODO::

            ImGui::EndMenu();
        }

        ImGuiStyle& style = ImGui::GetStyle();

        if (ImGui::BeginMenu("Window")) {
            auto window = DeviceManager::Get()->getWindow(0);

            if (ImGui::MenuItem("Fullscreen", "F11", &projectSettings.isFullscreen, true)) {
                window->setFullscreen(projectSettings.isFullscreen, nullptr);
            }

            if (ImGui::MenuItem("Borderless", "", &projectSettings.isBorderless, true)) {
                window->setBorderless(projectSettings.isBorderless);
            }

            if (ImGui::MenuItem("Resizable", "", &projectSettings.isResizable, true)) {
                window->setResizable(projectSettings.isResizable);
            }

            if (ImGui::MenuItem("Floating", "", &projectSettings.isFloating, true)) {
                window->setFloating(projectSettings.isFloating);
            }

            if (ImGui::MenuItem("VSync", "", &projectSettings.isVSync, true)) {
                window->setVSync(projectSettings.isVSync);
            }

            ImGui::Separator();

            if (ImGui::BeginMenu("Style")) {
                constexpr auto entries = me::enum_entries<ImGuiUtils::Theme>();
                for (const auto& [type, title] : entries) {
                    if (ImGui::MenuItem(title.data(), "", editorSettings.theme == type)) {
                        editorSettings.theme = type;
                        ImGuiUtils::SetTheme(type);
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("About")) {
            auto version = Engine::Get()->getVersion();

            ImGui::Text("Version : %s", version.toString().c_str());
            ImGui::Separator();

            if (ImGui::MenuItem(ICON_MDI_PROJECTOR_SCREEN " Screenshot")) {
                Graphics::Get()->captureScreenshot("screenshot_" + DateTime::Get("%Y_%m_%d_%H_%M_%S") + ".png");
            }

            if (ImGui::MenuItem(ICON_MDI_GITHUB_BOX " Github")) {
#if FUSION_PLATFORM_WINDOWS
                system("start " GIT_URL);
#elif FUSION_PLATFORM_APPLE
                system("open " GIT_URL);
#elif FUSION_PLATFORM_LINUX
                system("xdg-open " GIT_URL);
#endif
            }

            ImGui::EndMenu();
        }

        //_____________________________________________________________________________________//

        ImGui::SameLine((ImGui::GetWindowContentRegionMax().x * 0.5f) - (1.5f * (ImGui::GetFontSize() + style.ItemSpacing.x)));

        if (editorState == EditorState::Next)
            editorState = EditorState::Paused;

        bool selected;

        {
            selected = editorState == EditorState::Play;
            if (ImGuiUtils::ToggleButton(ICON_MDI_STEP_FORWARD, selected, true)) {
                /*Application::Get().GetSystem<PhysicsEngine>()->SetPaused(selected);
                Application::Get().GetSystem<AudioManager>()->UpdateListener(Application::Get().GetCurrentScene());
                Application::Get().GetSystem<AudioManager>()->SetPaused(selected);*/

                editorState = selected ? EditorState::Play : EditorState::Preview;
                selectedEntity = entt::null;

                if (selected) {
                    ImGui::SetWindowFocus("###game");
                    //cacheScene();
                    //Application::Get().GetCurrentScene()->OnInit();
                } else {
                    ImGui::SetWindowFocus("###scene");
                    //loadCachedScene();
                }
            }
            ImGuiUtils::Tooltip("Play");
        }

        ImGui::SameLine();

        {
            selected = editorState == EditorState::Paused;
            if (ImGuiUtils::ToggleButton(ICON_MDI_PAUSE, selected, true))
                editorState = selected ? EditorState::Paused : EditorState::Play;
            ImGuiUtils::Tooltip("Pause");
        }

        ImGui::SameLine();

        {
            selected = editorState == EditorState::Next;
            if (ImGuiUtils::ToggleButton(ICON_MDI_STEP_FORWARD, selected, true))
                editorState = EditorState::Next;
            ImGuiUtils::Tooltip("Next");
        }

        auto size = ImGui::CalcTextSize("%.2f ms (%.i FPS)");
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - size.x - style.ItemSpacing.x * 2.0f);

        ImGui::Text("%.2f ms (%.i FPS)", Time::DeltaTime().asMilliseconds(), Time::FramesPerSecond());

        //_____________________________________________________________________________________//

        ImGui::EndMainMenuBar();
    }

    if (openSaveScenePopup)
        ImGui::OpenPopup("Save Scene");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2{0.5f, 0.5f});
    const ImVec2 buttonSize{120, 0};

    if (locationPopupOpened) {
        // Cancel clicked on project location popups
        if (!fileBrowserPanel.isOpened()) {
            newProjectPopupOpen = false;
            locationPopupOpened = false;
            reopenNewProjectPopup = true;
        }
    }

    if (openNewScenePopup)
        ImGui::OpenPopup("New Scene");

    if ((reopenNewProjectPopup || openProjectLoadPopup) && !newProjectPopupOpen) {
        ImGui::OpenPopup("Open Project");
        reopenNewProjectPopup = false;
    }

    if (ImGui::BeginPopupModal("Open Project", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::Button("Load Project")) {
            ImGui::CloseCurrentPopup();

            newProjectPopupOpen = true;
            locationPopupOpened = true;

            // Set path to working directory
            fileBrowserPanel.setFileTypeFilters({ ".fsproj" });
            fileBrowserPanel.setOpenDirectory(false);
            fileBrowserPanel.setCallback(BIND_FILEBROWSER_FN(projectOpenCallback));
            fileBrowserPanel.open();
        }

        ImGui::Separator();

        ImGui::TextUnformatted("Create New Project?\n");

        static std::string newProjectName = "New Project";
        ImGuiUtils::InputText("##ProjectName", newProjectName);

        if (ImGui::Button(ICON_MDI_FOLDER)) {
            ImGui::CloseCurrentPopup();

            newProjectPopupOpen = true;
            locationPopupOpened = true;

            // Set path to working directory
            fileBrowserPanel.clearFileTypeFilters();
            fileBrowserPanel.setOpenDirectory(true);
            fileBrowserPanel.setCallback(BIND_FILEBROWSER_FN(newProjectLocationCallback));
            fileBrowserPanel.open();
        }

        ImGui::SameLine();

        ImGui::TextUnformatted(projectLocation.empty() ? fs::current_path().string().c_str() : projectLocation.c_str());

        ImGui::Separator();

        if (ImGui::Button("Create", buttonSize)) {
            openNewProject(projectLocation, newProjectName);
            fileBrowserPanel.setOpenDirectory(false);

            for (auto& panel : panels) {
                panel->onNewProject();
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Exit", buttonSize)) {
            ImGui::CloseCurrentPopup();
            Engine::Get()->requestClose();
        }

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Save Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Save Current Scene Changes?\n\n");
        ImGui::Separator();

        if (ImGui::Button("OK", buttonSize)) {
            SceneManager::Get()->getScene()->serialise();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", buttonSize)) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("New Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::Button("Save Current Scene Changes")) {
            SceneManager::Get()->getScene()->serialise();
        }

        ImGui::Text("Create New Scene?\n\n");
        ImGui::Separator();

        if (ImGui::Button("OK", buttonSize)) {
            fs::path scenePath{ VirtualFileSystem::Get()->resolvePhysicalPath("Scenes") };
            if (!scenePath.empty()) {
                scenePath = FileFormat::GetNextFileName(scenePath);
                std::string sceneName{ scenePath.filename().replace_extension().string() };
                SceneManager::Get()->setScene(std::make_unique<Scene>(sceneName));
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", buttonSize)) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (openReloadScenePopup)
        ImGui::OpenPopup("Reload Scene");

    if (ImGui::BeginPopupModal("Reload Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Reload Scene?\n\n");
        ImGui::Separator();

        if (ImGui::Button("OK", buttonSize)) {
            auto sceneManager = SceneManager::Get()->getScene();
            sceneManager->clearEntities();
            sceneManager->deserialise();

            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", buttonSize)) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void Editor::openFile() {
    fileBrowserPanel.setCallback(BIND_FILEBROWSER_FN(fileOpenCallback));
    fileBrowserPanel.open();
}

void Editor::fileOpenCallback(const fs::path& path) {
    if (FileFormat::IsTextFile(path)) {
        openTextFile(path, []{});
    } else if (FileFormat::IsModelFile(path)) {
    } else if (FileFormat::IsAudioFile(path)) {
    } else if (FileFormat::IsSceneFile(path)) {
        /*auto sceneManager = SceneManager::Get()->getScene();
        sceneManager->serialise();

        auto scene = std::make_unique<Scene>(path.filename().string());
        scene->deserialise();
        sceneManager->setScene(std::move(scene));

        sceneManager->deserialise(path);
        sceneManager->serialise();*/
    } else if (FileFormat::IsTextureFile(path)) {
    }

    LOG_DEBUG << "File opened: \"" << path << "\"";
}

void Editor::projectOpenCallback(const fs::path& path) {
    newProjectPopupOpen = false;
    reopenNewProjectPopup = false;
    locationPopupOpened = false;
    fileBrowserPanel.clearFileTypeFilters();
    openProject(path);

    for (auto& panel : panels) {
        panel->onNewProject();
    }

    LOG_DEBUG << "Project opened: \"" << path << "\"";
}

void Editor::newProjectOpenCallback(const fs::path& path) {
    openNewProject(path);
    fileBrowserPanel.setOpenDirectory(false);

    for (auto& panel : panels) {
        panel->onNewProject();
    }

    LOG_DEBUG << "New project opened: \"" << path << "\"";
}

void Editor::newProjectLocationCallback(const fs::path& path) {
    projectLocation = path;
    newProjectPopupOpen = false;
    reopenNewProjectPopup = true;
    locationPopupOpened = false;

    LOG_DEBUG << "New Project opened: \"" << path << "\"";
}

void Editor::removePanel(EditorPanel* panel) {
    for (const auto& [i, p] : enumerate(panels)) {
        if (p.get() == panel) {
            panels.erase(panels.begin() + i);
            return;
        }
    }
}

void Editor::openTextFile(const fs::path& filepath, const std::function<void()>& callback) {
    for (const auto& [i, p] : enumerate(panels)) {
        if (p->getName() == "TextEdit") {
            panels.erase(panels.begin() + i);
            break;
        }
    }

    panels.emplace_back(std::make_unique<TextEditPanel>(filepath, callback, this));
}

EditorPanel* Editor::getPanel(const std::string& name) {
    for (auto& panel : panels) {
        if (panel->getName() == name) {
            return panel.get();
        }
    }
    return nullptr;
}