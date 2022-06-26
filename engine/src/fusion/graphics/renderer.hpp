#pragma once

#include "render_stage.hpp"
#include "subrender_holder.hpp"

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

        /**
         * Run when switching to this scene from another, use this method to create {@link Subrender}'s.
         */
        virtual void start() = 0;

        /**
         * Run when updating the renderer manager.
         */
        virtual void update() = 0;

        /**
         * Checks whether a Subrender exists or not.
         * @tparam T The Subrender type.
         * @return If the Subrender has the System.
         */
         template<typename T>
         bool hasSubrender() const  {
             return subrenderHolder.has<T>();
         }

         /**
          * Gets a Subrender.
          * @tparam T The Subrender type.
          * @return The Subrender.
          */
        template<typename T>
        T* getSubrender() const {
            return subrenderHolder.get<T>();
        }

        /**
         * Adds a Subrender.
         * @tparam T The Subrender type.
         * @tparam Args The constructor arg types.
         * @param pipelineStage The Subrender pipeline stage.
         * @param args The constructor arguments.
         */
        template<typename T, typename... Args>
        T* addSubrender(const Pipeline::Stage& pipelineStage, Args&&...args) {
            return subrenderHolder.add<T>(pipelineStage, std::make_unique<T>(pipelineStage, std::forward<Args>(args)...));
        }

        /**
         * Removes a Subrender.
         * @tparam T The Subrender type.
         */
        template<typename T>
        void removeSubrender() {
            subrenderHolder.remove<T>();
        }

        /**
         * Clears all Subrenders.
         */
        void clearSubrenders() {
            subrenderHolder.clear();
        }

        RenderStage* getRenderStage(uint32_t index) const {
            if (renderStages.empty() || renderStages.size() < index)
                return nullptr;
            return renderStages[index].get();
        }

        void addRenderStage(std::unique_ptr<RenderStage>&& renderStage) {
            renderStages.push_back(std::move(renderStage));
        }

    private:
        std::vector<std::unique_ptr<RenderStage>> renderStages;
        SubrenderHolder subrenderHolder;
        bool started{ false };
    };
}
