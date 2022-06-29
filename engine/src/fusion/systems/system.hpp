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
        //virtual void create() {};

        /**
         * @brief Before the first OnUpdate and whenever the system resumes running.
         */
        virtual void start() {};

        /**
         * @brief Every frame as long as the system has work to do (see ShouldRunSystem()) and the system is Enabled.
         */
        virtual void update() {};

        /**
         * @brief Whenever the system stops updating because it finds no entities matching its queries. Also called before OnDestroy.
         */
        //virtual void stop() {};

        /**
         * @brief When the system is destroyed
         */
        //virtual void destroy() {};

    protected:
        entt::registry& registry;
    };
}