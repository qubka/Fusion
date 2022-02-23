#pragma once

namespace fe {
    class Layer {
    public:
        Layer(std::string name = "Layer") : name{std::move(name)} {}
        virtual ~Layer() = default;

        virtual void onAttach() {}
        virtual void onDetach() {}
        virtual void onPrepare() {}
        virtual void onUpdate() {}
        virtual void onRender() {}
        virtual void onImGui() {}

        virtual void onLoadAssets() {}
        // Called when the window has been resized
        // Can be overriden in derived class to recreate or rebuild resources attached to the frame buffer / swapchain
        virtual void onWindowResized() {}
        // Called when view change occurs
        // Can be overriden in derived class to e.g. update uniform buffers
        // Containing view dependant matrices
        virtual void onViewChanged() {}

        virtual void onUpdateCommandBufferPreDraw(const vk::CommandBuffer& cmdBuffer) {}
        virtual void onUpdateDrawCommandBuffer(const vk::CommandBuffer& cmdBuffer) {}
        virtual void onUpdateCommandBufferPostDraw(const vk::CommandBuffer& cmdBuffer) {}

        const std::string& getName() const { return name; }
    protected:
        std::string name;
    };
}
