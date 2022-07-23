#pragma once

#include <volk/volk.h>

namespace fe {
    class Vertex {
    public:
        /**
         * @brief Vertex layout components.
         */
        enum class Component : uint8_t {
            /** Model + Layout */
            Position,
            Normal,
            Color,
            UV,
            Tangent,
            Bitangent,
            /** Layout only */
            Position2,
            RGBA,
            DummyFloat,
            DummyInt,
            DummyUint,
            DummyVec2,
            DummyInt2,
            DummyUint2,
            DummyVec4,
            DummyInt4,
            DummyUint4,
        };

        /**
         * @brief Stores vertex layout components for model loading and Vulkan vertex input and atribute bindings for shader.
         */
        class Layout {
        public:
            Layout(std::vector<Component>&& components, uint32_t binding = 0) : components{std::move(components)}, binding{binding} {}
            ~Layout() = default;

            Component operator[](uint32_t index) const { return components[index]; }
            ITERATABLE(Component, components);

            uint32_t getSize() const { return components.size(); }
            uint32_t getBinding() const { return binding; }

            uint32_t getStride() const {
                uint32_t res = 0;
                for (const auto& component : components) {
                    res += ComponentSize(component);
                }
                return res;
            }

            uint32_t getOffset(uint32_t index) const {
                uint32_t res = 0;
                assert(index < components.size());
                for (uint32_t i = 0; i < index; ++i) {
                    res += ComponentSize(components[i]);
                }
                return res;
            }

            VkFormat getFormat(uint32_t index) const {
                return ComponentFormat(components[index]);
            }

        private:
            uint32_t binding{ 0 };
            /** @brief Components used to generate vertices from */
            std::vector<Component> components;
        };

        /**
         * @brief Class used to define sets of vertex inputs used in a shader.
         */
        class Input {
        public:
            Input() = default;
            Input(std::vector<Component>&& components) {
                append({std::move(components)});
            }
            explicit Input(const std::vector<Layout>& vertexLayouts) {
                for (const auto& layout : vertexLayouts) {
                    append(layout);
                }
            }
            explicit Input(std::vector<VkVertexInputBindingDescription> bindingDescriptions = {}, std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {})
                    : bindingDescriptions{std::move(bindingDescriptions)}
                    , attributeDescriptions{std::move(attributeDescriptions)} {
            }

            const std::vector<VkVertexInputBindingDescription>& getBindingDescriptions() const { return bindingDescriptions; }
            const std::vector<VkVertexInputAttributeDescription>& getAttributeDescriptions() const { return attributeDescriptions; }

            void append(const Layout& vertexLayout, VkVertexInputRate rate = VK_VERTEX_INPUT_RATE_VERTEX) {
                VkVertexInputBindingDescription bindingDescription = {};
                bindingDescription.binding = vertexLayout.getBinding();
                bindingDescription.stride = vertexLayout.getStride();
                bindingDescription.inputRate = rate;
                bindingDescriptions.push_back(bindingDescription);

                auto componentsSize = vertexLayout.getSize();
                attributeDescriptions.reserve(attributeDescriptions.size() + componentsSize);
                auto attributeIndexOffset = static_cast<uint32_t>(attributeDescriptions.size());

                for (uint32_t i = 0; i < componentsSize; ++i) {
                    VkVertexInputAttributeDescription attributeDescription = {};
                    attributeDescription.location = attributeIndexOffset + i;
                    attributeDescription.binding = vertexLayout.getBinding();
                    attributeDescription.format = vertexLayout.getFormat(i);
                    attributeDescription.offset = vertexLayout.getOffset(i);
                    attributeDescriptions.push_back(attributeDescription);
                }
            }

            bool operator<(const Input& rhs) const {
                return bindingDescriptions.front().binding < rhs.bindingDescriptions.front().binding;
            }

        private:
            std::vector<VkVertexInputBindingDescription> bindingDescriptions;
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        };

        static VkFormat ComponentFormat(Component component);
        static uint32_t ComponentSize(Component component);
    };
}