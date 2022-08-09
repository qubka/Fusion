#pragma once

#include "fusion/graphics/pipelines/pipeline.hpp"
#include "fusion/graphics/pipelines/vertex.hpp"

namespace fe {
    class TextureDepth;
    class Texture2d;
    class RenderArea;

    /**
     * @brief Class that represents a graphics pipeline.
     */
    class PipelineGraphics : public Pipeline {
    public:
        enum class Mode : unsigned char {
            Polygon,
            MRT
        };

        enum class Depth : unsigned char {
            None,
            Read,
            Write,
            ReadWrite
        };

        enum class Blend : unsigned char {
            None,
            OneZero,
            ZeroSrcColor,
            SrcAlphaOneMinusSrcAlpha,
        };

        /**
         * Creates a new pipeline.
         * @param stage The graphics stage this pipeline will be run on.
         * @param paths The source files to load the pipeline shaders from.
         * @param vertexInputs The vertex inputs that will be used as a shaders input.
         * @param constants A list of specialization constants.
         * @param bindlessSets
         * @param mode The mode this pipeline will run in.
         * @param depth The depth read/write that will be used.
         * @param topology The topology of the input assembly.
         * @param polygonMode The polygon draw mode.
         * @param cullMode The vertex cull mode.
         * @param frontFace The direction to render faces.
         * @param depthBiasEnabled Controls whether to bias fragment depth values.
         * @param depthBiasConstantFactor The scalar factor controlling the constant depth value added to each fragment.
         * @param depthBiasSlopeFactor The scalar factor applied to a fragment’s slope in depth bias calculations.
         * @param lineWidth The width of rasterized line segments.
         * @param transparencyEnabled
         * @param pushDescriptors If no actual descriptor sets are allocated but instead pushed.
         */
        PipelineGraphics(Stage stage,
                         std::vector<fs::path>&& paths,
                         std::vector<Vertex::Input>&& vertexInputs = {},
                         std::flat_map<std::string, SpecConstant>&& constants = {},
                         std::vector<std::string>&& bindlessSets = {},
                         Mode mode = Mode::Polygon,
                         Depth depth = Depth::ReadWrite,
                         VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                         VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
                         VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT,
                         VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE,
                         Blend blend = Blend::SrcAlphaOneMinusSrcAlpha,
                         bool depthBiasEnabled = false,
                         float depthBiasConstantFactor = 0.0f,
                         float depthBiasSlopeFactor = 0.0f,
                         float depthBiasClamp = 0.0f,
                         float lineWidth = 1.0f,
                         bool transparencyEnabled = true,
                         bool pushDescriptors = false);
        ~PipelineGraphics() override;

        /**
         * Gets the depth stencil used in a stage.
         * @param stage The stage to get values from, if not provided the pipelines stage will be used.
         * @return The depth stencil that is found.
         */
        const TextureDepth* getDepthStencil(const std::optional<uint32_t>& stage = std::nullopt) const;

        /**
         * Gets a image used in a stage by the index given to it in the renderpass.
         * @param index The renderpass Image index.
         * @param stage The stage to get values from, if not provided the pipelines stage will be used.
         * @return The image that is found.
         */
        const Texture2d* getTexture(size_t index, const std::optional<uint32_t>& stage = std::nullopt) const;

        /**
         * Gets the render stage viewport.
         * @param stage The stage to get values from, if not provided the pipelines stage will be used.
         * @return The the render stage viewport.
         */
        const RenderArea& getRenderArea(const std::optional<uint32_t>& stage = std::nullopt) const;

        void bindPipeline(const CommandBuffer& commandBuffer) const override;

        const Stage& getStage() const { return stage; }
        const std::vector<fs::path>& getPaths() const { return paths; }
        const std::vector<Vertex::Input>& getVertexInputs() const { return vertexInputs; }
        const std::flat_map<std::string, SpecConstant>& getConstants() const { return constants; }
        Mode getMode() const { return mode; }
        Depth getDepth() const { return depth; }
        VkPrimitiveTopology getTopology() const { return topology; }
        VkPolygonMode getPolygonMode() const { return polygonMode; }
        VkCullModeFlags getCullMode() const { return cullMode; }
        VkFrontFace getFrontFace() const { return frontFace; }
        bool isPushDescriptors() const override { return pushDescriptors; }
        const Shader& getShader() const override { return shader; }
        const VkDescriptorSetLayout& getDescriptorSetLayout() const override { return descriptorSetLayout; }
        const VkPipeline& getPipeline() const override { return pipeline; }
        const VkPipelineLayout& getPipelineLayout() const override { return pipelineLayout; }
        const VkPipelineBindPoint& getPipelineBindPoint() const override { return pipelineBindPoint; }

    private:
        void createShaderProgram();
        void createDescriptorLayout();
        void createPipelineLayout();
        void createAttributes();
        void createPipeline();
        void createPipelinePolygon();
        void createPipelineMrt();

        Stage stage;
        std::vector<fs::path> paths;
        std::vector<Vertex::Input> vertexInputs;
        std::flat_map<std::string, SpecConstant> constants;
        std::vector<std::string> bindlessSets;
        Mode mode;
        Depth depth;
        Blend blend;
        VkPrimitiveTopology topology;
        VkPolygonMode polygonMode;
        VkCullModeFlags cullMode;
        VkFrontFace frontFace;
        bool depthBiasEnabled;
        float depthBiasConstantFactor;
        float depthBiasSlopeFactor;
        float depthBiasClamp;
        float lineWidth;
        bool transparencyEnabled;
        bool pushDescriptors;

        Shader shader;

        std::vector<VkShaderModule> modules;
        std::vector<VkPipelineShaderStageCreateInfo> stages;
        std::vector<VkDynamicState> dynamicStates;

        VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };

        VkPipeline pipeline{ VK_NULL_HANDLE };
        VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
        VkPipelineBindPoint pipelineBindPoint;

        std::array<VkPipelineColorBlendAttachmentState, 1> blendAttachmentStates = {};
        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        VkPipelineDepthStencilStateCreateInfo depthStencilState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        VkPipelineTessellationStateCreateInfo tessellationState = { VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO };
    };
}
