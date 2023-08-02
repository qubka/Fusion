#pragma once

#include "fusion/scene/system.h"
#include "fusion/scene/components.h"

namespace fe {
    class ScriptSystem final : public System {
    public:
        explicit ScriptSystem(entt::registry& registry);
        ~ScriptSystem() override;

    private:
        void onPlay() override;
        void onUpdate() override;
        void onStop() override;
        void onEnabled() override {};
        void onDisabled() override {};
    };
}