#pragma once

#include "application.h"

#include "fusion/scene/scene_manager.h"
#include "fusion/filesystem/file_system.h"

namespace fe {
    class FUSION_API DefaultApplication : public Application {
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

        void serialise();
        void deserialise();

        void showConsole();

        virtual void onProjectLoad();

        template<typename Archive>
        void save(Archive& archive) const {
            archive(cereal::make_nvp("projectVersion", projectSettings.projectVersion));
            archive(cereal::make_nvp("projectRoot", projectSettings.projectRoot));
            archive(cereal::make_nvp("projectName", projectSettings.projectName));
            archive(cereal::make_nvp("scriptModulePath", projectSettings.scriptModulePath));
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
            archive(cereal::make_nvp("scriptModulePath", projectSettings.scriptModulePath));
            archive(cereal::make_nvp("title", projectSettings.title));
            archive(cereal::make_nvp("size", projectSettings.size));
            archive(cereal::make_nvp("isBorderless", projectSettings.isBorderless));
            archive(cereal::make_nvp("isResizable", projectSettings.isResizable));
            archive(cereal::make_nvp("isFloating", projectSettings.isFloating));
            archive(cereal::make_nvp("isFullscreen", projectSettings.isFullscreen));
            archive(cereal::make_nvp("isVSync", projectSettings.isVSync));
            archive(cereal::make_nvp("isShowConsole", projectSettings.isShowConsole));
            archive(cereal::make_nvp("currentScene", currentScene));
        }

    protected:
        fs::path executablePath;
        std::string currentScene;
        bool projectLoaded{ false };
        bool consoleOpened{ false };
    };
}
