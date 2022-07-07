#pragma once

#include "pipeline.hpp"
#include "vertex.hpp"

namespace fe {
    class ImageDepth;
    class Image2d;
    class RenderArea;

    /**
     * @brief Class that represents a graphics pipeline.
     */
    class PipelineGraphics : public Pipeline {
    public:
        enum class Mode {
            Polygon, MRT
        };

        enum class Depth {
            None = 0,
            Read = 1,
            Write = 2,
            ReadWrite = Read | Write
        };

        /**
         * Creates a new pipeline.
         * @param stage The graphics stage this pipeline will be run on.
         * @param shaderStages The source files to load the pipeline shaders from.
         * @param vertexInputs The vertex inputs that will be used as a shaders input.
         * @param defines A list of defines added to the top of each shader.
         * @param mode The mode this pipeline will run in.
         * @param depth The depth read/write that will be used.
         * @param topology The topology of the input assembly.
         * @param polygonMode The polygon draw mode.
         * @param cullMode The vertex cull mode.
         * @param frontFace The direction to render faces.
         * @param pushDescriptors If no actual descriptor sets are allocated but instead pushed.
         */
        PipelineGraphics(Stage stage, std::vector<std::filesystem::path> shaderStages, std::vector<Vertex::Input> vertexInputs, std::vector<Shader::Define> defines = {},
            Mode mode = Mode::Polygon, Depth depth = Depth::ReadWrite, VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
            VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT, VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE, bool pushDescriptors = false);
        ~PipelineGraphics();

        /**
         * Gets the depth stencil used in a stage.
         * @param stage The stage to get values from, if not provided the pipelines stage will be used.
         * @return The depth stencil that is found.
         */
        const ImageDepth* getDepthStencil(const std::optional<uint32_t>& stage = std::nullopt) const;

        /**
         * Gets a image used in a stage by the index given to it in the renderpass.
         * @param index The renderpass Image index.
         * @param stage The stage to get values from, if not provided the pipelines stage will be used.
         * @return The image that is found.
         */
        const Image2d* getImage(size_t index, const std::optional<uint32_t>& stage = std::nullopt) const;

        /**
         * Gets the render stage viewport.
         * @param stage The stage to get values from, if not provided the pipelines stage will be used.
         * @return The the render stage viewport.
         */
        const RenderArea& getRenderArea(const std::optional<uint32_t>& stage = std::nullopt) const;

        const Stage& getStage() const { return stage; }
        const std::vector<std::filesystem::path>& getShaderStages() const { return shaderStages; }
        const std::vector<Vertex::Input>& getVertexInputs() const { return vertexInputs; }
        const std::vector<Shader::Define>& getDefines() const { return defines; }
        Mode getMode() const { return mode; }
        Depth getDepth() const { return depth; }
        VkPrimitiveTopology getTopology() const { return topology; }
        VkPolygonMode getPolygonMode() const { return polygonMode; }
        VkCullModeFlags getCullMode() const { return cullMode; }
        VkFrontFace getFrontFace() const { return frontFace; }
        bool isPushDescriptors() const override { return pushDescriptors; }
        const Shader& getShader() const override { return shader; }
        const VkDescriptorSetLayout& getDescriptorSetLayout() const override { return descriptorSetLayout; }
        const VkDescriptorPool& getDescriptorPool() const override { return descriptorPool; }
        const VkPipeline& getPipeline() const override { return pipeline; }
        const VkPipelineLayout& getPipelineLayout() const override { return pipelineLayout; }
        const VkPipelineBindPoint& getPipelineBindPoint() const override { return pipelineBindPoint; }

    private:
        void createShaderProgram();
        void createDescriptorLayout();
        void createDescriptorPool();
        void createPipelineLayout();
        void createAttributes();
        void createPipeline();
        void createPipelinePolygon();
        void createPipelineMrt();

        Stage stage;
        std::vector<std::filesystem::path> shaderStages;
        std::vector<Vertex::Input> vertexInputs;
        std::vector<Shader::Define> defines;
        Mode mode;
        Depth depth;
        VkPrimitiveTopology topology;
        VkPolygonMode polygonMode;
        VkCullModeFlags cullMode;
        VkFrontFace frontFace;
        bool pushDescriptors;

        Shader shader;

        std::vector<VkDynamicState> dynamicStates;

        std::vector<VkShaderModule> modules;
        std::vector<VkPipelineShaderStageCreateInfo> stages;

        VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
        VkDescriptorPool descriptorPool{ VK_NULL_HANDLE };

        VkPipeline pipeline{ VK_NULL_HANDLE };
        VkPipelineLayout pipelineLayout{ VK_NULL_HANDLE };
        VkPipelineBindPoint pipelineBindPoint;

        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
        VkPipelineRasterizationStateCreateInfo rasterizationState = {};
        std::array<VkPipelineColorBlendAttachmentState, 1> blendAttachmentStates = {};
        VkPipelineColorBlendStateCreateInfo colorBlendState = {};
        VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
        VkPipelineViewportStateCreateInfo viewportState = {};
        VkPipelineMultisampleStateCreateInfo multisampleState = {};
        VkPipelineDynamicStateCreateInfo dynamicState = {};
        VkPipelineTessellationStateCreateInfo tessellationState = {};
    };
}
