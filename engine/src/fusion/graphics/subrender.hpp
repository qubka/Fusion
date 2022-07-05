#pragma once

#include "fusion/graphics/pipelines/pipeline.hpp"

namespace fe {
    /**
     * @brief Represents a render pipeline that is used to render a type of pipeline.
     */
    class Subrender {
    public:
        /**
         * Creates a new render pipeline.
         * @param stage The stage this renderer will be used in.
         */
        explicit Subrender(Pipeline::Stage stage) : stage{std::move(stage)} { }
        virtual ~Subrender() = default;
        NONCOPYABLE(Subrender);

        /**
         * Called after main renderer update.
         */
        virtual void onUpdate() = 0;

        /**
         * Runs the render pipeline in the current renderpass.
         * @param commandBuffer The command buffer to record render command into.
         */
        virtual void onRender(const CommandBuffer& commandBuffer) = 0;

        const Pipeline::Stage& getStage() const { return stage; }

        bool isEnabled() const { return enabled; }
        void setEnabled(bool enable) { this->enabled = enable; }

    private:
        bool enabled{ true };
        Pipeline::Stage stage;
    };

    //template class TypeInfo<Subrender>;
}
