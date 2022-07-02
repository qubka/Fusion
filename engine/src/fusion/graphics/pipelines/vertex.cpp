#include "vertex.hpp"

using namespace fe;

VkFormat Vertex::ComponentFormat(Component component) {
    switch (component) {
        case Component::UV:
        case Component::DummyVec2:
        case Component::Position2:
            return VK_FORMAT_R32G32_SFLOAT;
        case Component::RGBA:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case Component::DummyFloat:
            return VK_FORMAT_R32_SFLOAT;
        case Component::DummyInt:
            return VK_FORMAT_R32_SINT;
        case Component::DummyUint:
            return VK_FORMAT_R32_UINT;
        case Component::DummyInt2:
            return VK_FORMAT_R32G32_SINT;
        case Component::DummyUint2:
            return VK_FORMAT_R32G32_UINT;
        case Component::DummyVec4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case Component::DummyInt4:
            return VK_FORMAT_R32G32B32A32_SINT;
        case Component::DummyUint4:
            return VK_FORMAT_R32G32B32A32_UINT;
        default:
            return VK_FORMAT_R32G32B32_SFLOAT;
    }
}

uint32_t Vertex::ComponentSize(Component component) {
    switch (component) {
        case Component::UV:
        case Component::DummyVec2:
        case Component::Position2:
            return sizeof(glm::vec2);
        case Component::RGBA:
            return sizeof(uint32_t);
        case Component::DummyFloat:
            return sizeof(float);
        case Component::DummyInt:
            return sizeof(int32_t);
        case Component::DummyUint:
            return sizeof(uint32_t);
        case Component::DummyInt2:
            return sizeof(glm::ivec2);
        case Component::DummyUint2:
            return sizeof(glm::uvec2);
        case Component::DummyVec4:
            return sizeof(glm::vec4);
        case Component::DummyInt4:
            return sizeof(glm::ivec4);
        case Component::DummyUint4:
            return sizeof(glm::uvec4);
        default:
            // All components except the ones listed above are made up of 3 floats
            return sizeof(glm::vec3);
    }
}
