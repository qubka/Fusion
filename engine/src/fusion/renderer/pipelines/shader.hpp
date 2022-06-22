#pragma once

#include <volk.h>

namespace glslang {
    class TProgram;
    class TType;
}

namespace fe {
    /**
     * @brief Class that loads and processes a shader, and provides a reflection.
     */
    class Shader {
    public:
        class Uniform;
        class Attribute;
        class UniformBlock;
        class Constant;

        /*struct VertexInput {
            uint32_t binding{ 0 };
            std::vector<VkVertexInputBindingDescription> bindingDescriptions;
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        };

        struct Uniform {
            int32_t binding{ -1 };
            int32_t offset{ -1 };
            int32_t size{ -1 };
            int32_t glType{ -1 };
            bool readOnly{ false };
            bool writeOnly{ false };
            VkShaderStageFlags stageFlags{ 0 };
        };

        struct UniformBlock {
            enum class Type { None, Uniform, Storage, Push };
            int32_t binding{ -1 };
            int32_t size{ -1 };
            VkShaderStageFlags stageFlags{ 0 };
            Type type{ Type::Uniform };
            std::map<std::string, Uniform> uniforms;
        };

        struct Attribute {
            int32_t set{ -1 };
            int32_t location{ -1 };
            int32_t size{ -1 };
            int32_t glType{ -1 };
        };

        struct Constant {
            int32_t binding{ -1 };
            int32_t size{ -1 };
            VkShaderStageFlags stageFlags{ 0 };
            int32_t glType{ -1 };
        };*/

        /**
         * A define added to the start of a shader, first value is the define name and second is the value to be set.
         */
        using Define = std::pair<std::string, std::string>;

        Shader() = default;
        ~Shader() = default;

        bool reportedNotFound(const std::string& name, bool reportIfFound) const;
        static VkFormat GlTypeToVk(int32_t type);
        std::optional<uint32_t> getDescriptorLocation(const std::string& name) const;
        std::optional<uint32_t> getDescriptorSize(const std::string& name) const;
        std::optional<Uniform> getUniform(const std::string& name) const;
        std::optional<UniformBlock> getUniformBlock(const std::string& name) const;
        std::optional<Attribute> getAttribute(const std::string& name) const;
        std::vector<VkPushConstantRange> getPushConstantRanges() const;

        std::optional<VkDescriptorType> getDescriptorType(uint32_t location) const;
        static VkShaderStageFlagBits GetShaderStage(const std::filesystem::path& filename);
        VkShaderModule createShaderModule(const std::filesystem::path& moduleName, const std::string& moduleCode, const std::string& preamble, VkShaderStageFlags moduleFlag);
        void createReflection();

        const std::filesystem::path& getName() const { return stages.back(); }
        uint32_t GetLastDescriptorBinding() const { return lastDescriptorBinding; }
        const std::map<std::string, Uniform>& getUniforms() const { return uniforms; };
        const std::map<std::string, UniformBlock>& getUniformBlocks() const { return uniformBlocks; };
        const std::map<std::string, Attribute>& getAttributes() const { return attributes; };
        const std::map<std::string, Constant>& getConstants() const { return constants; };
        const std::array<std::optional<uint32_t>, 3>& getLocalSizes() const { return localSizes; }
        const std::vector<VkDescriptorSetLayoutBinding>& getDescriptorSetLayouts() const { return descriptorSetLayouts; }
        const std::vector<VkDescriptorPoolSize>& getDescriptorPools() const { return descriptorPools; }
        const std::vector<VkVertexInputAttributeDescription>& getAttributeDescriptions() const { return attributeDescriptions; }

    private:
        static void incrementDescriptorPool(std::map<VkDescriptorType, uint32_t>& descriptorPoolCounts, VkDescriptorType type);
        void loadUniformBlock(const glslang::TProgram& program, VkShaderStageFlags stageFlag, int32_t i);
        void loadUniform(const glslang::TProgram& program, VkShaderStageFlags stageFlag, int32_t i);
        void loadAttribute(const glslang::TProgram& program, VkShaderStageFlags stageFlag, int32_t i);
        static int32_t computeSize(const glslang::TType* ttype);

        std::vector<std::filesystem::path> stages;
        std::map<std::string, Uniform> uniforms;
        std::map<std::string, UniformBlock> uniformBlocks;
        std::map<std::string, Attribute> attributes;
        std::map<std::string, Constant> constants;

        std::array<std::optional<uint32_t>, 3> localSizes;

        std::map<std::string, uint32_t> descriptorLocations;
        std::map<std::string, uint32_t> descriptorSizes;

        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayouts;
        uint32_t lastDescriptorBinding{ 0 };
        std::vector<VkDescriptorPoolSize> descriptorPools;
        std::map<uint32_t, VkDescriptorType> descriptorTypes;
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

    public:
        /**
         * @brief Class used to define sets of vertex inputs used in a shader.
         */
        class VertexInput {
        public:
            VertexInput(std::vector<VkVertexInputBindingDescription> bindingDescriptions = {}, std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {})
                : binding{0}
                , bindingDescriptions{std::move(bindingDescriptions)}
                , attributeDescriptions{std::move(attributeDescriptions)} {
            }

            const std::vector<VkVertexInputBindingDescription>& getBindingDescriptions() const { return bindingDescriptions; }
            const std::vector<VkVertexInputAttributeDescription>& getAttributeDescriptions() const { return attributeDescriptions; }

            bool operator<(const VertexInput& rhs) const {
                return bindingDescriptions.front().binding < rhs.bindingDescriptions.front().binding;
            }

        private:
            uint32_t binding;
            std::vector<VkVertexInputBindingDescription> bindingDescriptions;
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        };

        class Uniform {
            friend class Shader;
        public:
            explicit Uniform(int32_t binding = -1, int32_t offset = -1, int32_t size = -1, int32_t glType = -1, bool readOnly = false, bool writeOnly = false, VkShaderStageFlags stageFlags = 0)
                : binding{binding}
                , offset{offset}
                , size{size}
                , glType{glType}
                , readOnly{readOnly}
                , writeOnly{writeOnly}
                , stageFlags{stageFlags} {
            }

            int32_t getBinding() const { return binding; }
            int32_t getOffset() const { return offset; }
            int32_t getSize() const { return size; }
            int32_t getGlType() const { return glType; }
            bool isReadOnly() const { return readOnly; }
            bool isWriteOnly() const { return writeOnly; }
            VkShaderStageFlags getStageFlags() const { return stageFlags; }

            bool operator==(const Uniform& rhs) const {
                return binding == rhs.binding && offset == rhs.offset && size == rhs.size && glType == rhs.glType && readOnly == rhs.readOnly &&
                       writeOnly == rhs.writeOnly && stageFlags == rhs.stageFlags;
            }

            bool operator!=(const Uniform& rhs) const {
                return !operator==(rhs);
            }

        private:
            int32_t binding;
            int32_t offset;
            int32_t size;
            int32_t glType;
            bool readOnly;
            bool writeOnly;
            VkShaderStageFlags stageFlags;
        };

        class UniformBlock {
            friend class Shader;
        public:
            enum class Type { None, Uniform, Storage, Push };

            explicit UniformBlock(int32_t binding = -1, int32_t size = -1, VkShaderStageFlags stageFlags = 0, Type type = Type::Uniform)
                : binding{binding}
                , size{size}
                , stageFlags{stageFlags}
                , type{type} {
            }

            int32_t getBinding() const { return binding; }
            int32_t getSize() const { return size; }
            VkShaderStageFlags getStageFlags() const { return stageFlags; }
            Type getType() const { return type; }
            const std::map<std::string, Uniform>& getUniforms() const { return uniforms; }

            std::optional<Uniform> getUniform(const std::string& name) const {
                if (auto it = uniforms.find(name); it != uniforms.end())
                    return it->second;
                return std::nullopt;
            }

            bool operator==(const UniformBlock& rhs) const {
                return binding == rhs.binding && size == rhs.size && stageFlags == rhs.stageFlags && type == rhs.type && uniforms == rhs.uniforms;
            }

            bool operator!=(const UniformBlock& rhs) const {
                return !operator==(rhs);
            }

        private:
            int32_t binding;
            int32_t size;
            VkShaderStageFlags stageFlags;
            Type type;
            std::map<std::string, Uniform> uniforms;
        };

        class Attribute {
            friend class Shader;
        public:
            explicit Attribute(int32_t set = -1, int32_t location = -1, int32_t size = -1, int32_t glType = -1)
                : set{set}
                , location{location}
                , size{size}
                , glType{glType} {
            }

            int32_t getSet() const { return set; }
            int32_t getLocation() const { return location; }
            int32_t getSize() const { return size; }
            int32_t getGlType() const { return glType; }

            bool operator==(const Attribute& rhs) const {
                return set == rhs.set && location == rhs.location && size == rhs.size && glType == rhs.glType;
            }

            bool operator!=(const Attribute& rhs) const {
                return !operator==(rhs);
            }

        private:
            int32_t set;
            int32_t location;
            int32_t size;
            int32_t glType;
        };

        class Constant {
            friend class Shader;
        public:
            explicit Constant(int32_t binding = -1, int32_t size = -1, VkShaderStageFlags stageFlags = 0, int32_t glType = -1)
                : binding{binding}
                , size{size}
                , stageFlags{stageFlags}
                , glType{glType} {
            }

            int32_t getBinding() const { return binding; }
            int32_t getSize() const { return size; }
            VkShaderStageFlags getStageFlags() const { return stageFlags; }
            int32_t getGlType() const { return glType; }

            bool operator==(const Constant& rhs) const {
                return binding == rhs.binding && size == rhs.size && stageFlags == rhs.stageFlags && glType == rhs.glType;
            }

            bool operator!=(const Constant& rhs) const {
                return !operator==(rhs);
            }

        private:
            int32_t binding;
            int32_t size;
            VkShaderStageFlags stageFlags;
            int32_t glType;
        };
    };
}
