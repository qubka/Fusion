#include "mesh_importer.hpp"

#include "fusion/graphics/textures/texture2d.hpp"
#include "fusion/assets/asset_manager.hpp"
#include "fusion/filesystem/virtual_file_system.hpp"
#include "fusion/scene/scene_manager.hpp"
#include "fusion/scene/components.hpp"
#include "fusion/scene/systems/hierarchy_system.hpp"

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

using namespace fe;

// For fast converting between ASSIMP and glm
static inline const glm::vec3& vec3_cast(const aiVector3D& v) { return *reinterpret_cast<const glm::vec3*>(&v); }
static inline const glm::vec2& vec2_cast(const aiVector3D& v) { return *reinterpret_cast<const glm::vec2*>(&v); }
static inline const glm::quat& quat_cast(const aiQuaternion& q) { return *reinterpret_cast<const glm::quat*>(&q); }
static inline glm::mat4 mat4_cast(const aiMatrix4x4 &m) { return glm::transpose(glm::make_mat4(&m.a1)); }
static inline glm::mat4 mat4_cast(const aiMatrix3x3 &m) { return glm::transpose(glm::make_mat3(&m.a1)); }

MeshImporter::MeshImporter(const fs::path& filepath, const Vertex::Layout& layout) : layout{layout} {
    path = VirtualFileSystem::Get()->resolvePhysicalPath(filepath);

    Assimp::Importer import;

    int defaultFlags = aiProcess_Triangulate;
    if (layout.contains(Vertex::Component::Normal)) {
        defaultFlags |= aiProcess_GenSmoothNormals;
    }
    if (layout.contains(Vertex::Component::Tangent) || layout.contains(Vertex::Component::Bitangent)) {
        defaultFlags |= aiProcess_CalcTangentSpace;
    }

    //aiProcess_FixInfacingNormals | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_RemoveRedundantMaterials | aiProcess_ImproveCacheLocality
    const aiScene* scene = import.ReadFile(path.string(), defaultFlags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOG_ERROR << "Failed to load model at: \"" << path << "\" - " << import.GetErrorString();
        return;
    }

    directory = path.parent_path();

    aiNode* root = scene->mRootNode;

    auto entity = SceneManager::Get()->getScene()->createEntity<TransformComponent>(path.filename().replace_extension().string());
    hierarchy.emplace(root, entity);

    processNode(scene, root);
}

void MeshImporter::processNode(const aiScene* scene, const aiNode* node) {
    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(scene, node, mesh, i);
    }

    for (uint32_t i = 0; i < scene->mNumCameras; i++) {
        const aiCamera* camera = scene->mCameras[i];
        if (camera->mName == node->mName) {
            processCamera(scene, node, camera);
        }
    }

    for (uint32_t i = 0; i < scene->mNumLights; i++) {
        const aiLight* light = scene->mLights[i];
        if (light->mName == node->mName) {
            processLight(scene, node, light);
        }
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++) {
        processNode(scene, node->mChildren[i]);
    }
}

void MeshImporter::processMesh(const aiScene* scene, const aiNode* node, const aiMesh* mesh, uint32_t index) {
    auto assetManager = AssetManager::Get();
    auto currentScene = SceneManager::Get()->getScene();
    auto& registry = currentScene->getRegistry();

    aiVector3D position; aiQuaternion rotation; aiVector3D scaling;

    auto it = hierarchy.find(node);
    if (it != hierarchy.end() && registry.any_of<MeshComponent>(it->second)) {
        return;
    } else {
        node->mTransformation.Decompose(scaling, rotation, position);
        //node->mTransformation = aiMatrix4x4{};
    }

    std::vector<uint8_t> vertices;
    vertices.reserve(mesh->mNumVertices * layout.getStride());
    std::vector<uint32_t> indices;
    indices.reserve(mesh->mNumFaces * 3);
    std::vector<std::shared_ptr<Texture2d>> textures;

    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        appendVertex(vertices, scene, mesh, i);
    }
    radius = std::max(glm::length(minExtents), glm::length(maxExtents));

    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];
        if (face.mNumIndices != 3)
            continue;
        for (uint32_t j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0) {
        const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        auto diffuseMaps = loadTextures(material, aiTextureType_DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        /*auto specularMaps = loadTextures(material, aiTextureType_SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        auto normalMaps = loadTextures(material, aiTextureType_HEIGHT);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        auto heightMaps = loadTextures(material, aiTextureType_AMBIENT);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());*/

        /*if (textures.empty()) {
            aiColor3D color;
            material->Get(AI_MATKEY_COLOR_DIFFUSE, color);

            auto r = static_cast<uint8_t>(color.r * 255);
            auto g = static_cast<uint8_t>(color.g * 255);
            auto b = static_cast<uint8_t>(color.b * 255);
            textures.push_back(std::make_shared<Image>(r, g, b));
        }*/
    }

    std::string name{ mesh->mName.C_Str() + ":"s + std::to_string(index) };
    Mesh tempMesh{path, name, minExtents, maxExtents, radius, 1, 1, layout};

    std::stringstream ss;
    {
        cereal::BinaryOutputArchive output{ss};
        output(tempMesh);
    }

    auto meshPtr = assetManager->find<Mesh>(ss.str());

    if (it != hierarchy.end()) {
        if (meshPtr) {
            registry.emplace<MeshComponent>(it->second, meshPtr);
        } else {
            meshPtr = std::make_shared<Mesh>(path, name, vertices, indices, layout);
            assetManager->add(meshPtr);
            registry.emplace<MeshComponent>(it->second, meshPtr);
        }
    } else {
        auto entity = currentScene->createEntity(name);

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.setLocalPosition(vec3_cast(position));
        transform.setLocalOrientation(quat_cast(rotation));
        transform.setLocalScale(vec3_cast(scaling));

        if (meshPtr) {
            registry.emplace<MeshComponent>(entity, meshPtr);
        } else {
            meshPtr = std::make_shared<Mesh>(path, name, vertices, indices, layout);
            assetManager->add(meshPtr);
            registry.emplace<MeshComponent>(it->second, meshPtr);
        }

        if (node->mParent) {
            if (auto it2 = hierarchy.find(node->mParent); it2 != hierarchy.end()) {
                currentScene->getSystem<HierarchySystem>()->assignChild(it2->second, entity);
            }
        }

        hierarchy.emplace(node, entity);
    }
}

void MeshImporter::processLight(const aiScene* scene, const aiNode* node, const aiLight* light) {
    auto currentScene = SceneManager::Get()->getScene();
    auto& registry = currentScene->getRegistry();

    // TODO: Can it happend ?
    if (auto it = hierarchy.find(node); it != hierarchy.end()) {
        registry.emplace_or_replace<CameraComponent>(it->second);
    } else {
        aiVector3D scaling; aiQuaternion rotation; aiVector3D position;
        node->mTransformation.Decompose(scaling, rotation, position);
        //node->mTransformation = aiMatrix4x4{};

        auto entity = currentScene->createEntity(node->mName.C_Str());

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.setLocalPosition(vec3_cast(position));
        transform.setLocalOrientation(quat_cast(rotation));
        transform.setLocalScale(vec3_cast(scaling));
        registry.emplace<LightComponent>(entity);

        if (node->mParent) {
            if (auto it2 = hierarchy.find(node->mParent); it2 != hierarchy.end()) {
                currentScene->getSystem<HierarchySystem>()->assignChild(it2->second, entity);
            }
        }

        hierarchy.emplace(node, entity);
    }
}

void MeshImporter::processCamera(const aiScene* scene, const aiNode* node, const aiCamera* camera) {
    auto currentScene = SceneManager::Get()->getScene();
    auto& registry = currentScene->getRegistry();

    // TODO: Can it happend ?
    if (auto it = hierarchy.find(node); it != hierarchy.end()) {
        registry.emplace_or_replace<CameraComponent>(it->second);
    } else {
        aiVector3D scaling; aiQuaternion rotation; aiVector3D position;
        node->mTransformation.Decompose(scaling, rotation, position);
        //node->mTransformation = aiMatrix4x4{};

        auto entity = currentScene->createEntity(node->mName.C_Str());

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.setLocalPosition(vec3_cast(position));
        transform.setLocalOrientation(quat_cast(rotation));
        transform.setLocalScale(vec3_cast(scaling));
        registry.emplace<CameraComponent>(entity);

        if (node->mParent) {
            if (auto it2 = hierarchy.find(node->mParent); it2 != hierarchy.end()) {
                currentScene->getSystem<HierarchySystem>()->assignChild(it2->second, entity);
            }
        }

        hierarchy.emplace(node, entity);
    }
}

std::vector<std::shared_ptr<Texture2d>> MeshImporter::loadTextures(const aiMaterial* material, int type) {
    auto textureType = static_cast<aiTextureType>(type);
    std::vector<std::shared_ptr<Texture2d>> textures;
    for (uint32_t i = 0; i < material->GetTextureCount(textureType); i++) {
        aiString str;
        if (material->GetTexture(textureType, i, &str) == AI_SUCCESS) {
            fs::path path{ directory / str.C_Str() };

            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (const auto& texture : texturesLoaded) {
                if (path == texture->getPath()) {
                    textures.push_back(texture);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }

            // if texture hasn't been loaded already, load it
            if (!skip) {
                if (!fs::exists(path)) {
                    LOG_ERROR << "Could load texture from path: \"" << path << "\"";
                    continue;
                }

                auto texture = std::make_shared<Texture2d>(path.string());
                textures.push_back(texture);
                texture->setType(static_cast<TextureType>(textureType));
                texturesLoaded.push_back(texture); // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        } else {
            LOG_ERROR << "Could not get texture from material";
        }
    }

    return textures;
}

void MeshImporter::appendVertex(std::vector<uint8_t>& outputBuffer, const aiScene* scene, const aiMesh* mesh, uint32_t i) {
    aiVector3D zero{0.0f, 0.0f, 0.0f};
    aiColor3D color{0.0f, 0.0f, 0.0f};
    scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, color);

    const aiVector3D& pos = mesh->mVertices[i];
    const aiVector3D& normal = mesh->mNormals[i];
    const aiVector3D& texCoord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : zero;
    const aiVector3D& tangent = mesh->HasTangentsAndBitangents() ? mesh->mTangents[i] : zero;
    const aiVector3D& biTangent = mesh->HasTangentsAndBitangents() ? mesh->mBitangents[i] : zero;

    // preallocate float buffer with approximate size
    std::vector<float> vertexBuffer;
    vertexBuffer.reserve(layout.getStride() / sizeof(float));

    using Component = Vertex::Component;
    for (const auto& component : layout) {
        switch (component) {
            case Component::Position2:
                vertexBuffer.push_back(pos.x);
                vertexBuffer.push_back(pos.y);
                break;
            case Component::Position:
                vertexBuffer.push_back(pos.x);
                vertexBuffer.push_back(pos.y);
                vertexBuffer.push_back(pos.z);
                break;
            case Component::Normal:
                vertexBuffer.push_back(normal.x);
                vertexBuffer.push_back(normal.y);
                vertexBuffer.push_back(normal.z);
                break;
            case Component::UV:
                vertexBuffer.push_back(texCoord.x);
                vertexBuffer.push_back(texCoord.y);
                break;
            case Component::Color:
                vertexBuffer.push_back(color.r);
                vertexBuffer.push_back(color.g);
                vertexBuffer.push_back(color.b);
                break;
            case Component::Tangent:
                vertexBuffer.push_back(tangent.x);
                vertexBuffer.push_back(tangent.y);
                vertexBuffer.push_back(tangent.z);
                break;
            case Component::Bitangent:
                vertexBuffer.push_back(biTangent.x);
                vertexBuffer.push_back(biTangent.y);
                vertexBuffer.push_back(biTangent.z);
                break;
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
            case Component::RGBA:
                uint32_t result =
                        (static_cast<uint32_t>(255) << 24) |
                        (static_cast<uint32_t>(color.b * 255) << 16) |
                        (static_cast<uint32_t>(color.g * 255) << 8)  |
                        (static_cast<uint32_t>(color.r * 255) << 0);
                vertexBuffer.push_back(static_cast<float>(result));
                break;
        };
    }

    maxExtents = glm::max(vec3_cast(pos), maxExtents);
    minExtents = glm::min(vec3_cast(pos), minExtents);

    appendOutput<float>(outputBuffer, vertexBuffer);
}