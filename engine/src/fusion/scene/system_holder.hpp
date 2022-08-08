#pragma once

#include "fusion/scene/system.hpp"

namespace fe {
    /**
     * @brief Class that contains and manages systems registered to a scene.
     */
    class SystemHolder {
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
            auto it = systems.find(type_id<T>);
            return it != systems.end() ? reinterpret_cast<T*>(it->second.get()) : nullptr;
        }

        /**
         * Adds a system.
         * @tparam T The system type.
         * @param system The system.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, System*>>>
        void add(std::unique_ptr<T>&& system) {
            systems.emplace(type_id<T>, std::move(system));
        }

        /**
         * Removes a system.
         * @tparam T The system type.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, System*>>>
        void remove() {
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
            for (const auto& system : systems.values()) {
                func(system.get());
            }
        }

    private:
        /// List of all systems
        std::flat_map<type_index, std::unique_ptr<System>> systems;
    };
}
