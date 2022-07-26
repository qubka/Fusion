#include "model.hpp"

#include "fusion/graphics/textures/texture2d.hpp"
#include "fusion/filesystem/virtual_file_system.hpp"

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

using namespace fe;

Model::Model(const fs::path& filepath, const Vertex::Layout& layout) : layout{layout} {
    fs::path path{ VirtualFileSystem::Get()->resolvePhysicalPath(filepath) };

    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOG_ERROR << "Failed to load model at: \"" << path << "\" - " << import.GetErrorString();
        return;
    }

    directory = path.parent_path();
    processNode(scene, scene->mRootNode);
    radius = std::max(glm::length(minExtents), glm::length(maxExtents));
}

void Model::processNode(const aiScene* scene, const aiNode* node) {
    for (uint32_t i = 0; i< node->mNumMeshes; i++) {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(scene, mesh);
    }

    for (uint32_t i = 0; i< node->mNumChildren; i++) {
        processNode(scene, node->mChildren[i]);
    }
}

void Model::processMesh(const aiScene* scene, const aiMesh* mesh) {
    std::vector<uint8_t> vertices;
    vertices.reserve(mesh->mNumVertices * layout.getStride());
    std::vector<uint32_t> indices;
    indices.reserve(mesh->mNumFaces * 3);
    std::vector<std::shared_ptr<Texture2d>> textures;

    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        appendVertex(vertices, scene, mesh, i);
    }

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

   meshes.push_back(std::make_unique<Mesh>(vertices, indices/*, std::move(textures)*/));
}

std::vector<std::shared_ptr<Texture2d>> Model::loadTextures(const aiMaterial* material, int type) {
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

void Model::appendVertex(std::vector<uint8_t>& outputBuffer, const aiScene* scene, const aiMesh* mesh, uint32_t i) {
    const aiVector3D zero{0.0f, 0.0f, 0.0f};
    aiColor3D color{0.0f, 0.0f, 0.0f};
    scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, color);

    const aiVector3D& pos = mesh->mVertices[i];
    const aiVector3D& normal = mesh->mNormals[i];
    const aiVector3D& texCoord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : zero;
    const aiVector3D& tangent = mesh->HasTangentsAndBitangents() ? mesh->mTangents[i] : zero;
    const aiVector3D& biTangent = mesh->HasTangentsAndBitangents() ? mesh->mBitangents[i] : zero;

    std::vector<float> vertexBuffer;
    glm::vec3 scaledPos{ pos.x, pos.y, pos.z };
    scaledPos *= scale;
    scaledPos += center;

    using Component = Vertex::Component;

    // preallocate float buffer with approximate size
    vertexBuffer.reserve(layout.getStride() / sizeof(float));
    for (const auto& component : layout) {
        switch (component) {
            case Component::Position2:
                vertexBuffer.push_back(scaledPos.x);
                vertexBuffer.push_back(scaledPos.y);
                break;
            case Component::Position:
                vertexBuffer.push_back(scaledPos.x);
                vertexBuffer.push_back(scaledPos.y);
                vertexBuffer.push_back(scaledPos.z);
                break;
            case Component::Normal:
                vertexBuffer.push_back(normal.x);
                vertexBuffer.push_back(-normal.y);
                vertexBuffer.push_back(normal.z);
                break;
            case Component::UV:
                vertexBuffer.push_back(texCoord.x * uvScale.s);
                vertexBuffer.push_back(texCoord.y * uvScale.t);
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

    appendOutput<float>(outputBuffer, vertexBuffer);

    maxExtents = glm::max(scaledPos, maxExtents);
    minExtents = glm::min(scaledPos, minExtents);
}

void Model::cmdRender(const CommandBuffer& commandBuffer) const {
    for (auto& mesh : meshes) {
        mesh->cmdRender(commandBuffer);
    }
}

/*aiScene Model::GenerateScene(const Mesh& mesh) {
    aiScene scene;
    scene.mRootNode = new aiNode{};

    scene.mMaterials = new aiMaterial*[1];
    scene.mNumMaterials = 1;
    scene.mMaterials[0] = new aiMaterial{};

    scene.mMeshes = new aiMesh*[1];
    scene.mNumMeshes = 1;
    scene.mMeshes[0] = new aiMesh{};
    scene.mMeshes[0]->mMaterialIndex = 0;

    scene.mRootNode->mMeshes = new unsigned int[1];
    scene.mRootNode->mNumMeshes = 1;
    scene.mRootNode->mMeshes[0] = 0;

    auto pMesh = scene.mMeshes[0];

    const auto& vertices = mesh->verticies;
    size_t numVertices = vertices.size();

    pMesh->mVertices = new aiVector3D[numVertices];
    pMesh->mNumVertices = numVertices;

    pMesh->mNormals = new aiVector3D[numVertices];

    pMesh->mTextureCoords[0] = new aiVector3D[numVertices];
    pMesh->mNumUVComponents[0] = numVertices;

    for (size_t i = 0; i < numVertices; i++) {
        const auto& v = vertices[i];
        pMesh->mVertices[i] = {v.position.x, v.position.y, v.position.z};
        pMesh->mNormals[i] = {v.normal.x, v.normal.y, v.normal.z};
        pMesh->mTextureCoords[0][i] = {v.texture.x, v.texture.y, 0};
    }

    const auto& indices = mesh->indices;
    size_t numFaces = indices.size() / 3;

    pMesh->mFaces = new aiFace[numFaces];
    pMesh->mNumFaces = numFaces;

    for (size_t i = 0, j = 0; i < numFaces; i++, j += 3) {
        aiFace& face = pMesh->mFaces[i];
        face.mIndices = new unsigned int[3];
        face.mNumIndices = 3;
        face.mIndices[0] = indices[j+0];
        face.mIndices[1] = indices[j+1];
        face.mIndices[2] = indices[j+2];
    }

    return scene;
}*/
