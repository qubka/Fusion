#pragma once

#include "fusion/core/module.hpp"
#include "fusion/scene/scene.hpp"

namespace fe {
    /**
     * @brief Module used for managing game scenes.
     */
    class Scenes : public Module::Registrar<Scenes> {
    public:
        Scenes() = default;
        ~Scenes() override = default;

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
        void setScene(std::shared_ptr<Scene> scene) { this->scene = std::move(scene); }

        const std::filesystem::path& getFilename() const { return filename; }

    private:
        std::shared_ptr<Scene> scene;
        std::filesystem::path filename;
    };
}
