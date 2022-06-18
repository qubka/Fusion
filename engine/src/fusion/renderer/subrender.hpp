#pragma once

namespace vk {
    class CommandBuffer;
}

namespace fe {
    class Renderer;
    class Subrender {
    public:
        /**
         * Creates a new render pipeline.
         */
        Subrender(Renderer& renderer) : renderer{renderer} {};
        virtual ~Subrender() = default;

        /**
         * Runs the render pipeline in the current renderpass.
         * @param commandBuffer The command buffer to record render command into.
         */
        virtual void onRender(const vk::CommandBuffer& commandBuffer) = 0;

        bool isEnabled() const { return enabled; }
        void setEnabled(bool flag) { enabled = flag; }

    private:
        Renderer& renderer;
        bool enabled{ true };
    };
}
