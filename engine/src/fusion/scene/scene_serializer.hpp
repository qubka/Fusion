#pragma once

namespace fe {
    class Scene;

    class SceneSerializer {
    public:
        SceneSerializer(const std::shared_ptr<Scene>& scene);

        void serialize(const std::string& filename);
        void serializeRuntime(const std::string& filename);

        bool deserialize(const std::string& filename);
        bool deserializeRuntime(const std::string& filename);
    private:
        std::shared_ptr<Scene> scene;
    };
}