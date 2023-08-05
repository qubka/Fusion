#pragma once

namespace fe {
    class FUSION_API System {
        friend class Scene;
    public:
        explicit System(entt::registry& registry) : registry{registry} {}
        virtual ~System() = default;
        NONCOPYABLE(System);

        bool isEnabled() const { return enabled; }
        void setEnabled(bool flag) {
            if (enabled == flag)
                return;
            enabled = flag;
            if (enabled)
                onEnabled();
            else
                onDisabled();
        }

    protected:
        /**
         * @brief Whenever the system starts updating because scene in the active state.
         */
        virtual void onPlay() = 0;

        /**
         * @brief Every frame as long as the system has work to do and the system is Enabled.
         */
        virtual void onUpdate() = 0;

        /**
        * @brief Whenever the system stops updating because scene not in the active state.
        */
        virtual void onStop() = 0;

        /**
         * @brief Called when the system is enabled.
         */
        virtual void onEnabled() {};

        /**
         * @brief Called when the system is disabled.
         */
        virtual void onDisabled() {};

    protected:
        entt::registry& registry;
        bool enabled{ false };
    };
}