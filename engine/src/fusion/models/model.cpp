#include "model.hpp"

#include "fusion/graphics/textures/texture2d.hpp"

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

using namespace fe;

void Model::processNode(const aiScene* scene, const aiNode* node) {
    for (size_t i = 0; i< node->mNumMeshes; i++) {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(scene, mesh);
    }

    for (size_t i = 0; i< node->mNumChildren; i++) {
        processNode(scene, node->mChildren[i]);
    }
}

void Model::processMesh(const aiScene* scene, const aiMesh* mesh) {
    std::vector<uint8_t> vertices;
    std::vector<uint32_t> indices;
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

    //meshes.push_back(std::make_unique<Mesh>(std::move(vertices), std::move(indices), std::move(textures)));
}

std::vector<std::shared_ptr<Texture2d>> Model::loadTextures(const aiMaterial* material, int type) {
    auto textureType = static_cast<aiTextureType>(type);
    std::vector<std::shared_ptr<Texture2d>> textures;
    for (size_t i = 0; i < material->GetTextureCount(textureType); i++) {
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
                    std::cerr << "ERROR: Could load texture from path: " << path << std::endl;
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
    glm::vec3 scaledPos{ pos.x, -pos.y, pos.z };
    scaledPos *= scale;
    scaledPos += center;

    using Component = Vertex::Component;

    // preallocate float buffer with approximate size
    vertexBuffer.reserve(layout.getSize() * 4);
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
                        (static_cast<uint8_t>(UINT8_MAX) << 24) +
                        (static_cast<uint8_t>(color.b * UINT8_MAX) << 16) +
                        (static_cast<uint8_t>(color.g * UINT8_MAX) << 8)  +
                         static_cast<uint8_t>(color.r * UINT8_MAX);
                vertexBuffer.push_back(static_cast<float>(result));
                break;
        };
    }

    appendOutput(outputBuffer, vertexBuffer);

    maxExtents = glm::max(scaledPos, maxExtents);
    minExtents = glm::min(scaledPos, minExtents);
}