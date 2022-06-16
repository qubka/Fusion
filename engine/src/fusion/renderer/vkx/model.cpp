/*
* Vulkan Model loader using ASSIMP
*
* Copyright(C) 2016-2017 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license(MIT) (http://opensource.org/licenses/MIT)
*/

#include "model.hpp"
#include "filesystem.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/material.h>

using namespace vkx;
using namespace std::string_literals;

const int Model::defaultFlags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;
const VertexLayout Model::defaultLayout = {{
    Component::Position,
    Component::Normal,
    Component::Color,
    Component::UV,
    Component::UV,
    Component::Bitangent,
    Component::Tangent
}};

void Model::loadFromFile(const Context& context, const std::string& filename, const VertexLayout& layout, const ModelCreateInfo& createInfo, const int flags) {
    this->layout = layout;
    scale = createInfo.scale;
    uvscale = createInfo.uvscale;
    center = createInfo.center;
    destroy();
    device = context.device;

    Assimp::Importer importer;
    const aiScene* pScene;

    // Load file
    vkx::file::withBinaryFileContents(filename, [&](const char* filename, size_t size, const void* data) {
        pScene = importer.ReadFileFromMemory(data, size, flags, filename);
    });

    if (!pScene) {
        throw std::runtime_error(importer.GetErrorString() + "\n\nThe file may be part of the additional asset pack.\n\nRun \"download_assets.py\" in the repository root to download the latest version."s);
    }

    parts.clear();
    parts.resize(pScene->mNumMeshes);
    for (uint32_t i = 0; i < pScene->mNumMeshes; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        parts[i] = {};
        parts[i].name = paiMesh->mName.C_Str();
        parts[i].vertexBase = vertexCount;
        parts[i].vertexCount = paiMesh->mNumVertices;
        vertexCount += paiMesh->mNumVertices;
    }

    onLoad(context, importer, pScene);

    std::vector<uint8_t> vertexBuffer;
    std::vector<uint32_t> indexBuffer;

    vertexCount = 0;
    indexCount = 0;

    // Load meshes
    for (uint32_t meshIndex = 0; meshIndex < pScene->mNumMeshes; meshIndex++) {
        auto& part = parts[meshIndex];
        const aiMesh* paiMesh = pScene->mMeshes[meshIndex];
        const uint32_t numVertices = pScene->mMeshes[meshIndex]->mNumVertices;
        for (uint32_t vertexIndex = 0; vertexIndex < numVertices; vertexIndex++) {
            appendVertex(vertexBuffer, pScene, meshIndex, vertexIndex);
        }

        dim.size = dim.max - dim.min;

        vertexCount += numVertices;
        part.indexBase = static_cast<uint32_t>(indexBuffer.size());
        for (uint32_t j = 0; j < paiMesh->mNumFaces; j++) {
            const aiFace& face = paiMesh->mFaces[j];
            if (face.mNumIndices != 3)
                continue;
            indexBuffer.push_back(part.indexBase + face.mIndices[0]);
            indexBuffer.push_back(part.indexBase + face.mIndices[1]);
            indexBuffer.push_back(part.indexBase + face.mIndices[2]);
            part.indexCount += 3;
        }
        indexCount += part.indexCount;
    }

    // Vertex buffer
    vertices = context.stageToDeviceBuffer<uint8_t>(vk::BufferUsageFlagBits::eVertexBuffer, vertexBuffer);
    // Index buffer
    indices = context.stageToDeviceBuffer<uint32_t>(vk::BufferUsageFlagBits::eIndexBuffer, indexBuffer);
};

void Model::appendVertex(std::vector<uint8_t>& outputBuffer, const aiScene* pScene, uint32_t meshIndex, uint32_t vertexIndex) {
    aiVector3D Zero3D{0.0f, 0.0f, 0.0f};
    const aiMesh* paiMesh = pScene->mMeshes[meshIndex];
    aiColor3D color{0.0f, 0.0f, 0.0f};
    pScene->mMaterials[paiMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    const aiVector3D& pos = paiMesh->HasPositions() ? paiMesh->mVertices[vertexIndex] : Zero3D;
    const aiVector3D& normal = paiMesh->HasNormals() ? paiMesh->mNormals[vertexIndex] : Zero3D;
    const aiVector3D& tangent = paiMesh->HasTangentsAndBitangents() ? paiMesh->mTangents[vertexIndex] : Zero3D;
    const aiVector3D& biTangent = paiMesh->HasTangentsAndBitangents() ? paiMesh->mBitangents[vertexIndex] : Zero3D;
    std::array<const aiVector3D*, AI_MAX_NUMBER_OF_TEXTURECOORDS> texCoords{};
    for (int i = 0; i < texCoords.size(); i++) {
        texCoords[i] = (paiMesh->HasTextureCoords(i) ? &paiMesh->mTextureCoords[i][vertexIndex] : &Zero3D);
        // Make first uv set as default for rest
        if (i == 0)
            Zero3D = *texCoords[i];
    }

    glm::vec3 scaledPos{ pos.x, -pos.y, pos.z };
    scaledPos *= scale;
    scaledPos += center;

    // preallocate float buffer with approximate size
    std::vector<float> vertexBuffer;
    vertexBuffer.reserve(layout.components.size() * 4);
    int uv = 0;
    for (auto& component : layout.components) {
        switch (component) {
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
                vertexBuffer.push_back(texCoords[uv]->x * uvscale.s);
                vertexBuffer.push_back(texCoords[uv]->y * uvscale.t);
                uv++;
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
        };
    }
    appendOutput(outputBuffer, vertexBuffer);

    dim.max = glm::max(scaledPos, dim.max);
    dim.min = glm::min(scaledPos, dim.min);
}
