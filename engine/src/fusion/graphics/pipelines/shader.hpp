#pragma once

namespace glslang {
    class TProgram;
    class TType;
    class TIntermediate;
}

namespace fe {\
    /**
     * @brief Class that loads and processes a shader, and provides a reflection.
     */
    class Shader {
    public:
        class SpecConstant {
        public:
            SpecConstant() : value{0.0f} {}
            template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
            SpecConstant(const T& value) : value{static_cast<float>(value)} {}

        private:
            float value;
        };

        class Specialization {
            friend class Shader;
        public:
            const VkSpecializationInfo& getSpecializationInfo() const { return info; }

        private:
            VkSpecializationInfo info = { 0 };
            std::vector<VkSpecializationMapEntry> mapEntries;
            std::flat_map<uint32_t, SpecConstant> data;
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

            bool operator<(const Uniform& rhs) const {
                return binding < rhs.binding;
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
            enum class Type : unsigned char { None, Uniform, Storage, Push };

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
            const std::flat_map<std::string, Uniform>& getUniforms() const { return uniforms; }

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

            bool operator<(const UniformBlock& rhs) const {
                return binding < rhs.binding;
            }

        private:
            int32_t binding;
            int32_t size;
            VkShaderStageFlags stageFlags;
            Type type;
            std::flat_map<std::string, Uniform> uniforms{};
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

            bool operator<(const Attribute& rhs) const {
                return location < rhs.location;
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
            explicit Constant(int32_t specId = -1, int32_t size = -1, VkShaderStageFlags stageFlags = 0, int32_t glType = -1)
                    : specId{specId}
                    , size{size}
                    , stageFlags{stageFlags}
                    , glType{glType} {
            }

            int32_t getSpecId() const { return specId; }
            int32_t getSize() const { return size; }
            VkShaderStageFlags getStageFlags() const { return stageFlags; }
            int32_t getGlType() const { return glType; }

            bool operator==(const Constant& rhs) const {
                return specId == rhs.specId && size == rhs.size && stageFlags == rhs.stageFlags && glType == rhs.glType;
            }

            bool operator!=(const Constant& rhs) const {
                return !operator==(rhs);
            }

            bool operator<(const Constant& rhs) const {
                return specId < rhs.specId;
            }

        private:
            int32_t specId;
            int32_t size;
            VkShaderStageFlags stageFlags;
            int32_t glType;
        };

        Shader() = default;
        ~Shader() = default;

        static VkFormat GlTypeToVk(int32_t type);
        static uint32_t GlTypeToSize(int32_t type);

        bool reportedNotFound(const std::string& name, bool reportIfFound) const;
        std::optional<VkDescriptorType> getDescriptorType(uint32_t location) const;
        std::optional<uint32_t> getDescriptorLocation(const std::string& name) const;
        std::optional<uint32_t> getDescriptorSize(const std::string& name) const;
        std::optional<Uniform> getUniform(const std::string& name) const;
        std::optional<UniformBlock> getUniformBlock(const std::string& name) const;
        std::optional<Attribute> getAttribute(const std::string& name) const;
        std::optional<Constant> getConstant(const std::string& name) const;

        std::vector<VkPushConstantRange> getPushConstantRanges() const;
        std::vector<VkSpecializationMapEntry> getSpecializationMapEntries(VkShaderStageFlagBits moduleFlag) const;

        VkShaderModule createShaderModule(const std::string& moduleName, const std::string& moduleCode, VkShaderStageFlags moduleFlag);
        std::optional<Specialization> createSpecialization(const std::flat_map<std::string, SpecConstant>& specConstants, VkShaderStageFlagBits moduleFlag) const;
        void createReflection();

        const std::string& getName() const { return name; }
        const std::flat_map<std::string, Uniform>& getUniforms() const { return uniforms; };
        const std::flat_map<std::string, UniformBlock>& getUniformBlocks() const { return uniformBlocks; };
        const std::flat_map<std::string, Attribute>& getAttributes() const { return attributes; };
        const std::flat_map<std::string, Constant>& getConstants() const { return constants; };
        const std::array<std::optional<uint32_t>, 3>& getLocalSizes() const { return localSizes; }
        const std::vector<VkDescriptorSetLayoutBinding>& getDescriptorSetLayouts() const { return descriptorSetLayouts; }
        const std::vector<VkVertexInputAttributeDescription>& getAttributeDescriptions() const { return attributeDescriptions; }
        uint32_t getLastDescriptorBinding() const { return lastDescriptorBinding; }

    private:
        void loadUniformBlock(const glslang::TProgram& program, VkShaderStageFlags stageFlag, int32_t i);
        void loadUniform(const glslang::TProgram& program, VkShaderStageFlags stageFlag, int32_t i);
        void loadAttribute(const glslang::TProgram& program, VkShaderStageFlags stageFlag, int32_t i);
        void loadConstants(const glslang::TIntermediate& intermediate, VkShaderStageFlags stageFlag);
        static int32_t computeSize(const glslang::TType& type);

        std::string name;
        std::flat_map<std::string, Uniform> uniforms;
        std::flat_map<std::string, UniformBlock> uniformBlocks;
        std::flat_map<std::string, Attribute> attributes;
        std::flat_map<std::string, Constant> constants;

        std::array<std::optional<uint32_t>, 3> localSizes;

        std::flat_map<std::string, uint32_t> descriptorLocations;
        std::flat_map<std::string, uint32_t> descriptorSizes;

        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayouts;
        std::flat_map<uint32_t, VkDescriptorType> descriptorTypes;
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        uint32_t lastDescriptorBinding{ 0 };

        mutable std::vector<std::string> notFoundNames;
    };
}
