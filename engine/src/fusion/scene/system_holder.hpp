#pragma once

#include "system.hpp"

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
            auto it = std::find_if(systems.begin(), systems.end(), [](const auto& p) {
                return p.first == type_id<T>;
            });
            return it != systems.end() && it->second;
        }

        /**
         * Gets a system.
         * @tparam T The system type.
         * @return The system.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, System*>>>
        T* get() const {
            auto it = std::find_if(systems.begin(), systems.end(), [](const auto& p) {
                return p.first == type_id<T>;
            });
            if (it != systems.end() && it->second) {
                return reinterpret_cast<T*>(it->second.get());
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
            systems.emplace_back(type_id<T>, std::move(system));
        }

        /**
         * Removes a system.
         * @tparam T The system type.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, System*>>>
        void remove() {
            // Then, remove the System.
            for (auto it = systems.begin(); it != systems.end();) {
                if (it->first == type_id<T>) {
                    it = systems.erase(it);
                } else {
                    ++it;
                }
            }
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
        template<typename Func>
        void each(Func&& func) {
            for (const auto& [type, system] : systems) {
                func(system.get());
            }
        }

    private:
        /// List of all systems
        std::vector<std::pair<type_index, std::unique_ptr<System>>> systems;
    };
}
