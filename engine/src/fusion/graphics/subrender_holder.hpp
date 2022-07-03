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
         * Checks whether a Subrender exists or not.
         * @tparam T The Subrender type.
         * @return If the Subrender exists.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, Subrender*>>>
        bool has() const {
            const auto it = subrenders.find(typeid(T));
            return it != subrenders.end() && it->second;
        }

        /**
         * Gets a Subrender.
         * @tparam T The Subrender type.
         * @return The Subrender.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, Subrender*>>>
        T* get() const {
            const auto& typeId = typeid(T);

            if (auto it = subrenders.find(typeId); it != subrenders.end() && it->second)
                return static_cast<T*>(it->second.get());

            //throw std::runtime_error("Subrender Holder does not have requested Subrender");
            return nullptr;
        }

        /**
         * Adds a Subrender.
         * @tparam T The Subrender type.
         * @param stage The Subrender pipeline stage.
         * @param subrender The Subrender.
         * @return The added renderer.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, Subrender*>>>
        T* add(const Pipeline::Stage& stage, std::unique_ptr<T>&& subrender) {
            // Remove previous Subrender, if it exists
            remove<T>();

            const auto& typeId = typeid(T);

            // Insert the stage value
            stages.insert({ StageIndex(stage, subrenders.size()), typeId });

            // Then, add the Subrender
            subrenders[typeId] = std::move(subrender);
            return static_cast<T*>(subrenders[typeId].get());
        }

        /**
         * Removes a Subrender.
         * @tparam T The Subrender type.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, Subrender*>>>
        void remove() {
            const auto& typeId = typeid(T);

            // Remove the stage value for this Subrender
            removeSubrenderStage(typeId);

            // Then, remove the Subrender
            subrenders.erase(typeId);
        }

        /**
         * Clears all Subrenders.
         */
        void clear();

    private:
        using StageIndex = std::pair<Pipeline::Stage, size_t>;

        void removeSubrenderStage(const std::type_index& id);

        /**
         * Iterates through all Subrenders.
         * @param stage The Subrender stage.
         * @param commandBuffer The command buffer to record render command into.
         */
        void renderStage(const Pipeline::Stage& stage, const CommandBuffer& commandBuffer);

        /// List of all Subrenders.
        std::unordered_map<std::type_index, std::unique_ptr<Subrender>> subrenders;
        /// List of subrender stages.
        std::multimap<StageIndex, std::type_index> stages;
    };
}
