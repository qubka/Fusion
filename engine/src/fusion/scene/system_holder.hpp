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
         * Checks whether a System exists or not.
         * @tparam T The System type.
         * @return If the System exists.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, System*>>>
        bool has() const {
            const auto it = systems.find(typeid(T));
            return it != systems.end() && it->second;
        }

        /**
         * Gets a System.
         * @tparam T The System type.
         * @return The System.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, System*>>>
        T* get() const {
            const auto& typeId = typeid(T);

            if (auto it = systems.find(typeId); it != systems.end() && it->second)
                return static_cast<T*>(it->second.get());

            //throw std::runtime_error("Scene does not have requested System");
            return nullptr;
        }

        /**
         * Adds a System.
         * @tparam T The System type.
         * @param system The System.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, System*>>>
        void add(std::unique_ptr<T>&& system) {
            // Remove previous System, if it exists
            remove<T>();

            const auto& typeId = typeid(T);

            // Then, add the System
            systems[typeId] = std::move(system);
        }

        /**
         * Removes a System.
         * @tparam T The System type.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, System*>>>
        void remove() {
            const auto& typeId = typeid(T);

            // Then, remove the System.
            systems.erase(typeId);
        }

        /**
         * Removes all Systems.
         */
        void clear();

        /**
         * Iterates through all valid Systems.
         * @tparam Func The function type.
         * @param func The function to pass each System into, System object and System ID.
         */
        template<typename Func>
        void each(Func&& func) {
            for (auto& [typeId, system] : systems) {
                func(system.get());
            }
        }

    private:
        std::unordered_map<std::type_index, std::unique_ptr<System>> systems;
    };
}
