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
                return static_cast<T*>(it->second.get());
            } else {
                throw std::runtime_error("System Holder does not have requested system");
            }
        }

        /**
         * Adds a system.
         * @tparam T The system type.
         * @param system The system.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, System*>>>
        void add(std::unique_ptr<T>&& system) {
            auto it = std::find_if(systems.begin(), systems.end(), [](const auto& p) {
                return p.first == type_id<T>;
            });

            if (it == systems.end()) {
                // Then, add the system
                systems.emplace_back(type_id<T>, std::move(system));
            } else {
                throw std::runtime_error("System Holder already have requested system");
            }
        }

        /**
         * Removes a system.
         * @tparam T The system type.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, System*>>>
        void remove() {
            auto it = std::find_if(systems.begin(), systems.end(), [](const auto& p) {
                return p.first == type_id<T>;
            });

            if (it != systems.end()) {
                // Then, remove the system
                systems.erase(it);
            } else {
                throw std::runtime_error("System Holder does not requested system to remove");
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
        template<typename F>
        void each(const F& func) {
            for (const auto& [type, system] : systems) {
                func(system.get());
            }
        }

    private:
        /// List of all systems
        std::vector<std::pair<type_index, std::unique_ptr<System>>> systems;
    };
}
