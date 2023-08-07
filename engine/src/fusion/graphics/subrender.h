#pragma once

#include "fusion/graphics/pipelines/pipeline.h"

namespace fe {
    class Camera;
    /**
     * @brief Represents a render pipeline that is used to render a type of pipeline.
     */
    class FUSION_API Subrender {
        friend class Renderer;
        friend class SubrenderHolder;
    public:
        /**
         * Creates a new render pipeline.
         * @param stage The stage this renderer will be used in.
         */
        explicit Subrender(Pipeline::Stage stage) : stage{stage} { }
        virtual ~Subrender() = default;
        NONCOPYABLE(Subrender);

        Pipeline::Stage getStage() const { return stage; }

        bool isEnabled() const { return enabled; }
        void setEnabled(bool flag) {
            if (enabled == flag)
                return;
            enabled = flag;
            if (enabled)
                onEnabled();
            else
                onDisabled();
        }

    protected:
        /**
         * Called after main renderer update.
         */
        virtual void onUpdate() = 0;

        /**
         * Runs the render pipeline in the current renderpass.
         * @param commandBuffer The command buffer to record render command into.
         * @param overrideCamera The optional camera for rendering.
         */
        virtual void onRender(const CommandBuffer& commandBuffer, const Camera* overrideCamera) = 0;

        /**
         * @brief Called when the renderer is enabled.
         */
        virtual void onEnabled() {};

        /**
         * @brief Called when the renderer is disabled.
         */
        virtual void onDisabled() {};

    protected:
        bool enabled{ true };
        Pipeline::Stage stage;
    };
}
