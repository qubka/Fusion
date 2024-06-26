#pragma once

namespace fe {
    class FUSION_API Transform {
    public:
        enum class Space : unsigned char { Self, Parent, World };

        Transform() = default;
        explicit Transform(const glm::mat4& local);
        Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
        Transform(const glm::mat4& parent, const glm::mat4& local);
        ~Transform() = default;

        void setWorldMatrix(const glm::mat4& mat);
        void setLocalTransform(const glm::mat4& localMat);

        bool setLocalPosition(const glm::vec3& localPos);
        bool setLocalScale(const glm::vec3& localScale);
        bool setLocalOrientation(const glm::quat& rotation);
        bool setLocalOrientation(const glm::vec3& axis, float angle);

        const glm::mat4& getParentMatrix() const { return parentMatrix; }
        const glm::mat4& getWorldMatrix() const;
        const glm::mat4& getLocalMatrix() const;
        const glm::mat3& getNormalMatrix() const;

        glm::vec3 getWorldPosition() const { return worldMatrix[3]; }
        glm::quat getWorldOrientation() const { return glm::toQuat(worldMatrix); }

        const glm::vec3& getLocalPosition() const { return localPosition; }
        const glm::quat& getLocalOrientation() const { return localOrientation; }
        const glm::vec3& getLocalScale() const { return localScale; }

        glm::vec3 getWorldUpDirection() const;
        glm::vec3 getLocalUpDirection() const;
        glm::vec3 getWorldDownDirection() const;
        glm::vec3 getLocalDownDirection() const;
        glm::vec3 getWorldLeftDirection() const;
        glm::vec3 getLocalLeftDirection() const;
        glm::vec3 getWorldRightDirection() const;
        glm::vec3 getLocalRightDirection() const;
        glm::vec3 getWorldForwardDirection() const;
        glm::vec3 getLocalForwardDirection() const;
        glm::vec3 getWorldBackDirection() const;
        glm::vec3 getLocalBackDirection() const;

        bool translateLocal(const glm::vec3& translation);
        bool scaleLocal(const glm::vec3& scale);

        bool rotate(glm::quat rotation, Space space = Space::Self);
        bool rotate(const glm::vec3& axis, float angle, Space space = Space::Self);
        bool lookAt(glm::vec3 target, glm::vec3 up);

        glm::vec3 transformPoint(const glm::vec3& point);
        glm::vec3 transformDirection(const glm::vec3& direction);
        //glm::vec3 inverseTransformPoint(const glm::vec3& point);
        //glm::vec3 inverseTransformDirection(const glm::vec3& direction);

    protected:
        void calcMatrices() const;
        void applyTransform();

        glm::vec3 localPosition{ vec3::zero };
        glm::quat localOrientation{ quat::identity };
        glm::vec3 localScale{ vec3::one };

        glm::mat4 parentMatrix{ 1.0f };
        mutable glm::mat4 localMatrix{ 1.0f };
        mutable glm::mat4 worldMatrix{ 1.0f };
        mutable glm::mat3 normalMatrix{ 1.0f };

        mutable bool dirty{ true };
    };
}
