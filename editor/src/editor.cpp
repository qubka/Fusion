#include "editor.hpp"
#include "main_renderer.hpp"

#include "fusion/devices/devices.hpp"
#include "fusion/graphics/graphics.hpp"
#include "fusion/graphics/cameras/editor_camera.hpp"
#include "fusion/scene/components.hpp"
#include "fusion/scene/scenes.hpp"
#include "fusion/utils/glm_extention.hpp"
#include "fusion/utils/string.hpp"

#include "panels/application_info_panel.hpp"
#include "panels/console_panel.hpp"

#include <portable-file-dialogs/portable-file-dialogs.h>

#include <imgui/imgui.h>
#include <imguizmo/ImGuizmo.h>

using namespace fe;

Editor::Editor(const std::string& name, const Version& version) : App{name, version} {
}

Editor::~Editor() {
}

void Editor::onStart() {
    Graphics::Get()->setRenderer(std::make_unique<MainRenderer>());

    editorCamera = std::make_shared<EditorCamera>();

    activeScene = std::make_shared<Scene>(editorCamera);
    Scenes::Get()->setScene(activeScene);

    /*auto commandLineArgs = Application::Instance().getCommandLineArgs();
    if (commandLineArgs.count > 1) {
        auto sceneFilePath = commandLineArgs[1];
        SceneSerializer serializer{activeScene};
        serializer.deserialize(sceneFilePath);
    }*/
    fileBrowserPanel.setCallback([](auto path){

    });
    fileBrowserPanel.open();
    //contentBrowserPanel.onStart();
    //sceneHierarchyPanel.setContext(activeScene);

    /*componentIconMap[typeid(TransformComponent)] = ICON_FA_YELP;
    componentIconMap[typeid(TransformComponent)] = ICON_FA_YELP;
    componentIconMap[typeid(MeshComponent)] = ICON_FA_CODEPEN;
    componentIconMap[typeid(RigidBodyComponent)] = ICON_FA_CUBE;
    componentIconMap[typeid(PhysicsMaterialComponent)] = ICON_FA_TENCENT_WEIBO;
    componentIconMap[typeid(BoxColliderComponent)] = ICON_FA_SQUARE_O;
    componentIconMap[typeid(SphereColliderComponent)] = ICON_FA_CIRCLE_O;
    componentIconMap[typeid(CapsuleColliderComponent)] = ICON_FA_TOGGLE_OFF;
    componentIconMap[typeid(MaterialComponent)] = ICON_FA_DELICIOUS;*/

   panels.push_back(std::make_unique<ApplicationInfoPanel>());
   panels.push_back(std::make_unique<ConsolePanel>());

    for(auto& panel : panels)
        panel->setEditor(this);
}

void Editor::onUpdate() {

}

void Editor::onImGui() {
    //ImGui::ShowDemoWindow();
    fileBrowserPanel.onImGui();

    for(auto& panel : panels) {
        if(panel->isEnabled())
            panel->onImGui();
    }
}

/*void Editor::drawMenuBar() {
    bool openSaveScenePopup = false;
    bool openNewScenePopup = false;
    bool openReloadScenePopup = false;
    bool openProjectLoadPopup = !m_ProjectLoaded;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open Project")) {
                reopenNewProjectPopup = false;
                openProjectLoadPopup = true;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Open File")) {
                m_FileBrowserPanel.SetCurrentPath(m_ProjectSettings.m_ProjectRoot);
                m_FileBrowserPanel.SetCallback(BIND_FILEBROWSER_FN(Editor::FileOpenCallback));
                m_FileBrowserPanel.Open();
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

            if (ImGui::BeginMenu("Style")) {
                if (ImGui::MenuItem("Dark", "", m_Settings.m_Theme == ImGuiUtilities::Dark)) {
                    m_Settings.m_Theme = ImGuiUtilities::Dark;
                    ImGuiUtilities::SetTheme(ImGuiUtilities::Dark);
                    OS::Instance()->SetTitleBarColor(ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("Dracula", "", m_Settings.m_Theme == ImGuiUtilities::Dracula)) {
                    m_Settings.m_Theme = ImGuiUtilities::Dracula;
                    ImGuiUtilities::SetTheme(ImGuiUtilities::Dracula);
                    OS::Instance()->SetTitleBarColor(ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("Black", "", m_Settings.m_Theme == ImGuiUtilities::Black)) {
                    m_Settings.m_Theme = ImGuiUtilities::Black;
                    ImGuiUtilities::SetTheme(ImGuiUtilities::Black);
                    OS::Instance()->SetTitleBarColor(ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("Grey", "", m_Settings.m_Theme == ImGuiUtilities::Grey)) {
                    m_Settings.m_Theme = ImGuiUtilities::Grey;
                    ImGuiUtilities::SetTheme(ImGuiUtilities::Grey);
                    OS::Instance()->SetTitleBarColor(ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("Light", "", m_Settings.m_Theme == ImGuiUtilities::Light)) {
                    m_Settings.m_Theme = ImGuiUtilities::Light;
                    ImGuiUtilities::SetTheme(ImGuiUtilities::Light);
                    OS::Instance()->SetTitleBarColor(ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("Cherry", "", m_Settings.m_Theme == ImGuiUtilities::Cherry)) {
                    m_Settings.m_Theme = ImGuiUtilities::Cherry;
                    ImGuiUtilities::SetTheme(ImGuiUtilities::Cherry);
                    OS::Instance()->SetTitleBarColor(ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("Blue", "", m_Settings.m_Theme == ImGuiUtilities::Blue)) {
                    m_Settings.m_Theme = ImGuiUtilities::Blue;
                    ImGuiUtilities::SetTheme(ImGuiUtilities::Blue);
                    OS::Instance()->SetTitleBarColor(ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("Cinder", "", m_Settings.m_Theme == ImGuiUtilities::Cinder)) {
                    m_Settings.m_Theme = ImGuiUtilities::Cinder;
                    ImGuiUtilities::SetTheme(ImGuiUtilities::Cinder);
                    OS::Instance()->SetTitleBarColor(ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("Classic", "", m_Settings.m_Theme == ImGuiUtilities::Classic)) {
                    m_Settings.m_Theme = ImGuiUtilities::Classic;
                    ImGuiUtilities::SetTheme(ImGuiUtilities::Classic);
                    OS::Instance()->SetTitleBarColor(ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("ClassicDark", "", m_Settings.m_Theme == ImGuiUtilities::ClassicDark)) {
                    m_Settings.m_Theme = ImGuiUtilities::ClassicDark;
                    ImGuiUtilities::SetTheme(ImGuiUtilities::ClassicDark);
                    OS::Instance()->SetTitleBarColor(ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
                }
                if (ImGui::MenuItem("ClassicLight", "", m_Settings.m_Theme == ImGuiUtilities::ClassicLight)) {
                    m_Settings.m_Theme = ImGuiUtilities::ClassicLight;
                    ImGuiUtilities::SetTheme(ImGuiUtilities::ClassicLight);
                    OS::Instance()->SetTitleBarColor(ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
                }
                ImGui::EndMenu();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit")) {
                Application::Get().SetAppState(AppState::Closing);
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            // TODO
            //  if(ImGui::MenuItem("Undo", "CTRL+Z"))
            //  {
            //  }
            //  if(ImGui::MenuItem("Redo", "CTRL+Y", false, false))
            //  {
            //  } // Disabled item
            //  ImGui::Separator();

            bool enabled = m_SelectedEntity != entt::null;

            if (ImGui::MenuItem("Cut", "CTRL+X", false, enabled)) {
                m_CopiedEntity = m_SelectedEntity;
                m_CutCopyEntity = true;
            }

            if (ImGui::MenuItem("Copy", "CTRL+C", false, enabled)) {
                m_CopiedEntity = m_SelectedEntity;
                m_CutCopyEntity = false;
            }

            enabled = m_CopiedEntity != entt::null;

            if (ImGui::MenuItem("Paste", "CTRL+V", false, enabled)) {
                Application::Get().GetCurrentScene()->DuplicateEntity(
                        { m_CopiedEntity, Application::Get().GetCurrentScene() });
                if (m_CutCopyEntity) {
                    if (m_CopiedEntity == m_SelectedEntity)
                        m_SelectedEntity = entt::null;
                    Entity(m_CopiedEntity, Application::Get().GetCurrentScene()).Destroy();
                }
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Panels")) {
            for (auto& panel: m_Panels) {
                if (ImGui::MenuItem(panel->GetName().c_str(), "", &panel->Active(), true)) {
                    panel->SetActive(true);
                }
            }

            if (ImGui::MenuItem("ImGui Demo", "", &m_Settings.m_ShowImGuiDemo, true)) {
                m_Settings.m_ShowImGuiDemo = true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scenes")) {
            auto scenes = Application::Get().GetSceneManager()->GetSceneNames();

            for (size_t i = 0; i < scenes.size(); i++) {
                auto name = scenes[i];
                if (ImGui::MenuItem(name.c_str())) {
                    Application::Get().GetSceneManager()->SwitchScene(name);
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Entity")) {
            auto scene = Application::Get().GetSceneManager()->GetCurrentScene();

            if (ImGui::MenuItem("CreateEmpty")) {
                scene->CreateEntity();
            }

            if (ImGui::MenuItem("Cube")) {
                auto entity = scene->CreateEntity("Cube");
                entity.AddComponent<Graphics::ModelComponent>(Graphics::PrimitiveType::Cube);
            }

            if (ImGui::MenuItem("Sphere")) {
                auto entity = scene->CreateEntity("Sphere");
                entity.AddComponent<Graphics::ModelComponent>(Graphics::PrimitiveType::Sphere);
            }

            if (ImGui::MenuItem("Pyramid")) {
                auto entity = scene->CreateEntity("Pyramid");
                entity.AddComponent<Graphics::ModelComponent>(Graphics::PrimitiveType::Pyramid);
            }

            if (ImGui::MenuItem("Plane")) {
                auto entity = scene->CreateEntity("Plane");
                entity.AddComponent<Graphics::ModelComponent>(Graphics::PrimitiveType::Plane);
            }

            if (ImGui::MenuItem("Cylinder")) {
                auto entity = scene->CreateEntity("Cylinder");
                entity.AddComponent<Graphics::ModelComponent>(Graphics::PrimitiveType::Cylinder);
            }

            if (ImGui::MenuItem("Capsule")) {
                auto entity = scene->CreateEntity("Capsule");
                entity.AddComponent<Graphics::ModelComponent>(Graphics::PrimitiveType::Capsule);
            }

            if (ImGui::MenuItem("Terrain")) {
                auto entity = scene->CreateEntity("Terrain");
                entity.AddComponent<Graphics::ModelComponent>(Graphics::PrimitiveType::Terrain);
            }

            if (ImGui::MenuItem("Light Cube")) {
                EntityFactory::AddLightCube(Application::Get().GetSceneManager()->GetCurrentScene(), glm::vec3(0.0f), glm::vec3(0.0f));
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Graphics")) {
            if (ImGui::MenuItem("Compile Shaders")) {
                RecompileShaders();
            }
            if (ImGui::MenuItem("Embed Shaders")) {
                std::string coreDataPath;
                VFS::Get().ResolvePhysicalPath("//CoreShaders", coreDataPath, true);
                auto shaderPath = std::filesystem::path(coreDataPath + "/CompiledSPV/");
                int shaderCount = 0;
                if (std::filesystem::is_directory(shaderPath)) {
                    for (auto entry: std::filesystem::directory_iterator(shaderPath)) {
                        auto extension = StringUtilities::GetFilePathExtension(entry.path().string());
                        if (extension == "spv") {
                            EmbedShader(entry.path().string());
                            shaderCount++;
                        }
                    }
                }
                LUMOS_LOG_INFO("Embedded {0} shaders. Recompile to use", shaderCount);
            }
            if (ImGui::MenuItem("Embed File")) {
                EmbedFile();
            }

            if (ImGui::BeginMenu("GPU Index")) {
                uint32_t gpuCount = Graphics::Renderer::GetRenderer()->GetGPUCount();

                if (gpuCount == 1) {
                    ImGui::TextUnformatted("Default");
                    ImGuiUtilities::Tooltip("Only default GPU selectable");
                } else {
                    int8_t currentDesiredIndex = Application::Get().GetProjectSettings().DesiredGPUIndex;
                    int8_t newIndex = currentDesiredIndex;

                    if (ImGui::Selectable("Default", currentDesiredIndex == -1)) {
                        newIndex = -1;
                    }

                    for (uint32_t index = 0; index < gpuCount; index++) {
                        if (ImGui::Selectable(std::to_string(index).c_str(), index == uint32_t(currentDesiredIndex))) {
                            newIndex = index;
                        }
                    }

                    Application::Get().GetProjectSettings().DesiredGPUIndex = newIndex;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("About")) {
            auto& version = Lumos::LumosVersion;
            ImGui::Text("Version : %d.%d.%d", version.major, version.minor, version.patch);
            ImGui::Separator();

            std::string githubMenuText = ICON_MDI_GITHUB_BOX
            " Github";
            if (ImGui::MenuItem(githubMenuText.c_str())) {
#ifdef LUMOS_PLATFORM_WINDOWS
                // TODO
                // ShellExecuteA( NULL, "open",  "https://www.github.com/jmorton06/Lumos", NULL, NULL, SW_SHOWNORMAL );
#else
#ifndef LUMOS_PLATFORM_IOS
                system("open https://www.github.com/jmorton06/Lumos");
#endif
#endif
            }

            ImGui::EndMenu();
        }

        ImGui::SameLine((ImGui::GetWindowContentRegionMax().x * 0.5f) -
                        (1.5f * (ImGui::GetFontSize() + ImGui::GetStyle().ItemSpacing.x)));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.2f, 0.7f, 0.0f));

        if (Application::Get().GetEditorState() == EditorState::Next)
            Application::Get().SetEditorState(EditorState::Paused);

        bool selected;
        {
            selected = Application::Get().GetEditorState() == EditorState::Play;
            if (selected)
                ImGui::PushStyleColor(ImGuiCol_Text, ImGuiUtilities::GetSelectedColor());

            if (ImGui::Button(ICON_MDI_PLAY)) {
                Application::Get().GetSystem<LumosPhysicsEngine>()->SetPaused(selected);
                Application::Get().GetSystem<B2PhysicsEngine>()->SetPaused(selected);

                Application::Get().GetSystem<AudioManager>()->UpdateListener(Application::Get().GetCurrentScene());
                Application::Get().GetSystem<AudioManager>()->SetPaused(selected);
                Application::Get().SetEditorState(selected ? EditorState::Preview : EditorState::Play);

                m_SelectedEntity = entt::null;
                if (selected) {
                    ImGui::SetWindowFocus("###scene");
                    LoadCachedScene();
                } else {
                    ImGui::SetWindowFocus("###game");
                    CacheScene();
                    Application::Get().GetCurrentScene()->OnInit();
                }
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Play");

            if (selected)
                ImGui::PopStyleColor();
        }

        ImGui::SameLine();

        {
            selected = Application::Get().GetEditorState() == EditorState::Paused;
            if (selected)
                ImGui::PushStyleColor(ImGuiCol_Text, ImGuiUtilities::GetSelectedColor());

            if (ImGui::Button(ICON_MDI_PAUSE))
                Application::Get().SetEditorState(selected ? EditorState::Play : EditorState::Paused);

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Pause");

            if (selected)
                ImGui::PopStyleColor();
        }

        ImGui::SameLine();

        {
            selected = Application::Get().GetEditorState() == EditorState::Next;
            if (selected)
                ImGui::PushStyleColor(ImGuiCol_Text, ImGuiUtilities::GetSelectedColor());

            if (ImGui::Button(ICON_MDI_STEP_FORWARD))
                Application::Get().SetEditorState(EditorState::Next);

            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Next");

            if (selected)
                ImGui::PopStyleColor();
        }

        static Engine::Stats stats = {};
        static float timer = 1.1f;
        timer += Engine::GetTimeStep().GetSeconds();

        if (timer > 1.0f) {
            timer = 0.0f;
            stats = Engine::Get().Statistics();
        }

        auto size = ImGui::CalcTextSize("%.2f ms (%.i FPS)");
        float sizeOfGfxAPIDropDown = ImGui::GetFontSize() * 8;
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - size.x - ImGui::GetStyle().ItemSpacing.x * 2.0f -
                        sizeOfGfxAPIDropDown);

        ImGui::Text("%.2f ms (%.i FPS)", stats.FrameTime, stats.FramesPerSecond);

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetColorU32(ImGuiCol_TitleBg));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(7, 2));

        bool setNewValue = false;
        static std::string RenderAPI = "";

        auto renderAPI = (Graphics::RenderAPI) m_ProjectSettings.RenderAPI;

        bool needsRestart = false;
        if (renderAPI != Graphics::GraphicsContext::GetRenderAPI()) {
            needsRestart = true;
        }

        switch (renderAPI) {
#ifdef LUMOS_RENDER_API_OPENGL
            case Graphics::RenderAPI::OPENGL:
            RenderAPI = "OpenGL";
            break;
#endif

#ifdef LUMOS_RENDER_API_VULKAN
            case Graphics::RenderAPI::VULKAN:
            RenderAPI = "Vulkan";
            break;
#endif

#ifdef LUMOS_RENDER_API_DIRECT3D
            case DIRECT3D:
            RenderAPI = "Direct3D";
            break;
#endif
            default:
                break;
        }

        int numSupported = 0;
#ifdef LUMOS_RENDER_API_OPENGL
        numSupported++;
#endif
#ifdef LUMOS_RENDER_API_VULKAN
        numSupported++;
#endif
#ifdef LUMOS_RENDER_API_DIRECT3D11
        numSupported++;
#endif
        const char* api[] = { "OpenGL", "Vulkan", "Direct3D11" };
        const char* current_api = RenderAPI.c_str();
        if (needsRestart)
            RenderAPI = "*" + RenderAPI;

        ImGui::PushItemWidth(-1.0f);
        if (ImGui::BeginCombo(
                "", current_api, 0)) // The second parameter is the label previewed before opening the combo.
        {
            for (int n = 0; n < numSupported; n++) {
                bool is_selected = (current_api == api[n]);
                if (ImGui::Selectable(api[n], current_api)) {
                    setNewValue = true;
                    current_api = api[n];
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (needsRestart)
            ImGuiUtilities::Tooltip("Restart needed to switch Render API");

        if (setNewValue) {
            m_ProjectSettings.RenderAPI = int(StringToRenderAPI(current_api));
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();

        ImGui::EndMainMenuBar();
    }

    if (openSaveScenePopup)
        ImGui::OpenPopup("Save Scene");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Save Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Save Current Scene Changes?\n\n");
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            Application::Get().GetSceneManager()->GetCurrentScene()->Serialise(
                    m_ProjectSettings.m_ProjectRoot + "Assets/Scenes/", false);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (locationPopupOpened) {
        // Cancel clicked on project location popups
        if (!m_FileBrowserPanel.IsOpen()) {
            m_NewProjectPopupOpen = false;
            locationPopupOpened = false;
            reopenNewProjectPopup = true;
        }
    }
    if (openNewScenePopup)
        ImGui::OpenPopup("New Scene");

    if ((reopenNewProjectPopup || openProjectLoadPopup) && !m_NewProjectPopupOpen) {
        ImGui::OpenPopup("Open Project");
        reopenNewProjectPopup = false;
    }

    if (ImGui::BeginPopupModal("New Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::Button("Save Current Scene Changes")) {
            Application::Get().GetSceneManager()->GetCurrentScene()->Serialise(
                    m_ProjectSettings.m_ProjectRoot + "ExampleProject/Assets/Scenes/", false);
        }

        ImGui::Text("Create New Scene?\n\n");
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            std::string sceneName = "NewScene";
            int sameNameCount = 0;
            auto sceneNames = m_SceneManager->GetSceneNames();

            while (FileSystem::FileExists("//Scenes/" + sceneName + ".lsn") ||
                   std::find(sceneNames.begin(), sceneNames.end(), sceneName) != sceneNames.end()) {
                sameNameCount++;
                sceneName = fmt::format("NewScene({0})", sameNameCount);
            }
            auto scene = new Scene(sceneName);
            Application::Get().GetSceneManager()->EnqueueScene(scene);
            Application::Get().GetSceneManager()->SwitchScene(
                    (int) (Application::Get().GetSceneManager()->GetScenes().size()) - 1);

            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Open Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::Button("Load Project")) {
            ImGui::CloseCurrentPopup();

            m_NewProjectPopupOpen = true;
            locationPopupOpened = true;

            // Set filePath to working directory
            const auto& path = OS::Instance()->GetExecutablePath();
            auto& browserPath = m_FileBrowserPanel.GetPath();
            browserPath = std::filesystem::path(path);
            m_FileBrowserPanel.SetFileTypeFilters({ ".lmproj" });
            m_FileBrowserPanel.SetOpenDirectory(false);
            m_FileBrowserPanel.SetCallback(BIND_FILEBROWSER_FN(ProjectOpenCallback));
            m_FileBrowserPanel.Open();
        }

        ImGui::Separator();

        ImGui::TextUnformatted("Create New Project?\n");

        static std::string newProjectName = "New Project";
        ImGuiUtilities::InputText(newProjectName);

        if (ImGui::Button(ICON_MDI_FOLDER)) {
            ImGui::CloseCurrentPopup();

            m_NewProjectPopupOpen = true;
            locationPopupOpened = true;

            // Set filePath to working directory
            const auto& path = OS::Instance()->GetExecutablePath();
            auto& browserPath = m_FileBrowserPanel.GetPath();
            browserPath = std::filesystem::path(path);
            m_FileBrowserPanel.ClearFileTypeFilters();
            m_FileBrowserPanel.SetOpenDirectory(true);
            m_FileBrowserPanel.SetCallback(BIND_FILEBROWSER_FN(NewProjectLocationCallback));
            m_FileBrowserPanel.Open();
        }

        ImGui::SameLine();

        ImGui::TextUnformatted(projectLocation.c_str());

        ImGui::Separator();

        if (ImGui::Button("Create", ImVec2(120, 0))) {
            Application::Get().OpenNewProject(projectLocation, newProjectName);
            m_FileBrowserPanel.SetOpenDirectory(false);

            for (int i = 0; i < int(m_Panels.size()); i++) {
                m_Panels[i]->OnNewProject();
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Exit", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            SetAppState(AppState::Closing);
        }
        ImGui::EndPopup();
    }

    if (openReloadScenePopup)
        ImGui::OpenPopup("Reload Scene");

    if (ImGui::BeginPopupModal("Reload Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Reload Scene?\n\n");
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            auto scene = new Scene("New Scene");
            Application::Get().GetSceneManager()->SwitchScene(
                    Application::Get().GetSceneManager()->GetCurrentSceneIndex());

            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}*/