#pragma once

#include "fusion/scene/system.h"
#include "fusion/scene/components.h"

namespace fe {
    class CameraSystem final : public System {
    public:
        explicit CameraSystem(entt::registry& registry);
        ~CameraSystem() override;

    private:
        void onPlay() override {};
        void onUpdate() override;
        void onStop() override {};
        void onEnabled() override {};
        void onDisabled() override {};
    };
}
