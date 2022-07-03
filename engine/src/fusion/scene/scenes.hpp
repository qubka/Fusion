#pragma once

#include "fusion/core/module.hpp"

namespace fe {
    class Scene;
    /**
     * @brief Module used for managing game scenes.
     */
    class Scenes : public Module::Registrar<Scenes> {
    public:
        Scenes();
        ~Scenes();

        void update() override;

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
