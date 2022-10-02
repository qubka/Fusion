#pragma once

#include "application.hpp"

#include "fusion/scene/scene_manager.hpp"
#include "fusion/filesystem/file_system.hpp"

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
            archive(cereal::make_nvp("project version", projectSettings.projectVersion));
            archive(cereal::make_nvp("project root", projectSettings.projectRoot));
            archive(cereal::make_nvp("project name", projectSettings.projectName));
            //archive(cereal::make_nvp("engine asset path", projectSettings.engineAssetPath));
            archive(cereal::make_nvp("title", projectSettings.title));
            archive(cereal::make_nvp("size", projectSettings.size));
            archive(cereal::make_nvp("borderless", projectSettings.isBorderless));
            archive(cereal::make_nvp("resizable", projectSettings.isResizable));
            archive(cereal::make_nvp("floating", projectSettings.isFloating));
            archive(cereal::make_nvp("fullscreen", projectSettings.isFullscreen));
            archive(cereal::make_nvp("vsync", projectSettings.isVSync));
            archive(cereal::make_nvp("show console", projectSettings.isShowConsole));
            archive(cereal::make_nvp("current scene", SceneManager::Get()->getScene()->getName())
            );
        }

        template<typename Archive>
        void load(Archive& archive) {
            std::string sceneName;
            archive(cereal::make_nvp("project version", projectSettings.projectVersion));
            archive(cereal::make_nvp("project root", projectSettings.projectRoot));
            archive(cereal::make_nvp("project name", projectSettings.projectName));
            //archive(cereal::make_nvp("Engine Asset Path", projectSettings.engineAssetPath));
            archive(cereal::make_nvp("title", projectSettings.title));
            archive(cereal::make_nvp("size", projectSettings.size));
            archive(cereal::make_nvp("borderless", projectSettings.isBorderless));
            archive(cereal::make_nvp("resizable", projectSettings.isResizable));
            archive(cereal::make_nvp("floating", projectSettings.isFloating));
            archive(cereal::make_nvp("fullscreen", projectSettings.isFullscreen));
            archive(cereal::make_nvp("vsync", projectSettings.isVSync));
            archive(cereal::make_nvp("show console", projectSettings.isShowConsole));
            archive(cereal::make_nvp("current scene", sceneName)
            );

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
