#pragma once

#include <entt/entity/registry.hpp>

namespace fe {
    class System {
    public:
        explicit System(entt::registry& registry) : registry{registry} {}
        virtual ~System() = default;
        NONCOPYABLE(System);

        /**
         * @brief Run when switching to this scene from another.
         */
        virtual void onStart() {};

        /**
         * @brief Whenever the system starts updating because scene in the active state.
         */
        virtual void onPlay() {};

        /**
         * @brief Every frame as long as the system has work to do and the system is Enabled.
         */
        virtual void onUpdate() {};

        /**
        * @brief Whenever the system stops updating because scene not in the active state.
        */
        virtual void onStop() {};

        bool isEnabled() const { return enabled; }
        void setEnabled(bool flag) { enabled = flag; }

    protected:
        entt::registry& registry;
        bool enabled{ true };
    };
}