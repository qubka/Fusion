#pragma once

#include "application.h"

#include "fusion/scene/scene_manager.h"
#include "fusion/filesystem/file_system.h"

namespace fe {
    struct ProjectSettings {
        std::string projectVersion{ "0.0.0.0" };
        fs::path projectRoot;
        std::string projectName;
        //fs::path engineAssetPath;
        fs::path scriptModulePath;
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
        explicit DefaultApplication(std::string_view name);
        ~DefaultApplication() override;

    protected:
        void onStart() override;
        void onUpdate() override {};
        void onImGui() override {};
        void onStop() override {};

    public:
        void openNewProject(const fs::path& path, std::string_view name = "New Project");
        void openProject(const fs::path& path);
        //void onViewportResize(const glm::uvec2& size);

        bool isProjectLoaded() const { return projectLoaded; }
        bool isConsoleOpened() const { return consoleOpened; }
        ProjectSettings& getProjectSettings() { return projectSettings; }
        const fs::path& getRootPath() const override { return projectSettings.projectRoot; }

        void serialise();
        void deserialise();

        void showConsole();

        template<typename Archive>
        void save(Archive& archive) const {
            archive(cereal::make_nvp("projectVersion", projectSettings.projectVersion));
            archive(cereal::make_nvp("projectRoot", projectSettings.projectRoot));
            archive(cereal::make_nvp("projectName", projectSettings.projectName));
            //archive(cereal::make_nvp("engineAssetPath", projectSettings.engineAssetPath));
            archive(cereal::make_nvp("title", projectSettings.title));
            archive(cereal::make_nvp("size", projectSettings.size));
            archive(cereal::make_nvp("isBorderless", projectSettings.isBorderless));
            archive(cereal::make_nvp("isResizable", projectSettings.isResizable));
            archive(cereal::make_nvp("isFloating", projectSettings.isFloating));
            archive(cereal::make_nvp("isFullscreen", projectSettings.isFullscreen));
            archive(cereal::make_nvp("isVSync", projectSettings.isVSync));
            archive(cereal::make_nvp("isShowConsole", projectSettings.isShowConsole));
            archive(cereal::make_nvp("currentScene", SceneManager::Get()->getScene()->getName()));
        }

        template<typename Archive>
        void load(Archive& archive) {
            std::string sceneName;
            archive(cereal::make_nvp("projectVersion", projectSettings.projectVersion));
            archive(cereal::make_nvp("projectRoot", projectSettings.projectRoot));
            archive(cereal::make_nvp("projectName", projectSettings.projectName));
            //archive(cereal::make_nvp("engineAssetPath", projectSettings.engineAssetPath));
            archive(cereal::make_nvp("title", projectSettings.title));
            archive(cereal::make_nvp("size", projectSettings.size));
            archive(cereal::make_nvp("isBorderless", projectSettings.isBorderless));
            archive(cereal::make_nvp("isResizable", projectSettings.isResizable));
            archive(cereal::make_nvp("isFloating", projectSettings.isFloating));
            archive(cereal::make_nvp("isFullscreen", projectSettings.isFullscreen));
            archive(cereal::make_nvp("isVSync", projectSettings.isVSync));
            archive(cereal::make_nvp("isShowConsole", projectSettings.isShowConsole));
            archive(cereal::make_nvp("currentScene", sceneName));

            if (sceneName.empty()) {
                SceneManager::Get()->setScene(std::make_unique<Scene>("Empty Scene"));
            } else {
                auto scene = std::make_unique<Scene>(sceneName);
                scene->deserialise();
                SceneManager::Get()->setScene(std::move(scene));
            }
        }

    protected:
        fs::path executablePath;
        ProjectSettings projectSettings;
        bool projectLoaded{ false };
        bool consoleOpened{ false };
    };
}
