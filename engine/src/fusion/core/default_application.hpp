#pragma once

#include "application.hpp"

#include <cereal/cereal.hpp>

namespace fe {
    struct ProjectSettings {
        uint32_t projectVersion;
        std::filesystem::path projectRoot;
        std::filesystem::path projectName;
        std::filesystem::path engineAssetPath;
        std::string title;
        glm::uvec2 viewport;
        bool isBorderless;
        bool isResizable;
        bool isFloating;
        bool isFullscreen;
        bool isVSync;
        bool isShowConsole;
    };

    class DefaultApplication : public Application {
    public:
        explicit DefaultApplication(std::string name);
        ~DefaultApplication() override;

        void onStart() override;

        void openNewProject(const std::filesystem::path& path);
        void openProject(const std::filesystem::path& path);
        //void onViewportResize(const glm::uvec2& size);

        const ProjectSettings& getProjectSettings() const { return projectSettings; }
        const std::filesystem::path& getExecutablePath() const { return executablePath; }

        void serialise();
        void deserialise();

        template <typename Archive>
        void serialize(Archive& archive) const {
            cereal::make_nvp("Project Version", projectSettings.projectVersion);
            cereal::make_nvp("Project Root", projectSettings.projectRoot);
            cereal::make_nvp("Project Name", projectSettings.projectName);
            cereal::make_nvp("Engine Asset Path", projectSettings.engineAssetPath);
            cereal::make_nvp("Title", projectSettings.title);
            cereal::make_nvp("viewport", projectSettings.viewport);
            cereal::make_nvp("Borderless", projectSettings.isBorderless);
            cereal::make_nvp("Resizable", projectSettings.isResizable);
            cereal::make_nvp("Floating", projectSettings.isFloating);
            cereal::make_nvp("Fullscreen", projectSettings.isFullscreen);
            cereal::make_nvp("VSync", projectSettings.isVSync);
            cereal::make_nvp("Show Console", projectSettings.isShowConsole);
        }

    protected:
        ProjectSettings projectSettings;
        bool projectLoaded{ false };

        std::filesystem::path executablePath;
    };
}
