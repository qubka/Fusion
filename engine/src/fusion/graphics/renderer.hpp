#pragma once

#include "fusion/graphics/subrender_holder.hpp"
#include "fusion/graphics/render_stage.hpp"

namespace fe {
    /**
     * @brief Class used to manage {@link Subrender} objects to create a list of render pass.
     */
    class Renderer {
        friend class Graphics;
    public:
        /**
         * Creates a new renderer, fill {@link renderStages} in your subclass of this.
         */
        Renderer() = default;
        virtual ~Renderer() = default;
        NONCOPYABLE(Renderer);

        /**
         * Checks whether a subrender exists or not.
         * @tparam T The subrender type.
         * @return If the subrender has the System.
         */
         template<typename T>
         bool hasSubrender() const  {
             return subrenderHolder.has<T>();
         }

         /**
          * Gets a subrender.
          * @tparam T The subrender type.
          * @return The subrender.
          */
        template<typename T>
        T* getSubrender() const {
            return subrenderHolder.get<T>();
        }

        /**
         * Adds a subrender.
         * @tparam T The subrender type.
         * @tparam Args The constructor arg types.
         * @param pipelineStage The subrender pipeline stage.
         * @param args The constructor arguments.
         */
        template<typename T, typename... Args>
        T* addSubrender(Pipeline::Stage pipelineStage, Args&&...args) {
            return subrenderHolder.add<T>(pipelineStage, std::make_unique<T>(pipelineStage, std::forward<Args>(args)...));
        }

        /**
         * Removes a subrender.
         * @tparam T The subrender type.
         */
        template<typename T>
        void removeSubrender() {
            subrenderHolder.remove<T>();
        }

        /**
         * Clears all subrenders.
         */
        void clearSubrenders() {
            subrenderHolder.clear();
        }

        RenderStage* getRenderStage(size_t index) const {
            if (renderStages.empty() || renderStages.size() < index)
                return nullptr;
            return renderStages[index].get();
        }
        void addRenderStage(std::unique_ptr<RenderStage>&& renderStage) {
            renderStages.push_back(std::move(renderStage));
        }

    protected:
        /**
         * Run when switching to this scene from another, use this method to create {@link Subrender}'s.
         */
        virtual void onStart() = 0;

        /**
         * Run when updating the renderer module.
         */
        virtual void onUpdate() = 0;

    private:
        std::vector<std::unique_ptr<RenderStage>> renderStages;
        SubrenderHolder subrenderHolder;
        bool started{ false };
    };
}
