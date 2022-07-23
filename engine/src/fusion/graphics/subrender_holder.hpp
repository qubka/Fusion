#pragma once

#include "subrender.hpp"

#include "fusion/graphics/pipelines/pipeline.hpp"

namespace fe {
    /**
     * @brief Class that contains and manages subrenders registered to a render manager.
     */
    class SubrenderHolder {
        friend class Graphics;
    public:
        SubrenderHolder() = default;
        ~SubrenderHolder() = default;
        NONCOPYABLE(SubrenderHolder);

        /**
         * Checks whether a subrender exists or not.
         * @tparam T The subrender type.
         * @return If the subrender exists.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, Subrender*>>>
        bool has() const {
            auto it = subrenders.find(type_id<T>);
            return it != subrenders.end() && it->second;
        }

        /**
         * Gets a subrender.
         * @tparam T The subrender type.
         * @return The subrender.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, Subrender*>>>
        T* get() const {
            if (auto it = subrenders.find(type_id<T>); it != subrenders.end() && it->second) {
                return reinterpret_cast<T*>(it->second.get());
            }
            throw std::runtime_error("Subrender Holder does not have requested subrender");
        }

        /**
         * Adds a subrender.
         * @tparam T The subrender type.
         * @param stage The subrender pipeline stage.
         * @param subrender The subrender.
         * @return The added renderer.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, Subrender*>>>
        T* add(const Pipeline::Stage& stage, std::unique_ptr<T>&& subrender) {
            // Remove previous subrender, if it exists
            //remove<T>();

            const auto& type = type_id<T>;

            // Insert the stage value
            stages.insert({ stage, type });

            if (auto it = subrenders.find(type); it != subrenders.end() && it->second) {
                // Then, get the existed subrender
                return reinterpret_cast<T*>(it->second.get());
            } else {
                // Then, add the subrender
                subrenders[type] = std::move(subrender);
                return reinterpret_cast<T*>(subrenders[type].get());
            }
        }

        /**
         * Removes a subrender.
         * @tparam T The subrender type.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, Subrender*>>>
        void remove() {
            const auto& type = type_id<T>;

            // Remove the stage value for this subrender
            for (auto it = stages.begin(); it != stages.end();) {
                if (it->second == type) {
                    it = stages.erase(it);
                } else {
                    ++it;
                }
            }

            // Then, remove the subrender
            subrenders.erase(type);
        }

        /**
         * Clears all subrenders.
         */
        void clear();

    private:
        /**
         * Iterates through all subrenders for updating stages.
         */
        void updateAll();

        /**
         * Iterates through all subrenders for rendering.
         * @param stage The subrender stage.
         * @param commandBuffer The command buffer to record render command into.
         */
        void renderStage(const Pipeline::Stage& stage, const CommandBuffer& commandBuffer);

        /// List of all subrenders
        std::unordered_map<type_index, std::unique_ptr<Subrender>> subrenders;
        /// List of subrender stages
        std::multimap<Pipeline::Stage, type_index> stages;
    };
}
