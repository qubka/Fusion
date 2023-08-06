#include "model.h"

#include "fusion/core/engine.h"
#include "fusion/assets/asset_registry.h"

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace fe;

// For fast converting between ASSIMP and glm
static inline const glm::vec3& vec3_cast(const aiVector3D& v) { return *reinterpret_cast<const glm::vec3*>(&v); }
static inline const glm::vec3& vec3_cast(const aiColor3D& v) { return *reinterpret_cast<const glm::vec3*>(&v); }
static inline const glm::vec2& vec2_cast(const aiVector3D& v) { return *reinterpret_cast<const glm::vec2*>(&v); }
static inline const glm::quat& quat_cast(const aiQuaternion& q) { return *reinterpret_cast<const glm::quat*>(&q); }
static inline glm::mat4 mat4_cast(const aiMatrix4x4& m) { return glm::transpose(glm::make_mat4(&m.a1)); }
static inline glm::mat4 mat4_cast(const aiMatrix3x3& m) { return glm::transpose(glm::make_mat3(&m.a1)); }

// TODO: May be load from shader ?
Vertex::Layout Model::Layout = {{ Vertex::Component::Position, Vertex::Component::Normal, Vertex::Component::Tangent, Vertex::Component::Bitangent, Vertex::Component::UV }};

Model::Model(fs::path filepath, bool load) : path{std::move(filepath)} {
    if (load)
        loadFromFile();
}

void Model::loadFromFile() {
    if (!meshesLoaded.empty()) {
        FS_LOG_DEBUG("Model: '{}' already was loaded", path);
        return;
    }

    fs::path modelPath{ Engine::Get()->getApp()->getProjectSettings().projectRoot / path }; // get full path

    uint32_t flags = aiProcess_Triangulate;
    if (Layout.contains(Vertex::Component::Normal)) {
        flags |= aiProcess_GenSmoothNormals;
    }
    if (Layout.contains(Vertex::Component::UV)) {
        flags |= aiProcess_GenUVCoords | aiProcess_FlipUVs;
    }
    if (Layout.contains(Vertex::Component::Tangent) || Layout.contains(Vertex::Component::Bitangent)) {
        flags |= aiProcess_CalcTangentSpace;
    }

    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(modelPath.string(), flags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        FS_LOG_ERROR("Failed to load model at: '{}' - {}", modelPath, import.GetErrorString());
        return;
    }

    name = scene->mRootNode->mName.C_Str();
    //directory = modelPath.parent_path();

    aiVector3D position; aiQuaternion orientation; aiVector3D scale;
    scene->mRootNode->mTransformation.Decompose(scale, orientation, position);
    root = { scene->mRootNode->mName.C_Str(), vec3_cast(position), quat_cast(orientation), vec3_cast(scale) };

    processNode(scene, scene->mRootNode, root);
}

void Model::processNode(const aiScene* scene, const aiNode* node, SceneObject& targetParent) {
    SceneObject* parent;

    /// if node has meshes, create a new scene object for it
    if (node->mNumMeshes > 0) {
        aiVector3D position; aiQuaternion orientation; aiVector3D scale;
        node->mTransformation.Decompose(scale, orientation, position);

        auto& newObject = targetParent.children.emplace_back(node->mName.C_Str(), vec3_cast(position), quat_cast(orientation), vec3_cast(scale));
        processMeshes(scene, node, newObject);

        // the new object is the parent for all child nodes
        parent = &newObject;
    } else {
        // if no meshes, skip the node, but keep its transformation
        parent = &targetParent;
    }

    // continue for all child nodes
    for (uint32_t i = 0; i < node->mNumChildren; ++i) {
        processNode(scene, node->mChildren[i], *parent);
    }
}

void Model::processMeshes(const aiScene* scene, const aiNode* node, SceneObject& parent) {
    for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
        std::vector<std::byte> vertices;
        std::vector<uint32_t> indices;
        uint32_t index = node->mMeshes[i];
        {
            const aiMesh* mesh = scene->mMeshes[index];
            vertices.reserve(mesh->mNumVertices * Layout.getStride());
            indices.reserve(mesh->mNumFaces * 3);

            for (uint32_t j = 0; j < mesh->mNumVertices; ++j) {
                appendVertex(vertices, scene, mesh, j);
            }

            for (uint32_t j = 0; j < mesh->mNumFaces; ++j) {
                const aiFace& face = mesh->mFaces[j];
                if (face.mNumIndices != 3)
                    continue;
                for (uint32_t k = 0; k < face.mNumIndices; ++k) {
                    indices.push_back(face.mIndices[k]);
                }
            }

            /*if (mesh->mMaterialIndex >= 0) {
                const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

                auto diffuseMaps = loadTextures(material, aiTextureType_DIFFUSE);
                textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

                auto specularMaps = loadTextures(material, aiTextureType_SPECULAR);
                textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

                auto normalMaps = loadTextures(material, aiTextureType_HEIGHT);
                textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

                auto heightMaps = loadTextures(material, aiTextureType_AMBIENT);
                textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

                if (textures.empty()) {
                    aiColor3D color;
                    material->Get(AI_MATKEY_COLOR_DIFFUSE, color);

                    auto r = static_cast<uint8_t>(color.r * 255);
                    auto g = static_cast<uint8_t>(color.g * 255);
                    auto b = static_cast<uint8_t>(color.b * 255);
                    textures.push_back(std::make_shared<Image>(r, g, b));
                }
            }*/
        }

        auto& mesh = parent.meshes.emplace_back(std::make_unique<Mesh>(index));
        mesh->setVertices(vertices, Layout.getStride());
        mesh->setIndices(indices);
        meshesLoaded.push_back(mesh.get());
    }
}

/*std::vector<std::shared_ptr<Texture2d>> Model::loadTextures(const aiMaterial* material, int type) {
    auto textureType = static_cast<aiTextureType>(type);
    std::vector<std::shared_ptr<Texture2d>> textures;
    for (uint32_t i = 0; i < material->GetTextureCount(textureType); ++i) {
        aiString str;
        if (material->GetTexture(textureType, i, &str) == AI_SUCCESS) {
            fs::path path{ directory / str.C_Str() };

            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            if (auto it = texturesLoaded.find(path); it != texturesLoaded.end()) {
                textures.push_back(it->second);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
            }

            // if texture hasn't been loaded already, load it
            if (!skip) {
                if (!fs::exists(path)) {
                    FS_LOG_ERROR("Could load texture from path: '{}'", path);
                    continue;
                }

                auto texture = std::make_shared<Texture2d>(path);
                textures.push_back(texture);
                //texture->setType(static_cast<TextureType>(textureType));
                texturesLoaded.emplace(path, texture); // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
                //AssetRegistry::Get()->add(texture, path);
            }
        } else {
            FS_LOG_ERROR("Could not get texture from material");
        }
    }

    return textures;
}*/

static aiVector3D zero{0.0f, 0.0f, 0.0f};

const aiVector3D& getTextureCoord(const aiMesh* mesh, uint32_t texture, uint32_t i) {
    for (uint32_t j = texture; j != -1; j--) {
        if (mesh->HasTextureCoords(j)) {
            return mesh->mTextureCoords[j][i];
        }
    }
    return zero;
}

void Model::appendVertex(std::vector<std::byte>& outputBuffer, const aiScene* scene, const aiMesh* mesh, uint32_t i) {
    uint32_t texture = 0;

    // preallocate float buffer with approximate size
    std::vector<float> vertexBuffer;
    vertexBuffer.reserve(Layout.getStride() / sizeof(float));

    using Component = Vertex::Component;
    for (const auto& component : Layout) {
        switch (component) {
            case Component::Position2: {
                const aiVector3D& pos = mesh->mVertices[i];
                vertexBuffer.push_back(pos.x);
                vertexBuffer.push_back(pos.y);
                break;
            }
            case Component::Position: {
                const aiVector3D& pos = mesh->mVertices[i];
                vertexBuffer.push_back(pos.x);
                vertexBuffer.push_back(pos.y);
                vertexBuffer.push_back(pos.z);
                break;
            }
            case Component::Normal: {
                const aiVector3D& normal = mesh->mNormals[i];
                vertexBuffer.push_back(normal.x);
                vertexBuffer.push_back(normal.y);
                vertexBuffer.push_back(normal.z);
                break;
            }
            case Component::Tangent: {
                const aiVector3D& tangent = mesh->HasTangentsAndBitangents() ? mesh->mTangents[i] : zero;
                vertexBuffer.push_back(tangent.x);
                vertexBuffer.push_back(tangent.y);
                vertexBuffer.push_back(tangent.z);
                break;
            }
            case Component::Bitangent: {
                const aiVector3D& biTangent = mesh->HasTangentsAndBitangents() ? mesh->mBitangents[i] : zero;
                vertexBuffer.push_back(biTangent.x);
                vertexBuffer.push_back(biTangent.y);
                vertexBuffer.push_back(biTangent.z);
                break;
            }
            case Component::UV: {
                const aiVector3D& texCoord = getTextureCoord(mesh, texture, i);
                vertexBuffer.push_back(texCoord.x);
                vertexBuffer.push_back(texCoord.y);
                //texture++;
                break;
            }
            case Component::Color: {
                aiColor3D color{0.0f, 0.0f, 0.0f};
                scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, color);
                vertexBuffer.push_back(color.r);
                vertexBuffer.push_back(color.g);
                vertexBuffer.push_back(color.b);
                break;
            }
            case Component::RGBA: {
                aiColor3D color{0.0f, 0.0f, 0.0f};
                scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, color);
                vertexBuffer.push_back(static_cast<float>(glm::rgbaColor(vec3_cast(color))));
                break;
            }

            // Dummy components for padding
            case Component::DummyInt:
            case Component::DummyUint:
            case Component::DummyFloat:
                vertexBuffer.push_back(0.0f);
                break;
            case Component::DummyInt4:
            case Component::DummyUint4:
            case Component::DummyVec4:
                vertexBuffer.push_back(0.0f);
                vertexBuffer.push_back(0.0f);
                vertexBuffer.push_back(0.0f);
                vertexBuffer.push_back(0.0f);
                break;
            case Component::DummyInt2:
            case Component::DummyUint2:
            case Component::DummyVec2:
                vertexBuffer.push_back(0.0f);
                vertexBuffer.push_back(0.0f);
                break;
        };
    }

    Vector::Append(outputBuffer, vertexBuffer);
}
