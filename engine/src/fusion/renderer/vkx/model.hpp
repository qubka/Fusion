/*
* Vulkan Model loader using ASSIMP
*
* Copyright(C) 2016-2017 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license(MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

#include "buffer.hpp"
#include "context.hpp"

struct aiScene;
namespace Assimp {
class Importer;
};

namespace vkx { namespace model {

/** @brief Vertex layout components */
enum class Component : uint8_t {
    Position = 0x0,
    Normal = 0x1,
    Color = 0x2,
    UV = 0x3,
    Tangent = 0x4,
    Bitangent = 0x5,
    DummyFloat = 0x6,
    DummyInt = 0x7,
    DummyVec4 = 0x8,
    DummyInt4 = 0x9,
    DummyUint4 = 0xa,
};

/** @brief Stores vertex layout components for model loading and Vulkan vertex input and atribute bindings  */
struct VertexLayout {
    /** @brief Components used to generate vertices from */
    std::vector<Component> components;
    uint32_t binding;

    VertexLayout() = default;
    VertexLayout(std::vector<Component>&& components, uint32_t binding = 0)
        : components{std::move(components)}, binding{binding} {}

    uint32_t componentIndex(Component component) const {
        for (uint32_t i = 0; i < components.size(); ++i) {
            if (components[i] == component) {
                return i;
            }
        }
        return UINT32_MAX;
    }

    static vk::Format componentFormat(Component component) {
        switch (component) {
            case Component::UV:
                return vk::Format::eR32G32Sfloat;
            case Component::DummyFloat:
                return vk::Format::eR32Sfloat;
            case Component::DummyInt:
                return vk::Format::eR32Sint;
            case Component::DummyVec4:
                return vk::Format::eR32G32B32A32Sfloat;
            case Component::DummyInt4:
                return vk::Format::eR32G32B32A32Sint;
            case Component::DummyUint4:
                return vk::Format::eR32G32B32A32Uint;
            default:
                return vk::Format::eR32G32B32Sfloat;
        }
    }

    static uint32_t componentSize(Component component) {
        switch (component) {
            case Component::UV:
                return 2 * sizeof(float);
            case Component::DummyFloat:
                return sizeof(float);
            case Component::DummyInt:
                return sizeof(int);
            case Component::DummyVec4:
                return 4 * sizeof(float);
            case Component::DummyInt4:
                return 4 * sizeof(int32_t);
            case Component::DummyUint4:
                return 4 * sizeof(uint32_t);
            default:
                // All components except the ones listed above are made up of 3 floats
                return 3 * sizeof(float);
        }
    }

    uint32_t stride() const {
        uint32_t res = 0;
        for (auto& component : components) {
            res += componentSize(component);
        }
        return res;
    }

    uint32_t offset(uint32_t index) const {
        uint32_t res = 0;
        assert(index < components.size());
        for (uint32_t i = 0; i < index; ++i) {
            res += componentSize(components[i]);
        }
        return res;
    }
};

/** @brief Used to parametrize model loading */
struct ModelCreateInfo {
    glm::vec3 center{ 0.0f };
    glm::vec3 scale{ 1.0f };
    glm::vec2 uvscale{ 1.0f };

    ModelCreateInfo() = default;

    ModelCreateInfo(const glm::vec3& scale, const glm::vec2& uvscale, const glm::vec3& center)
        : center{center}
        , scale{scale}
        , uvscale{uvscale} {}

    ModelCreateInfo(float scale, float uvscale, float center)
        : ModelCreateInfo(glm::vec3{scale}, glm::vec2{ uvscale }, glm::vec3{center}) {}
};

struct Model {
    vk::Device device;
    Buffer vertices;
    Buffer indices;
    VertexLayout layout;
    uint32_t indexCount{ 0 };
    uint32_t vertexCount{ 0 };
    glm::vec3 scale{ 1.0f };
    glm::vec3 center{ 0.0f };
    glm::vec2 uvscale{ 1.0f };

    /** @brief Stores vertex and index base and counts for each part of a model */
    struct ModelPart {
        std::string name;
        uint32_t vertexBase;
        uint32_t vertexCount;
        uint32_t indexBase;
        uint32_t indexCount;
    };
    std::vector<ModelPart> parts;

    static const int defaultFlags;

    struct Dimension {
        glm::vec3 min{ FLT_MAX };
        glm::vec3 max{ -FLT_MAX };
        glm::vec3 size;
    } dim;

    /** @brief Release all Vulkan resources of this model */
    void destroy() {
        vertices.destroy();
        indices.destroy();
    }

    /**
    * Loads a 3D model from a file into Vulkan buffers
    *
    * @param device Pointer to the Vulkan device used to generated the vertex and index buffers on
    * @param filename File to load (must be a model format supported by ASSIMP)
    * @param layout Vertex layout components (position, normals, tangents, etc.)
    * @param createInfo MeshCreateInfo structure for load time settings like scale, center, etc.
    * @param copyQueue Queue used for the memory staging copy commands (must support transfer)
    * @param (Optional) flags ASSIMP model loading flags
    */
    void loadFromFile(const Context& context,
                      const std::string& filename,
                      const VertexLayout& layout,
                      const ModelCreateInfo& createInfo,
                      int flags = defaultFlags);

    /**
    * Loads a 3D model from a file into Vulkan buffers
    *
    * @param device Pointer to the Vulkan device used to generated the vertex and index buffers on
    * @param filename File to load (must be a model format supported by ASSIMP)
    * @param layout Vertex layout components (position, normals, tangents, etc.)
    * @param scale Load time scene scale
    * @param copyQueue Queue used for the memory staging copy commands (must support transfer)
    * @param (Optional) flags ASSIMP model loading flags
    */
    void loadFromFile(const Context& context, const std::string& filename, const VertexLayout& layout = {{ Component::Position, Component::Normal, Component::UV, Component::Color}}, float scale = 1.0f, const int flags = defaultFlags) {
        loadFromFile(context, filename, layout, ModelCreateInfo{ scale, 1.0f, 0.0f }, flags);
    }

    virtual void onLoad(const Context& context, Assimp::Importer& importer, const aiScene* pScene) {}

    virtual void appendVertex(std::vector<uint8_t>& outputBuffer, const aiScene* pScene, uint32_t meshIndex, uint32_t vertexIndex);

    template <typename T>
    void appendOutput(std::vector<uint8_t>& outputBuffer, const T& t) {
        auto offset = outputBuffer.size();
        auto copySize = sizeof(T);
        outputBuffer.resize(offset + copySize);
        memcpy(outputBuffer.data() + offset, &t, copySize);
    }

    template <typename T>
    void appendOutput(std::vector<uint8_t>& outputBuffer, const std::vector<T>& v) {
        auto offset = outputBuffer.size();
        auto copySize = v.size() * sizeof(T);
        outputBuffer.resize(offset + copySize);
        memcpy(outputBuffer.data() + offset, v.data(), copySize);
    }
};

}}  // namespace vkx::model
