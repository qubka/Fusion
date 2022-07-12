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
         * Gets the current scene.
         * @return The current scene.
         */
        Scene* getScene() const { return scene.get(); }

        /**
         * Sets the current scene to a new scene.
         * @param scene The new scene.
         */
        void setScene(std::unique_ptr<Scene>&& scene) { this->scene = std::move(scene); }

    private:
        std::unique_ptr<Scene> scene;
    };
}
