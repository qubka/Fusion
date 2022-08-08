#pragma once

#include "fusion/scene/scene.hpp"

namespace fe {
    /**
     * @brief Module used for managing game scenes.
     */
    class SceneManager : public Module::Registrar<SceneManager> {
    public:
        SceneManager() = default;
        ~SceneManager() override;

        /**
         * Gets the current scene.
         * @return The current scene.
         */
        Scene* getScene() const { return scene.get(); }

        /**
         * Sets the current scene to a new scene.
         * @param scene The new scene.
         */
        void setScene(std::unique_ptr<Scene>&& scene) { this->scene = std::move(scene); }

        bool cacheScene();
        bool loadCachedScene();

    private:
        void onStart() override;
        void onUpdate() override;
        void onWindowResize(const glm::uvec2& size);

        std::unique_ptr<Scene> scene;
        std::unique_ptr<Scene> sceneCached;
    };
}
