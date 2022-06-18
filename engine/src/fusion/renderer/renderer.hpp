#pragma once

#include "subrender.hpp"

namespace fe {
    class Renderer {
        friend class Subrender;
    public:
        /**
         * Creates a new renderer.
         */
        Renderer(const vkx::Context& context) : context{context} { }
        virtual ~Renderer() = default;

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
            if (auto it{ subrenders.find(typeid(T)) }; it != subrenders.end()) {
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

    private:
        const vkx::Context& context;
        std::unordered_map<std::type_index, std::unique_ptr<Subrender>> subrenders;
    };
}
