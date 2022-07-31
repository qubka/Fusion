#pragma once

#include "fusion/graphics/subrender.hpp"
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
            return it != subrenders.end();
        }

        /**
         * Gets a subrender.
         * @tparam T The subrender type.
         * @return The subrender.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, Subrender*>>>
       T* get() const {
            /*if (auto it = subrenders.find(type_id<T>); it != subrenders.end() && it->second) {
                std::vector<T*> result;
                result.reserve(it->second.size());
                for (auto& system : it->second) {
                    result.push_back(static_cast<T*>(system.get()));
                }
                return result;
            }*/
            if (auto it = subrenders.find(type_id<T>); it != subrenders.end()) {
                return static_cast<T*>(it->second.front().get());
            }
            throw std::runtime_error("Subrender Holder does not have requested subrender");
        }

        /**
         * Adds a subrender.
         * @tparam T The subrender type.
         * @param pipelineStage The subrender pipeline stage.
         * @param subrender The subrender.
         * @return The added renderer.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, Subrender*>>>
        T* add(Pipeline::Stage pipelineStage, std::unique_ptr<T>&& subrender) {
            const auto& type = type_id<T>;
            auto& storage = subrenders[type];

            // Insert the stage value
            stages.insert({ pipelineStage, { type, static_cast<uint32_t>(storage.size()) } });

            // Then, add the subrender
            auto& it = storage.emplace_back(std::move(subrender));
            return static_cast<T*>(it.get());
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
         * Represents position in the subrenders structure, first value being the subrenderer type and second for subrenderer id in the array.
         */
        using SubrenderIndex = std::pair<type_index, uint32_t>;

        /**
         * Iterates through all subrenders for updating stages.
         */
        void updateAll();

        /**
         * Iterates through all subrenders for rendering.
         * @param pipelineStage The subrender stage.
         * @param commandBuffer The command buffer to record render command into.
         * @param overrideCamera The optional camera for rendering.
         */
        void renderStage(Pipeline::Stage pipelineStage, const CommandBuffer& commandBuffer, const Camera* overrideCamera = nullptr);

        /// List of all subrenders
        std::unordered_map<type_index, std::vector<std::unique_ptr<Subrender>>> subrenders;
        /// List of subrender stages
        std::multimap<Pipeline::Stage, SubrenderIndex> stages;
    };
}
