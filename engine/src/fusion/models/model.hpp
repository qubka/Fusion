#pragma once

namespace fe {
    class aiScene;
    class aiNode;
    class aiMesh;
    class aiMaterial;
    class Mesh;
    class Texture;

    class Model {
    public:
        Model();
        ~Model();
    private:
        std::filesystem::path directory;
        std::vector<std::unique_ptr<Mesh>> meshes;
        std::vector<std::shared_ptr<Texture>> texturesLoaded;

        void processNode(const aiScene* scene, const aiNode* node);
        void processMesh(const aiScene* scene, const aiMesh* mesh);
        //std::vector<std::shared_ptr<Texture>> loadTextures(const aiMaterial* material, aiTextureType type);

        static aiScene GenerateScene(const std::unique_ptr<Mesh>& mesh);
    };
}
