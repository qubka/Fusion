#pragma once

namespace fe {
    struct PhysicsMaterialComponent {
        enum class CombineMode : unsigned char { Average = 0, Minimum, Multiply, Maximum };

        float dynamicFriction{ 0.5f };
        float staticFriction{ 0.5f };
        float restitution{ 0.5f };
        CombineMode frictionCombine{ CombineMode::Average };
        CombineMode restitutionCombine{ CombineMode::Average };

        // Storage for runtime
        void* runtimeMaterial{ nullptr };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("dynamicFriction", dynamicFriction));
            archive(cereal::make_nvp("staticFriction", staticFriction));
            archive(cereal::make_nvp("restitution", restitution));
            archive(cereal::make_nvp("frictionCombine", frictionCombine));
            archive(cereal::make_nvp("restitutionCombine", restitutionCombine));
        }
    };
}