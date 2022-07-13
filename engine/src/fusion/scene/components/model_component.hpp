#pragma once

#include <cereal/cereal.hpp>

namespace fe {
    struct ModelComponent {
        fs::path path;
        //bool castShadows{ false };

        // Storage for runtime
        void* runtimeModel{ nullptr };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("Path", path));
        }
    };
    struct DirtyMeshComponent {};
}