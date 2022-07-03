#pragma once

namespace fe {
    class System {
    public:
        System() = default;
        virtual ~System() = default;
        NONCOPYABLE(System);

        /**
         * @brief Before the first OnUpdate and whenever the system resumes running.
         */
        virtual void start() = 0;

        /**
         * @brief Every frame as long as the system has work to do (see ShouldRunSystem()) and the system is Enabled.
         */
        virtual void update() = 0;

        bool isEnabled() const { return enabled; }
        void setEnabled(bool enable) { this->enabled = enable; }

    protected:
        bool enabled{ true };
    };
}