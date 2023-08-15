#pragma once

#include "fusion/scene/scene.h"

namespace fe {
    template<typename T>
    class Module;
    /**
     * @brief Module used for managing game scenes.
     */
    class FUSION_API SceneManager {
        friend class Module<SceneManager>;
    private:
        SceneManager();
        ~SceneManager();

    public:
        static SceneManager* Get() { return Instance; }

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
        void onStart();
        void onUpdate();
        void onStop();


        void onWindowResize(const glm::uvec2& size);

        std::unique_ptr<Scene> scene;
        std::unique_ptr<Scene> sceneCached;

        static SceneManager* Instance;
    };
}
