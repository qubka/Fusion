#include "model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

using namespace fe;

void Model::processNode(const aiScene* scene, const aiNode* node) {
    /*for (uint32_t i = 0; i< node->mNumMeshes; i++) {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(scene, mesh);
    }

    for (size_t i = 0; i< node->mNumChildren; i++) {
        processNode(scene, node->mChildren[i]);
    }*/
}

void Model::processMesh(const aiScene* scene, const aiMesh* mesh) {

}
