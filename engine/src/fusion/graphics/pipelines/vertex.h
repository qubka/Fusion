#pragma once

namespace fe {
    class FUSION_API Vertex {
    public:
        /**
         * @brief Vertex layout components.
         */
        enum class Component : unsigned char {
            /** Model + Layout */
            Position,
            Normal,
            Color,
            UV,
            Tangent,
            Bitangent,
            Position2,
            RGBA,

            /** Layout only */
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
        class FUSION_API Layout {
        public:
            Layout(const std::initializer_list<Component>& components) : components{components} {
                for (const auto& component : this->components) {
                    stride += ComponentSize(component);
                }
            }
            explicit Layout(std::vector<Component>&& components, uint32_t binding = 0) : components{std::move(components)}, binding{binding} {
                for (const auto& component : this->components) {
                    stride += ComponentSize(component);
                }
            }
            ~Layout() = default;

            Component operator[](uint32_t index) const { return components[index]; }
            ITERATABLE(Component, components);

            bool contains(Component component) const {
                return std::find(components.begin(), components.end(), component) != components.end();
            }

            uint32_t getSize() const { return components.size(); }
            uint32_t getBinding() const { return binding; }
            uint32_t getStride() const { return stride; }

            uint32_t getOffset(uint32_t index) const {
                FS_ASSERT(index < components.size());
                uint32_t res = 0;
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
            uint32_t stride{ 0 };
            /** @brief Components used to generate vertices from */
            std::vector<Component> components;
        };

        /**
         * @brief Class used to define sets of vertex inputs used in a shader.
         */
        class FUSION_API Input {
        public:
            Input() = default;
            Input(const std::initializer_list<Layout>& vertexLayouts) {
                for (const auto& layout : vertexLayouts) {
                    append(layout);
                }
            }
            explicit Input(std::vector<VkVertexInputBindingDescription>&& bindingDescriptions = {}, std::vector<VkVertexInputAttributeDescription>&& attributeDescriptions = {})
                    : bindingDescriptions{std::move(bindingDescriptions)}
                    , attributeDescriptions{std::move(attributeDescriptions)} {
            }

            const std::vector<VkVertexInputBindingDescription>& getBindingDescriptions() const { return bindingDescriptions; }
            const std::vector<VkVertexInputAttributeDescription>& getAttributeDescriptions() const { return attributeDescriptions; }

            void append(const Layout& vertexLayout, VkVertexInputRate rate = VK_VERTEX_INPUT_RATE_VERTEX) {
                auto& bindingDescription = bindingDescriptions.emplace_back();
                bindingDescription.binding = vertexLayout.getBinding();
                bindingDescription.stride = vertexLayout.getStride();
                bindingDescription.inputRate = rate;

                auto componentsSize = vertexLayout.getSize();
                attributeDescriptions.reserve(attributeDescriptions.size() + componentsSize);

                auto attributeIndexOffset = static_cast<uint32_t>(attributeDescriptions.size());

                for (uint32_t i = 0; i < componentsSize; ++i) {
                    auto& attributeDescription = attributeDescriptions.emplace_back();
                    attributeDescription.location = attributeIndexOffset + i;
                    attributeDescription.binding = vertexLayout.getBinding();
                    attributeDescription.format = vertexLayout.getFormat(i);
                    attributeDescription.offset = vertexLayout.getOffset(i);
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