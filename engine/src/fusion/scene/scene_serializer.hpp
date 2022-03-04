#pragma once

namespace fe {
    class Scene;

    class SceneSerializer {
    public:
        SceneSerializer(const std::shared_ptr<Scene>& scene);

        void serialize(const std::string& filepath);
        void serializeRuntime(const std::string& filepath);

        bool deserialize(const std::string& filepath);
        bool deserializeRuntime(const std::string& filepath);
    private:
        std::shared_ptr<Scene> scene;
    };
}