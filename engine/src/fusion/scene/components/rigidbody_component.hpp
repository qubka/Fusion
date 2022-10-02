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
            archive(cereal::make_nvp("type", type));
            archive(cereal::make_nvp("mass", mass));
            archive(cereal::make_nvp("linear drag", linearDrag));
            archive(cereal::make_nvp("angular drag", angularDrag));
            archive(cereal::make_nvp("disable gravity", disableGravity));
            archive(cereal::make_nvp("kinematic", kinematic));
            archive(cereal::make_nvp("sleep", sleep));
            archive(cereal::make_nvp("freeze position", freezePosition));
            archive(cereal::make_nvp("freeze rotation", freezeRotation));
        }
    };
}
