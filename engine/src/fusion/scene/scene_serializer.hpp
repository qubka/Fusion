#pragma once

namespace fe {
    class Scene;

    class SceneSerializer {
    public:
        SceneSerializer(const std::shared_ptr<Scene>& scene);

        void serialize(const std::filesystem::path& filename);
        void serializeRuntime(const std::filesystem::path& filename);

        bool deserialize(const std::filesystem::path& filename);
        bool deserializeRuntime(const std::filesystem::path& filename);
    private:
        std::shared_ptr<Scene> scene;
    };
}