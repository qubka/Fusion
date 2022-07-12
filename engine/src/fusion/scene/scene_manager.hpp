#pragma once

#include "fusion/core/module.hpp"
#include "fusion/scene/scene.hpp"

namespace fe {
    /**
     * @brief Module used for managing game scenes.
     */
    class SceneManager : public Module::Registrar<SceneManager> {
    public:
        SceneManager();
        ~SceneManager() override;

        void onUpdate() override;

        /**
         * Jump to the next scene in the list or first scene if at the end
         */
        void switchScene();

        /**
         * Jump to scene index (stored in order they were originally added starting at zero)
         * @param id Scene index in the queue.
         */
        void switchScene(uint32_t id);

        /**
         * Jump to scene name
         * @param name
         */
        void switchScene(const std::string& name);

        /**
         * Gets currently active scene (returns NULL if no scenes yet added)
         * @return The current scene.
         */
        Scene* getCurrentScene() const { return currentScene; }

        /**
         * Gets currently active scene's index. (return 0 if no scenes yet added)
         * @return The scene's index.
         */
        uint32_t getCurrentSceneIndex() const { return sceneIndex;  }

        /**
         * Get total number of enqueued scenes.
         * @return Number of scenes.
         */
        uint32_t sceneCount() const { return scenes.size(); }

        /**
         * Gets names of enqueued scenes.
         * @return Array of names.
         */
        std::vector<const char*> getSceneNames();

        /**
         * Gets enqueued scenes.
         * @return Array of scenes.
         */
        const std::vector<std::unique_ptr<Scene>>& getScenes() const { return scenes; }

        /**
         * Gets paths of enqueued scenes.
         * @return Array of scenes.
         */
        std::vector<fs::path> getSceneFilePaths() const;

        /**
         *
         */
        void applySceneSwitch();

        /**
         * Enqueue a new scene to the manager.
         */
        void enqueueSceneFromFile(const fs::path& filepath);
        void enqueueScene(std::unique_ptr<Scene>&& scene);
        void enqueueScene(const std::string& name);

    private:
        uint32_t sceneIndex{ 0 };
        uint32_t queuedSceneIndex{ UINT32_MAX };
        Scene* currentScene{ nullptr };
        bool switchingScenes{ false };

        std::vector<std::unique_ptr<Scene>> scenes;
    };
}
