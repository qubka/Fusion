#pragma once

namespace fe {
    /*class Renderer;
    class TextRenderer {
        struct PushConstantData {
            alignas(16) glm::mat4 projection{1};
            alignas(16) glm::vec4 color{1};
        };

    public:
        TextRenderer(const vkx::Context& context, Renderer& renderer)
                : context{context}, renderer{renderer} {
            assert(!instance && "Text Renderer already exists!");
            if (instance == nullptr) {
                create();
                instance = this;
            }
        }

        ~TextRenderer() {
            if (instance != nullptr) {
                destroy();
            }
            instance = nullptr;
        };

        void begin();
        void draw(const std::shared_ptr<vkx::Font>& font, glm::vec2 position);
        void end();

        static TextRenderer& Instance() { assert(instance && "Text Renderer was not initialized!"); return *instance; }

        std::shared_ptr<vkx::Font> loadFont(const std::string& filename);

    private:
        void create() {
            createDescriptorSets();
            createPipelineLayout();
            createPipeline();
        }
        void destroy() {
            context.device.destroyPipelineLayout(pipelineLayout);
            context.device.destroyPipeline(pipeline);

            for (auto [path, font] : fonts) {
                font->destroy();
            }
            fonts.clear();
        }

        const vkx::Context& context;
        Renderer& renderer;

        void createDescriptorSets();
        void createPipelineLayout();
        void createPipeline();

        vk::Pipeline pipeline;
        vk::PipelineLayout pipelineLayout;
        vk::CommandBuffer* commandBuffer{ nullptr };

        std::unordered_map<std::string, std::shared_ptr<vkx::Font>> fonts;

        static TextRenderer* instance;
    };*/
}
