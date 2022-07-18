#pragma once

#include "fusion/utils/glm_extention.hpp"

namespace fe {
    class Transform {
    public:
        enum class Space { Self, Parent, World };

        Transform() = default;
        explicit Transform(const glm::mat4& matrix);
        explicit Transform(const glm::vec3& position);
        ~Transform() = default;

        void setWorldMatrix(const glm::mat4& mat);
        void setLocalTransform(const glm::mat4& localMat);

        void setLocalPosition(const glm::vec3& localPos);
        void setLocalScale(const glm::vec3& localScale);
        void setLocalOrientation(const glm::quat& rotation);
        void setLocalOrientation(const glm::vec3& axis, float angle);

        const glm::mat4& getParentMatrix() const { return parentMatrix; }
        const glm::mat4& getWorldMatrix();
        const glm::mat4& getLocalMatrix();

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

        void translateLocal(const glm::vec3& translation);
        void scaleLocal(const glm::vec3& scale);

        void rotate(glm::quat rotation, Space space = Space::Self);
        void rotate(const glm::vec3& axis, float angle, Space space = Space::Self);

        void lookAt(glm::vec3 target, glm::vec3 up);

        glm::vec3 transformPoint(const glm::vec3& point);
        glm::vec3 transformDirection(const glm::vec3& direction);
        //glm::vec3 inverseTransformPoint(const glm::vec3& point);
        //glm::vec3 inverseTransformDirection(const glm::vec3& direction);

        void calcMatrices();
        void applyTransform();

        bool isUpdated() const { return updated; }
        void setUpdated(bool flag) { updated = flag; }
        void setDirty(bool flag) { dirty = flag; }

    private:
        glm::mat4 parentMatrix{ 1.0f };
        glm::mat4 localMatrix{ 1.0f };
        glm::mat4 worldMatrix{ 1.0f };

        glm::vec3 localPosition{ vec3::zero };
        glm::quat localOrientation{ quat::identity };
        glm::vec3 localScale{ vec3::one };

        bool updated{ false };
        bool dirty{ false };
    };
}