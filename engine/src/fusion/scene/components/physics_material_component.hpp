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
            archive(cereal::make_nvp("dynamic friction", dynamicFriction),
                    cereal::make_nvp("static friction", staticFriction),
                    cereal::make_nvp("restitution", restitution),
                    cereal::make_nvp("friction combine", frictionCombine),
                    cereal::make_nvp("restitution combine", restitutionCombine));
        }
    };
}