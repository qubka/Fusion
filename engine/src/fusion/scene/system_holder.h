#pragma once

#include "fusion/scene/system.h"

namespace fe {
    /**
     * @brief Class that contains and manages systems registered to a scene.
     */
    class FUSION_API SystemHolder {
        friend class Scene;
    public:
        SystemHolder() = default;
        ~SystemHolder() = default;
        NONCOPYABLE(SystemHolder);

        /**
         * Checks whether a system exists or not.
         * @tparam T The system type.
         * @return If the system exists.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, System*>>>
        bool has() const {
            auto it = systems.find(type_id<T>);
            return it != systems.end();
        }

        /**
         * Gets a system.
         * @tparam T The system type.
         * @return The system.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, System*>>>
        T* get() const {
            if (auto it = systems.find(type_id<T>); it != systems.end()) {
                return static_cast<T*>(it->second.get());
            }
            throw std::runtime_error("System Holder does not have requested system");
        }

        /**
         * Adds a system.
         * @tparam T The system type.
         * @param system The system.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, System*>>>
        void add(std::unique_ptr<T>&& system) {
            // Then, add the system
            systems.emplace(type_id<T>, std::move(system));
        }

        /**
         * Removes a system.
         * @tparam T The system type.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, System*>>>
        void remove() {
            // Then, remove the system
            systems.erase(type_id<T>);
        }

        /**
         * Removes all systems.
         */
        void clear();

        /**
         * Iterates through all valid systems.
         * @tparam Func The function type.
         * @param func The function to pass each system into.
         */
        template<typename F>
        void each(const F& func) {
            for (const auto& [typeId, system] : systems) {
                func(system.get());
            }
        }

    private:
        /// List of all systems
        fst::unordered_flatmap<type_index, std::unique_ptr<System>> systems;
    };
}
