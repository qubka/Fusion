#pragma once

#include "application.hpp"

#include "fusion/utils/cereal_extention.hpp"
#include "fusion/scene/scene_manager.hpp"
#include "fusion/filesystem/virtual_file_system.hpp"

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

namespace fe {
    struct ProjectSettings {
        std::string projectVersion{ "0.0.0.0" };
        fs::path projectRoot;
        std::string projectName;
        //fs::path engineAssetPath;
        std::string title{ "Fusion App" };
        glm::uvec2 size{ 1280, 720 };
        bool isBorderless{ false };
        bool isResizable{ true };
        bool isFloating{ false };
        bool isFullscreen{ false };
        bool isVSync{ false };
        bool isShowConsole{ true };
    };

    class DefaultApplication : public Application {
    public:
        explicit DefaultApplication(std::string name);
        ~DefaultApplication() override;

        void onStart() override;

        void openNewProject(const fs::path& path, const std::string& name = "New Project");
        void openProject(const fs::path& path);
        //void onViewportResize(const glm::uvec2& size);

        bool isProjectLoaded() const { return projectLoaded; }
        bool isConsoleOpened() const { return consoleOpened; }
        ProjectSettings& getProjectSettings() { return projectSettings; }

        void serialise();
        void deserialise();

        void mountPaths() const;
        void showConsole();

        template<typename Archive>
        void save(Archive& archive) const {
            archive(cereal::make_nvp("Project Version", projectSettings.projectVersion),
                    cereal::make_nvp("Project Root", projectSettings.projectRoot),
                    cereal::make_nvp("Project Name", projectSettings.projectName),
                    //cereal::make_nvp("Engine Asset Path", projectSettings.engineAssetPath),
                    cereal::make_nvp("Title", projectSettings.title),
                    cereal::make_nvp("Size", projectSettings.size),
                    cereal::make_nvp("Borderless", projectSettings.isBorderless),
                    cereal::make_nvp("Resizable", projectSettings.isResizable),
                    cereal::make_nvp("Floating", projectSettings.isFloating),
                    cereal::make_nvp("Fullscreen", projectSettings.isFullscreen),
                    cereal::make_nvp("VSync", projectSettings.isVSync),
                    cereal::make_nvp("Show Console", projectSettings.isShowConsole)
            );

            auto sceneManager = SceneManager::Get();
            archive(cereal::make_nvp("Scene Index", sceneManager->getCurrentSceneIndex()),
                    cereal::make_nvp("Scenes", SceneManager::Get()->getSceneFilePaths())
            );
        }

        template<typename Archive>
        void load(Archive& archive) {
            archive(cereal::make_nvp("Project Version", projectSettings.projectVersion),
                    cereal::make_nvp("Project Root", projectSettings.projectRoot),
                    cereal::make_nvp("Project Name", projectSettings.projectName),
                    //cereal::make_nvp("Engine Asset Path", projectSettings.engineAssetPath),
                    cereal::make_nvp("Title", projectSettings.title),
                    cereal::make_nvp("Size", projectSettings.size),
                    cereal::make_nvp("Borderless", projectSettings.isBorderless),
                    cereal::make_nvp("Resizable", projectSettings.isResizable),
                    cereal::make_nvp("Floating", projectSettings.isFloating),
                    cereal::make_nvp("Fullscreen", projectSettings.isFullscreen),
                    cereal::make_nvp("VSync", projectSettings.isVSync),
                    cereal::make_nvp("Show Console", projectSettings.isShowConsole)
            );

            auto sceneManager = SceneManager::Get();

            uint32_t sceneIndex;
            std::vector<std::string> sceneFilePaths;
            archive(cereal::make_nvp("Scene Index", sceneIndex),
                    cereal::make_nvp("Scenes", sceneFilePaths));

            if (sceneFilePaths.empty()) {
                sceneManager->enqueueScene("Empty Scene");
                sceneManager->switchScene(0);
            } else {
                for (auto& filePath : sceneFilePaths) {
                    auto path = VirtualFileSystem::Get()->resolvePhysicalPath(filePath);
                    sceneManager->enqueueSceneFromFile(path);
                }
                sceneManager->switchScene(sceneIndex);
            }
        }


    protected:
        fs::path executablePath;
        ProjectSettings projectSettings;
        bool projectLoaded{ false };
        bool consoleOpened{ false };
    };
}
