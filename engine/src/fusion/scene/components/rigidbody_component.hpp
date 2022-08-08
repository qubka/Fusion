#pragma once

namespace fe {
    struct RigidbodyComponent {
        enum class BodyType : unsigned char { Static = 0, Dynamic = 1 };
        BodyType type{ BodyType::Static };
        //Layer layer;
        float mass{ 1.0f };
        float linearDrag{ 0.0f };
        float angularDrag{ 0.05f };
        bool disableGravity{ false };
        bool kinematic{ false };
        bool sleep{ false };
        glm::bvec3 freezePosition{ false };
        glm::bvec3 freezeRotation{ false };

        // Storage for runtime
        void* runtime{ nullptr };

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("type", type),
                    cereal::make_nvp("mass", mass),
                    cereal::make_nvp("linear drag", linearDrag),
                    cereal::make_nvp("angular drag", angularDrag),
                    cereal::make_nvp("disable gravity", disableGravity),
                    cereal::make_nvp("kinematic", kinematic),
                    cereal::make_nvp("sleep", sleep),
                    cereal::make_nvp("freeze position", freezePosition),
                    cereal::make_nvp("freeze rotation", freezeRotation));
        }
    };
}
