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
            const auto& typeId = typeid(T);

            return indexed.find(typeId) != indexed.end();
        }

        /**
         * Gets a System.
         * @tparam T The System type.
         * @return The System.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, System*>>>
        T* get() const {
            const auto& typeId = typeid(T);

            if (auto it = indexed.find(typeId); it != indexed.end()) {
                auto& system = systems[it->second];
                return system ? static_cast<T*>(system.get()) : nullptr;
            }

            throw std::runtime_error("System Holder does not have requested System");
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
            indexed.emplace(typeId, systems.size());
            systems.push_back(std::move(system));
        }

        /**
         * Removes a System.
         * @tparam T The System type.
         */
        template<typename T, typename = std::enable_if_t<std::is_convertible_v<T*, System*>>>
        void remove() {
            const auto& typeId = typeid(T);

            // Then, remove the System
            if (auto it = indexed.find(typeId); it != indexed.end()) {
                systems.erase(systems.begin() + it->second);
                indexed.erase(it);
            }
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
            for (const auto& system : systems) {
                func(system.get());
            }
        }

    private:
        /// List of all Systems in insertion order
        std::vector<std::unique_ptr<System>> systems;
        /// Map of all indexed Systems in hash map
        std::unordered_map<std::type_index, size_t> indexed;
    };
}
