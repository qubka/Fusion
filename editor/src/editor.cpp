#include "editor.h"
#include "editor_renderer.h"

#include "fusion/core/engine.h"
#include "fusion/devices/device_manager.h"
#include "fusion/input/input.h"
#include "fusion/graphics/graphics.h"
#include "fusion/graphics/cameras/camera.h"
#include "fusion/scene/components.h"
#include "fusion/scene/scene_manager.h"
#include "fusion/filesystem/file_format.h"
#include "fusion/filesystem/file_system.h"
#include "fusion/geometry/ray.h"
#include "fusion/scripting/script_engine.h"
#include "fusion/filesystem/file_system.h"

#include "panels/application_info_panel.h"
#include "panels/console_panel.h"
#include "panels/content_browser_panel.h"
#include "panels/text_edit_panel.h"
#include "panels/hierarchy_panel.h"
#include "panels/inspector_panel.h"
#include "panels/scene_view_panel.h"
#include "panels/game_view_panel.h"
#include "panels/project_settings_panel.h"

#include <imgui/imgui.h>
#include <imguizmo/ImGuizmo.h>

#include <memory>

using namespace fe;

Editor::Editor(std::string_view name) : DefaultApplication{name} {

}

Editor::~Editor() {
}

void Editor::onStart() {
    DefaultApplication::onStart();

    Graphics::Get()->setRenderer(std::make_unique<EditorRenderer>());

    editorCamera = std::make_shared<Camera>();

    componentIconMap[type_id<TransformComponent>] = ICON_MDI_AXIS_ARROW;
    //componentIconMap[type_id<ModelComponent>] = ICON_MDI_SHAPE;
    componentIconMap[type_id<MeshComponent>] = ICON_MDI_SHAPE;
    componentIconMap[type_id<CameraComponent>] = ICON_MDI_CAMERA;
    componentIconMap[type_id<LightComponent>] = ICON_MDI_LIGHTBULB;
    componentIconMap[type_id<RigidbodyComponent>] =  ICON_MDI_WEBPACK;
    componentIconMap[type_id<PhysicsMaterialComponent>] = ICON_MDI_GRADIENT;
    componentIconMap[type_id<BoxColliderComponent>] = ICON_MDI_SQUARE_OUTLINE;
    componentIconMap[type_id<SphereColliderComponent>] = ICON_MDI_CIRCLE_OUTLINE;
    componentIconMap[type_id<CapsuleColliderComponent>] = ICON_MDI_PILL;
    componentIconMap[type_id<MeshColliderComponent>] =  ICON_MDI_SHAPE_OUTLINE;
    componentIconMap[type_id<MaterialComponent>] = ICON_MDI_MATERIAL_UI;
    /*componentIconMap[type_id<PointLightComponent>] = ICON_MDI_LIGHTBULB;
    componentIconMap[type_id<DirectionalLightComponent>] = ICON_MDI_SPOTLIGHT_BEAM;
    componentIconMap[type_id<SoundComponent>] = ICON_MDI_VOLUME_HIGH;
    componentIconMap[type_id<SpriteComponent>] = ICON_MDI_IMAGE;
    componentIconMap[type_id<LuaScriptComponent>] = ICON_MDI_SCRIPT;
    componentIconMap[type_id<EnvironmentComponent>] = ICON_MDI_EARTH;*/
    componentIconMap[type_id<ScriptComponent>] = ICON_MDI_SCRIPT;
    componentIconMap[type_id<Editor>] = ICON_MDI_SQUARE;

    panels.push_back(std::make_unique<ApplicationInfoPanel>(*this));
    panels.push_back(std::make_unique<ConsolePanel>(*this));
    panels.push_back(std::make_unique<ContentBrowserPanel>(*this));
    panels.push_back(std::make_unique<ProjectSettingsPanel>(*this));
    panels.push_back(std::make_unique<HierarchyPanel>(*this));
    panels.push_back(std::make_unique<InspectorPanel>(*this));
    panels.push_back(std::make_unique<SceneViewPanel>(*this));
    panels.push_back(std::make_unique<GameViewPanel>(*this));

    editorSettings.showImGuiDemo = false;
}

void Editor::onUpdate() {
    auto scene = SceneManager::Get()->getScene();
    if (scene && sceneViewActive) {
        auto input = Input::Get();

        if (editorCamera) {
            editorCameraController.update(*editorCamera);

            if (input->getKeyDown(Key::F)) {
                auto& registry = scene->getRegistry();
                if (registry.valid(selectedEntity)) {
                    if (auto transform = registry.try_get<TransformComponent>(selectedEntity))
                        focusCamera(transform->getWorldPosition(), 5.0f, 0.2f);
                }
            }

            if (input->getKey(Key::O)) {
                focusCamera(vec3::zero, 5.0f, 0.2f);
            }

            if (transitioningCamera/* && glm::distance2(editorCamera->getEyePoint(), cameraDestination) > 1.0f*/) {
                transitioningCamera = editorCamera->setEyePoint(
                        glm::smoothdamp(editorCamera->getEyePoint(), cameraDestination, cameraVelocity, 0.15f,
                                        cameraTransitionMaxSpeed, Time::DeltaTime().asSeconds()));
            } else {
                cameraVelocity = vec3::zero;
            }

            if (input->getMouseScroll().y != 0 || input->getKey(Key::W) || input->getKey(Key::S) ||
                input->getKey(Key::A) || input->getKey(Key::D) || input->getKey(Key::Q) || input->getKey(Key::E)) {
                transitioningCamera = false;
            }
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
                    copiedEntity = entt::null;
                }
            }

            if (input->getKeyDown(Key::D) && selectedEntity != entt::null) {
                scene->duplicateEntity(selectedEntity);
            }

            if (input->getKeyDown(Key::Z)) {
            }

            if (input->getKeyDown(Key::Y)) {
            }
        }
    } else
        editorCameraController.stopMovement();

    DefaultApplication::onUpdate();
}

void Editor::onImGui() {
    beginDockSpace(editorSettings.fullScreenOnPlay && editorState == EditorState::Play);

    drawMenuBar();

    // Not range based loop
    for (size_t i = 0; i < panels.size(); ++i) {
        auto& panel = panels[i]; // Panel could change during onImGui
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
        ImGui::DockBuilderDockWindow("###projecteditorSettings", DockingBottomRightChild);
        ImGui::DockBuilderDockWindow("###hierarchy", DockLeft);
        ImGui::DockBuilderDockWindow("###textedit", DockMiddle);
        ImGui::DockBuilderDockWindow("###editoreditorSettings", DockingBottomRightChild);
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
                fileBrowserPanel.setCallback(BIND_EVENT_FN(fileOpenCallback));
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

            /*if (ImGui::MenuItem("Undo", "CTRL+Z", false, true)) {

            }

            if (ImGui::MenuItem("Redo", "CTRL+Y", false, true)) {

            }*/

            ImGui::EndMenu();
        }

#if FUSION_SCRIPTING
        if (ImGui::BeginMenu("Script")) {
            if (ImGui::MenuItem("Reload assembly", "Ctrl+T"))
                ScriptEngine::Get()->reloadAssembly();

            ImGui::EndMenu();
        }
#endif

        if (ImGui::BeginMenu("Panels")) {
            for (auto& panel : panels) {
                if (ImGui::MenuItem(panel->getTitle().c_str(), "", &panel->Active(), true)) {
                }
            }

            if (ImGui::MenuItem("ImGui Demo", "", &editorSettings.showImGuiDemo, true)) {
                editorSettings.showImGuiDemo = true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scenes")) {
            fs::path scenePath{ projectSettings.projectRoot / "assets/scenes" };
            if (fs::exists(scenePath)) {
                for (const auto& entry : fs::directory_iterator(scenePath)) {
                    const auto& path = entry.path();
                    if (FileFormat::IsSceneFile(path)) {
                        std::string sceneName{ path.filename().replace_extension().string() };
                        auto sceneManager = SceneManager::Get();
                        if (ImGui::MenuItem(sceneName.c_str(), nullptr, sceneName == sceneManager->getScene()->getName())) {
                            auto scene = std::make_unique<Scene>(std::move(sceneName));
                            scene->deserialise();
                            sceneManager->setScene(std::move(scene));
                        }
                    }
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Entity")) {

            auto scene = SceneManager::Get()->getScene();
            auto& registry = scene->getRegistry();

            if (ImGui::MenuItem("Create Empty")) {
                scene->createEntity();
            }

            if (ImGui::MenuItem("Light")) {
                auto entity = scene->createEntity("Light");
                registry.emplace<TransformComponent>(entity);
                registry.emplace<LightComponent>(entity);
            }

            ImGui::EndMenu();
        }

        ImGuiStyle& style = ImGui::GetStyle();

        if (ImGui::BeginMenu("Window")) {
#define window DeviceManager::Get()->getWindow(0)

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

        //if (editorState == EditorState::Next)
        //    editorState = EditorState::Paused;

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
                    SceneManager::Get()->cacheScene();
                } else {
                    ImGui::SetWindowFocus("###scene");
                    SceneManager::Get()->loadCachedScene();
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

        /*ImGui::SameLine();

        {
            selected = editorState == EditorState::Next;
            if (ImGuiUtils::ToggleButton(ICON_MDI_STEP_FORWARD, selected, true))
                editorState = EditorState::Next;
            ImGuiUtils::Tooltip("Next");
        }*/

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
            fileBrowserPanel.setCallback(BIND_EVENT_FN(projectOpenCallback));
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
            fileBrowserPanel.setCallback(BIND_EVENT_FN(newProjectLocationCallback));
            fileBrowserPanel.open();
        }

        ImGui::SameLine();

        ImGui::TextUnformatted(projectLocation.empty() ? fs::current_path().string().c_str() : projectLocation.string().c_str());

        ImGui::Separator();

        if (ImGui::Button("Create", buttonSize)) {
            openNewProject(projectLocation, newProjectName);
            fileBrowserPanel.setOpenDirectory(false);

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
            fs::path scenePath{ projectSettings.projectRoot / "assets/scenes/New Scene.fsn" };
            scenePath = FileFormat::GetNextFileName(scenePath);
            std::string sceneName{ scenePath.filename().replace_extension().string() };
            SceneManager::Get()->setScene(std::make_unique<Scene>(std::move(sceneName)));
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
    fileBrowserPanel.setCallback(BIND_EVENT_FN(fileOpenCallback));
    fileBrowserPanel.open();
}

void Editor::fileOpenCallback(const fs::path& path) {
    if (FileFormat::IsTextFile(path)) {
        openTextFile(path, []{});
    } else if (FileFormat::IsModelFile(path)) {
        SceneManager::Get()->getScene()->importMesh(path);
    } else if (FileFormat::IsAudioFile(path)) {
        ///
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

    FE_LOG_DEBUG("File opened: '{}'", path.lexically_relative(projectSettings.projectRoot));
}

void Editor::projectOpenCallback(const fs::path& path) {
    newProjectPopupOpen = false;
    reopenNewProjectPopup = false;
    locationPopupOpened = false;
    fileBrowserPanel.clearFileTypeFilters();
    openProject(path);

    FE_LOG_DEBUG("Project opened: '{}'", path);
}

/*void Editor::newProjectOpenCallback(const fs::path& path) {
    openNewProject(path);
    fileBrowserPanel.setOpenDirectory(false);

    LOG_DEBUG << "New project opened: '{}'", path);
}*/

void Editor::newProjectLocationCallback(const fs::path& path) {
    projectLocation = path;
    newProjectPopupOpen = false;
    reopenNewProjectPopup = true;
    locationPopupOpened = false;

    FE_LOG_DEBUG("New Project opened: '{}'", path);
}

void Editor::removePanel(EditorPanel* panel) {
    auto it = std::find_if(panels.begin(), panels.end(), [&panel](const auto& p) {
        return p.get() == panel;
    });
    if (it != panels.end())
        panels.erase(it);
}

void Editor::openTextFile(const fs::path& filepath, const std::function<void()>& callback) {
    auto it = std::find_if(panels.begin(), panels.end(), [](const auto& p) {
        return p->getName() == "TextEdit";
    });

    if (it != panels.end())
        *it = std::make_unique<TextEditPanel>(filepath, callback, *this);
    else
        panels.push_back(std::make_unique<TextEditPanel>(filepath, callback, *this));
}

EditorPanel* Editor::getPanel(std::string_view name) {
    auto it = std::find_if(panels.begin(), panels.end(), [&name](const auto& p) {
        return p->getName() == name;
    });
    return it != panels.end() ? it->get() : nullptr;
}

void Editor::focusCamera(const glm::vec3& point, float distance, float speed) {
    transitioningCamera = true;

    if (editorCamera->isOrthographic()) {
        cameraDestination = glm::vec3{point.x, editorCamera->getEyePoint().y, point.z};
    } else {
        cameraDestination = point - editorCamera->getForwardDirection() * distance;
    }

    cameraTransitionMaxSpeed = speed;
}

void Editor::selectObject(const Ray& ray, const glm::vec2& position) {
    auto scene = SceneManager::Get()->getScene();
    auto& registry = scene->getRegistry();
    auto view = registry.view<TransformComponent, MeshComponent>();

    float closestEntityDist = FLT_MAX;
    entt::entity currentClosestEntity = entt::null;

    for (const auto& [entity, transform, mesh] : view.each()) {
        auto filter = mesh.get();
        if (!filter)
            continue;

        const auto& worldTransform = transform.getWorldMatrix();

        auto bbCopy = filter->getBoundingBox().transformed(worldTransform);

        float min, max;
        if (bbCopy.intersect(ray, min, max) > 0) {
            if (min < closestEntityDist) {
                closestEntityDist = min;
                currentClosestEntity = entity;
            }
        }
    }

    auto now = DateTime::Now();

    if (selectedEntity != entt::null && selectedEntity == currentClosestEntity) {
        if (((now - lastSelectTime).asSeconds() < 0.5f) && glm::distance2(lastSelectPos, position) <= 1.0f) {
            auto& transform = registry.get<TransformComponent>(currentClosestEntity);
            auto& bb = registry.get<MeshComponent>(currentClosestEntity).get()->getBoundingBox();
            focusCamera(transform.getWorldPosition(), glm::distance(bb.getMin(), bb.getMax()));
        }
    } else {
        selectedEntity = currentClosestEntity;
    }

    lastSelectTime = now;
    lastSelectPos = position;
}

void Editor::onProjectLoad() {
    DefaultApplication::onProjectLoad();

    for (auto& panel : panels) {
        panel->onNewProject();
    }
}
