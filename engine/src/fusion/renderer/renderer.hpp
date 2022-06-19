#pragma once

#include "subrender.hpp"
#include "renderstage.hpp"

namespace fe {
    class Renderer {
        friend class Graphics;
    public:
        /**
         * Creates a new renderer.
         */
        Renderer() = default;
        virtual ~Renderer() = default;
        FE_NONCOPYABLE(Renderer);

        /**
         * Run when switching to this scene from another, use this method to create {@link Subrender}'s.
         */
        virtual void onStart() = 0;

        /**
         * Run when updating the renderer manager.
         */
        virtual void onUpdate() = 0;

        /**
         * Checks whether a Subrender exists or not.
         * @tparam T The Subrender type.
         * @return If the Subrender has the System.
         */
        template<typename T>
        bool hasSubrender() const  {
            return subrenders.find(typeid(T)) != subrenders.end();
        }

        /**
         * Gets a Subrender.
         * @tparam T The Subrender type.
         * @return The Subrender.
         */
        template<typename T>
        T* getSubrender() const {
            if (auto it = subrenders.find(typeid(T)); it != subrenders.end()) {
                return it->second.get();
            }
            return nullptr;
        }

        /**
         * Adds a Subrender.
         * @tparam T The Subrender type.
         * @tparam Args The constructor arg types.
         * @param args The constructor arguments.
         */
        template<typename T, typename... Args>
        T* addSubrender(Args &&...args) {
            auto [it, result] = subrenders.emplace(typeid(T), std::make_unique<T>(std::forward<Args>(args)...));
            return it->second.get();
        }

        /**
         * Removes a Subrender.
         * @tparam T The Subrender type.
         */
        template<typename T>
        void removeSubrender() {
            subrenders.erase(std::remove(subrenders.begin(), subrenders.end(), typeid(T)), subrenders.end());
        }

        /**
         * Clears all Subrenders.
         */
        void clearSubrenders() {
            subrenders.clear();
        }

        RenderStage* getRenderStage(uint32_t index) const {
            if (renderStages.empty() || renderStages.size() < index)
                return nullptr;
            return renderStages.at(index).get();
        }

        void addRenderStage(std::unique_ptr<RenderStage>&& renderStage) {
            renderStages.emplace_back(std::move(renderStage));
        }

    private:
        //const vkx::Context& context;
        std::vector<std::unique_ptr<RenderStage>> renderStages;
        std::unordered_map<std::type_index, std::unique_ptr<Subrender>> subrenders;
        //SubrenderHolder subrenderHolder;
        bool started{ false };
    };
}
