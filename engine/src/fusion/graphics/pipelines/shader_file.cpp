#include "shader_file.hpp"

#include "fusion/filesystem/file_system.hpp"

using namespace fe;

ShaderFile::ShaderFile(const fs::path& filepath) {
    path = filepath;
    name = filepath.filename().string();

    code = FileSystem::ReadText(path);
    if (code.empty())
        throw std::runtime_error("File " + name + " empty");

    std::string extension{ FileSystem::GetExtension(filepath) };
    if (extension == ".comp")
        stage = VK_SHADER_STAGE_COMPUTE_BIT;
    else if (extension == ".vert")
        stage = VK_SHADER_STAGE_VERTEX_BIT;
    else if (extension == ".tesc")
        stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    else if (extension == ".tese")
        stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    else if (extension == ".geom")
        stage = VK_SHADER_STAGE_GEOMETRY_BIT;
    else if (extension == ".frag")
        stage = VK_SHADER_STAGE_FRAGMENT_BIT;
}