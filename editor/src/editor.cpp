#include "editor.hpp"
#include "main_renderer.hpp"

#include "fusion/core/engine.hpp"
#include "fusion/core/time.hpp"
#include "fusion/devices/device_manager.hpp"
#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/cameras/editor_camera.hpp"
#include "fusion/scene/components.hpp"
#include "fusion/scene/scene_manager.hpp"
#include "fusion/utils/enumerate.hpp"
#include "fusion/filesystem/file_format.hpp"
#include "fusion/filesystem/file_system.hpp"

#include "panels/application_info_panel.hpp"
#include "panels/console_panel.hpp"
#include "panels/content_browser_panel.hpp"
#include "panels/project_settings_panel.hpp"
#include "panels/text_edit_panel.hpp"
#include "panels/hierarchy_panel.hpp"

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

    Graphics::Get()->setRenderer(std::make_unique<MainRenderer>());

    componentIconMap[typeid(TransformComponent)] = ICON_MDI_AXIS_ARROW;
    componentIconMap[typeid(ModelComponent)] = ICON_MDI_SHAPE;
    //componentIconMap[typeid(MeshComponent)] = ICON_MDI_SHAPE;
    componentIconMap[typeid(CameraComponent)] = ICON_MDI_CAMERA;
    /*componentIconMap[typeid(RigidBodyComponent)] = ICON_MDI_CUBE_OUTLINE;
    componentIconMap[typeid(PhysicsMaterialComponent)] = ICON_FA_TENCENT_WEIBO;
    componentIconMap[typeid(BoxColliderComponent)] = ICON_FA_SQUARE_O;
    componentIconMap[typeid(SphereColliderComponent)] = ICON_FA_CIRCLE_O;
    componentIconMap[typeid(CapsuleColliderComponent)] = ICON_FA_TOGGLE_OFF;
    componentIconMap[typeid(MaterialComponent)] = ICON_MDI_MATERIAL_UI;
    componentIconMap[typeid(PointLightComponent)] = ICON_MDI_LIGHTBULB;
    componentIconMap[typeid(DirectionalLightComponent)] = ICON_MDI_SPOTLIGHT_BEAM;
    componentIconMap[typeid(SoundComponent)] = ICON_MDI_VOLUME_HIGH;
    componentIconMap[typeid(SpriteComponent)] = ICON_MDI_IMAGE;
    componentIconMap[typeid(LuaScriptComponent)] = ICON_MDI_SCRIPT;
    componentIconMap[typeid(EnvironmentComponent)] = ICON_MDI_EARTH;*/

    panels.push_back(std::make_unique<ApplicationInfoPanel>(this));
    panels.push_back(std::make_unique<ConsolePanel>(this));
    panels.push_back(std::make_unique<ContentBrowserPanel>(this));
    panels.push_back(std::make_unique<ProjectSettingsPanel>(this));
    panels.push_back(std::make_unique<HierarchyPanel>(this));

    editorSettings.showImGuiDemo = false;
}

void Editor::onUpdate() {

}

void Editor::onImGui() {
    beginDockSpace(editorSettings.fullScreenOnPlay && editorState == EditorState::Play);

    drawMenuBar();

    for (auto& panel : panels) {
        if (panel->isEnabled()) {
            panel->onImGui();
        }
    }

    if (editorSettings.showImGuiDemo)
        ImGui::ShowDemoWindow(&editorSettings.showImGuiDemo);

    fileBrowserPanel.onImGui();

    endDockSpace();
}

void Editor::beginDockSpace(bool gameFullScreen) {
    static bool p_open = true;
    static bool opt_fullscreen_persistant = true;
    static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton; // ImGuiDockNodeFlags_PassthruCentralNode
    bool opt_fullscreen = opt_fullscreen_persistant;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the
    // pass-thru hole, so we ask Begin() to not render a background.
    if (opt_flags & ImGuiDockNodeFlags_DockSpace || opt_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
    ImGui::Begin("DockSpace", &p_open, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    ImGuiID dockspaceID = ImGui::GetID("DockSpace");

    static std::vector<EditorPanel*> hiddenPanels;

    if (editorSettings.fullScreenSceneView != gameFullScreen) {
        editorSettings.fullScreenSceneView = gameFullScreen;

        if (editorSettings.fullScreenSceneView) {
            for (auto& panel : panels) {
                if (panel->getSimpleName() != "Game" && panel->isEnabled()) {
                    panel->setEnabled(false);
                    hiddenPanels.push_back(panel.get());
                }
            }
        } else {
            for (auto& panel: hiddenPanels) {
                panel->setEnabled(true);
            }

            hiddenPanels.clear();
        }
    }

    /*if (!ImGui::DockBuilderGetNode(dockspaceID)) {
        ImGui::DockBuilderRemoveNode(dockspaceID); // Clear out existing layout
        ImGui::DockBuilderAddNode(dockspaceID); // Add empty node
        ImGui::DockBuilderSetNodeSize(dockspaceID, ImGui::GetIO().DisplaySize * ImGui::GetIO().DisplayFramebufferScale);

        ImGuiID dock_main_id = dockspaceID;

        ImGuiID DockBottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.3f, nullptr, &dock_main_id);
        ImGuiID DockLeft = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
        ImGuiID DockRight = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, nullptr, &dock_main_id);

        ImGuiID DockLeftChild = ImGui::DockBuilderSplitNode(DockLeft, ImGuiDir_Down, 0.875f, nullptr, &DockLeft);
        ImGuiID DockRightChild = ImGui::DockBuilderSplitNode(DockRight, ImGuiDir_Down, 0.875f, nullptr, &DockRight);
        ImGuiID DockingLeftDownChild = ImGui::DockBuilderSplitNode(DockLeftChild, ImGuiDir_Down, 0.06f, nullptr, &DockLeftChild);
        ImGuiID DockingRightDownChild = ImGui::DockBuilderSplitNode(DockRightChild, ImGuiDir_Down, 0.06f, nullptr, &DockRightChild);

        ImGuiID DockBottomChild = ImGui::DockBuilderSplitNode(DockBottom, ImGuiDir_Down, 0.2f, nullptr, &DockBottom);
        ImGuiID DockingBottomLeftChild = ImGui::DockBuilderSplitNode(DockLeft, ImGuiDir_Down, 0.4f, nullptr, &DockLeft);
        ImGuiID DockingBottomRightChild = ImGui::DockBuilderSplitNode(DockRight, ImGuiDir_Down, 0.4f, nullptr, &DockRight);

        ImGuiID DockMiddle = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.8f, nullptr, &dock_main_id);
        ImGuiID DockBottomMiddle = ImGui::DockBuilderSplitNode(DockMiddle, ImGuiDir_Down, 0.3f, nullptr, &DockMiddle);
        ImGuiID DockMiddleLeft = ImGui::DockBuilderSplitNode(DockMiddle, ImGuiDir_Left, 0.5f, nullptr, &DockMiddle);
        ImGuiID DockMiddleRight = ImGui::DockBuilderSplitNode(DockMiddle, ImGuiDir_Right, 0.5f, nullptr, &DockMiddle);

        //ImGui::DockBuilderDockWindow("###game", DockMiddleRight);
        //ImGui::DockBuilderDockWindow("###scene", DockMiddleLeft);
        //ImGui::DockBuilderDockWindow("###inspector", DockRight);
        ImGui::DockBuilderDockWindow("###console", DockBottomMiddle);
        //ImGui::DockBuilderDockWindow("###profiler", DockingBottomLeftChild);
        ImGui::DockBuilderDockWindow("###content", DockingBottomLeftChild);
        //ImGui::DockBuilderDockWindow("Dear ImGui Demo", DockLeft);
        //ImGui::DockBuilderDockWindow("GraphicsInfo", DockLeft);
        ImGui::DockBuilderDockWindow("##appinfo", DockLeft);
        //ImGui::DockBuilderDockWindow("###hierarchy", DockLeft);
        //ImGui::DockBuilderDockWindow("###textEdit", DockMiddle);
        //ImGui::DockBuilderDockWindow("###scenesettings", DockLeft);

        ImGui::DockBuilderFinish(dockspaceID);
    }*/

    // Dockspace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGui::DockSpace(dockspaceID, ImVec2{0.0f, 0.0f}, opt_flags);
    }
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
                //Application::Get().GetCurrentScene()->DuplicateEntity({ copiedEntity, Application::Get().GetCurrentScene() });
                if (cutCopyEntity) {
                    if (copiedEntity == selectedEntity)
                        selectedEntity = entt::null;
                    //Entity(copiedEntity, Application::Get().GetCurrentScene()).Destroy();
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Panels")) {
            for (auto& panel : panels) {
                if (ImGui::MenuItem(panel->getName().c_str(), "", &panel->Enabled(), true)) {
                    panel->setEnabled(true);
                }
            }

            if (ImGui::MenuItem("ImGui Demo", "", &editorSettings.showImGuiDemo, true)) {
                editorSettings.showImGuiDemo = true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scenes")) {
            auto scenePath = projectSettings.projectRoot / "assets" / "scenes";
            if (fs::exists(scenePath)) {
                for (const auto& entry : fs::directory_iterator(scenePath)) {
                    const auto& path = entry.path();
                    if (FileFormat::IsSceneFile(path)) {
                        auto name = path.filename().replace_extension().string();
                        if (ImGui::MenuItem(name.c_str())) {
                            SceneManager::Get()->setScene(std::make_unique<Scene>(name));
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
                if (ImGui::MenuItem("Dark", "", editorSettings.theme == ImGuiUtils::Theme::Dark)) {
                    editorSettings.theme = ImGuiUtils::Theme::Dark;
                    ImGuiUtils::SetTheme(ImGuiUtils::Theme::Dark);
                    //setTitleBarColour(style.Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("Dracula", "", editorSettings.theme == ImGuiUtils::Theme::Dracula)) {
                    editorSettings.theme = ImGuiUtils::Theme::Dracula;
                    ImGuiUtils::SetTheme(ImGuiUtils::Theme::Dracula);
                    //setTitleBarColour(style.Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("Black", "", editorSettings.theme == ImGuiUtils::Theme::Black)) {
                    editorSettings.theme = ImGuiUtils::Theme::Black;
                    ImGuiUtils::SetTheme(ImGuiUtils::Theme::Black);
                    //setTitleBarColour(style.Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("Grey", "", editorSettings.theme == ImGuiUtils::Theme::Grey)) {
                    editorSettings.theme = ImGuiUtils::Theme::Grey;
                    ImGuiUtils::SetTheme(ImGuiUtils::Theme::Grey);
                    //setTitleBarColour(style.Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("Light", "", editorSettings.theme == ImGuiUtils::Theme::Light)) {
                    editorSettings.theme = ImGuiUtils::Theme::Light;
                    ImGuiUtils::SetTheme(ImGuiUtils::Theme::Light);
                    //setTitleBarColour(style.Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("Cherry", "", editorSettings.theme == ImGuiUtils::Theme::Cherry)) {
                    editorSettings.theme = ImGuiUtils::Theme::Cherry;
                    ImGuiUtils::SetTheme(ImGuiUtils::Theme::Cherry);
                    //setTitleBarColour(style.Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("Blue", "", editorSettings.theme == ImGuiUtils::Theme::Blue)) {
                    editorSettings.theme = ImGuiUtils::Theme::Blue;
                    ImGuiUtils::SetTheme(ImGuiUtils::Theme::Blue);
                    //setTitleBarColour(style.Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("Cinder", "", editorSettings.theme == ImGuiUtils::Theme::Cinder)) {
                    editorSettings.theme = ImGuiUtils::Theme::Cinder;
                    ImGuiUtils::SetTheme(ImGuiUtils::Theme::Cinder);
                    //setTitleBarColour(style.Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("Classic", "", editorSettings.theme == ImGuiUtils::Theme::Classic)) {
                    editorSettings.theme = ImGuiUtils::Theme::Classic;
                    ImGuiUtils::SetTheme(ImGuiUtils::Theme::Classic);
                    //setTitleBarColour(style.Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("ClassicDark", "", editorSettings.theme == ImGuiUtils::Theme::ClassicDark)) {
                    editorSettings.theme = ImGuiUtils::Theme::ClassicDark;
                    ImGuiUtils::SetTheme(ImGuiUtils::Theme::ClassicDark);
                    //setTitleBarColour(style.Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("ClassicLight", "", editorSettings.theme == ImGuiUtils::Theme::ClassicLight)) {
                    editorSettings.theme = ImGuiUtils::Theme::ClassicLight;
                    ImGuiUtils::SetTheme(ImGuiUtils::Theme::ClassicLight);
                    //setTitleBarColour(style.Colors[ImGuiCol_MenuBarBg]);
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("About")) {
            auto version = Engine::Get()->getVersion();

            ImGui::Text("Version : %s", version.string().c_str());
            ImGui::Separator();

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

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.2f, 0.7f, 0.0f});

        if (editorState == EditorState::Next)
            editorState = EditorState::Paused;

        bool selected;

        {
            selected = editorState == EditorState::Play;
            if (selected)
                ImGui::PushStyleColor(ImGuiCol_Text, ImGuiUtils::GetSelectedColor());

            if (ImGui::Button(ICON_MDI_PLAY)) {
                /*Application::Get().GetSystem<PhysicsEngine>()->SetPaused(selected);
                Application::Get().GetSystem<AudioManager>()->UpdateListener(Application::Get().GetCurrentScene());
                Application::Get().GetSystem<AudioManager>()->SetPaused(selected);*/

                editorState = (selected ? EditorState::Preview : EditorState::Play);

                selectedEntity = entt::null;
                if (selected) {
                    //ImGui::SetWindowFocus("###scene");
                    //loadCachedScene();
                } else {
                    //ImGui::SetWindowFocus("###game");
                    //cacheScene();
                    //Application::Get().GetCurrentScene()->OnInit();
                }
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Play");

            if (selected)
                ImGui::PopStyleColor();
        }

        ImGui::SameLine();

        {
            selected = editorState == EditorState::Paused;
            if (selected)
                ImGui::PushStyleColor(ImGuiCol_Text, ImGuiUtils::GetSelectedColor());

            if (ImGui::Button(ICON_MDI_PAUSE))
                editorState = selected ? EditorState::Play : EditorState::Paused;

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Pause");

            if (selected)
                ImGui::PopStyleColor();
        }

        ImGui::SameLine();

        {
            selected = editorState == EditorState::Next;
            if (selected)
                ImGui::PushStyleColor(ImGuiCol_Text, ImGuiUtils::GetSelectedColor());

            if (ImGui::Button(ICON_MDI_STEP_FORWARD))
                editorState = EditorState::Next;

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Next");

            if (selected)
                ImGui::PopStyleColor();
        }

        auto size = ImGui::CalcTextSize("%.2f ms (%.i FPS)");
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - size.x - style.ItemSpacing.x * 2.0f);

        ImGui::Text("%.2f ms (%.i FPS)", Time::DeltaTime().asMilliseconds(), Time::FramesPerSecond());

        ImGui::PopStyleColor();

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

    auto sceneManager = SceneManager::Get();

    if (ImGui::BeginPopupModal("Save Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Save Current Scene Changes?\n\n");
        ImGui::Separator();

        if (ImGui::Button("OK", buttonSize)) {
            sceneManager->getScene()->serialise();
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
            sceneManager->getScene()->serialise();
        }

        ImGui::Text("Create New Scene?\n\n");
        ImGui::Separator();

        if (ImGui::Button("OK", buttonSize)) {
            /*std::string sceneName = "NewScene";
            int sameNameCount = 0;
            auto sceneNames = sceneManager->getSceneNames();

            while (FileSystem::ExistsInPath("Scenes" / (sceneName + ".fsn"))) {
                sameNameCount++;
                sceneName = "NewScene(" + String::ToString(sameNameCount) + ")";
            }

            sceneManager->setScene(std::make_unique<Scene>());*/

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
            sceneManager->getScene()->deserialise();

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
    } else if (FileFormat::IsTextureFile(path)) {
    }

    LOG_DEBUG << "File opened: " << path;
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

    LOG_DEBUG << "Project opened: " << path;
}

void Editor::newProjectOpenCallback(const fs::path& path) {
    openNewProject(path);
    fileBrowserPanel.setOpenDirectory(false);

    for (auto& panel : panels) {
        panel->onNewProject();
    }

    LOG_DEBUG << "New project opened: " << path;
}

void Editor::newProjectLocationCallback(const fs::path& path) {
    projectLocation = path;
    newProjectPopupOpen = false;
    reopenNewProjectPopup = true;
    locationPopupOpened = false;

    LOG_DEBUG << "New Project opened: " << path;
}

void Editor::removePanel(EditorPanel* panel) {
    for (const auto& [i, p] : enumerate(panels)) {
        if (p.get() == panel) {
            panels.erase(panels.begin() + i);
            return;
        }
    }
}

void Editor::openTextFile(const fs::path& filepath, std::function<void()>&& callback) {
    for (const auto& [i, p] : enumerate(panels)) {
        if (p->getSimpleName() == "TextEdit") {
            panels.erase(panels.begin() + i);
            break;
        }
    }

    panels.emplace_back(std::make_unique<TextEditPanel>(filepath, std::move(callback), this));
}

EditorPanel* Editor::getTextEditPanel() {
    for (auto& panel : panels) {
        if (panel->getSimpleName() == "TextEdit") {
            return panel.get();
        }
    }
    return nullptr;
}