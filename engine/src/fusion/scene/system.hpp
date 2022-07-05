#pragma once

#include <entt/entity/registry.hpp>

namespace fe {
    class System {
    public:
        System(entt::registry& registry) : registry{registry} {}
        virtual ~System() = default;
        NONCOPYABLE(System);

        /**
         * @brief Called when the system is created.
         */
        virtual void onCreate() {};

        /**
         * @brief Whenever the system starts updating because scene in the active state.
         */
        virtual void onStart() {};

        /**
         * @brief Every frame as long as the system has work to do and the system is Enabled.
         */
        virtual void onUpdate() {};

        /**
        * @brief Whenever the system stops updating because scene not in the active state.
        */
        virtual void onStop() {};

        bool isEnabled() const { return enabled; }
        void setEnabled(bool enable) { this->enabled = enable; }

    protected:
        bool enabled{ true };
        entt::registry& registry;
    };
}