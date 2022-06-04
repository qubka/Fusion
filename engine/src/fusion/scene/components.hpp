#pragma once

#include "scene_camera.hpp"

#include <entt/entt.hpp>

namespace fe {

    enum Space {
        World,
        Local
    };

    /*static std::random_device RandomDevice;
    static std::mt19937_64 Engine(RandomDevice());
    static std::uniform_int_distribution<uint64_t> UniformDistribution;

    struct IdComponent {
        uint64_t id;

        IdComponent() : id{UniformDistribution(Engine)} {}
        explicit IdComponent(uint64_t id) : id{id} {}

        uint64_t& operator*() { return id; }
        const uint64_t& operator*() const { return id; }
        operator uint64_t() const { return id; }
    };*/

    struct TagComponent {
        std::string tag;

        std::string& operator*() { return tag; }
        const std::string& operator*() const { return tag; }
        operator std::string() const { return tag; }
    };

    struct DirtyComponent {
    };

    struct RelationshipComponent {
        size_t children{ 0 }; //! the number of children for the given entity.
        entt::entity first{ entt::null }; //! the entity identifier of the first child, if any.
        entt::entity prev{ entt::null }; // the previous sibling in the list of children for the parent.
        entt::entity next{ entt::null }; // the next sibling in the list of children for the parent.
        entt::entity parent{ entt::null }; // the entity identifier of the parent, if any.
    };

    struct TransformComponent { // http://graphics.cs.cmu.edu/courses/15-466-f17/notes/hierarchy.html
        glm::vec3 translation{ 0.0f };
        glm::quat rotation{ quat::identity };
        glm::vec3 scale{ 1.0f };

        /*glm::vec3 localTranslation{ 0.0f };
        glm::quat localRotation{ quat::identity };
        glm::vec3 localScale{ 1.0f };*/

        glm::mat4 worldToLocalMatrix{ 1.0f };
        glm::mat4 localToWorldMatrix{ 1.0f };

        glm::mat4 make_local_to_parent() const {
            return glm::mat4{ //translate
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f, 0.0f },
                { translation, 1.0f }
            }
            * glm::mat4_cast(rotation) //rotate
            * glm::mat4{ //scale
                { scale.x, 0.0f, 0.0f, 0.0f },
                { 0.0f, scale.y, 0.0f, 0.0f },
                { 0.0f, 0.0f, scale.z, 0.0f },
                { 0.0f, 0.0f, 0.0f, 1.0f }
            };
        }

        glm::mat4 make_parent_to_local() const {
            glm::vec3 inv_scale{
                (scale.x == 0.0f ? 0.0f : 1.0f / scale.x),
                (scale.y == 0.0f ? 0.0f : 1.0f / scale.y),
                (scale.z == 0.0f ? 0.0f : 1.0f / scale.z)
            };
            return glm::mat4{ //un-scale
                { inv_scale.x, 0.0f, 0.0f, 0.0f },
                { 0.0f, inv_scale.y, 0.0f, 0.0f },
                { 0.0f, 0.0f, inv_scale.z, 0.0f },
                { 0.0f, 0.0f, 0.0f, 1.0f }
            }
            * glm::mat4_cast(glm::inverse(rotation)) //un-rotate
            * glm::mat4{ //un-translate
                { 1.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 1.0f, 0.0f },
                { -translation, 1.0f }
            };
        }

        /*void lookAt(const Transform* pTarget, const glm::vec3& up) {
           glm::vec3 z = GetZ();
           glm::vec3 y = GetY();
           glm::vec3 forward = -z;
           glm::vec3 newForward = glm::normalize(pTarget->m_position - this->m_position);
           glm::quat q1{ forward, newForward }; // https://www.reddit.com/r/opengl/comments/t01fwn/how_to_store_rotation_transform_and_scale/

           glm::vec3 newRight = glm::normalize(glm::cross(newForward, up));
           glm::vec3 newUp = glm::cross(newRight, newForward);
           glm::quat q2{ glm::rotate(q1, y), newUp };

           Rotate(q2 * q1, Space::World);
       }

       // Get x-axis, y-axis and z-axis relative to parent node's space or world space
       glm::vec3 getLocalX() const {
           float x = localRotation.x;
           float y = localRotation.y;
           float z = localRotation.z;
           float w = localRotation.w;

           float xx = 1 - 2 * y * y - 2 * z * z;
           float xy = 2 * x * y + 2 * w * z;
           float xz = 2 * x * z - 2 * w * y;

           return glm::vec3{ xx, xy, xz };
       }

       glm::vec3 getLocalY() const {
           float x = localRotation.x;
           float y = localRotation.y;
           float z = localRotation.z;
           float w = localRotation.w;

           float yx = 2 * x * y - 2 * w * z;
           float yy = 1 - 2 * x * x - 2 * z * z;
           float yz = 2 * y * z + 2 * w * x;

           return{ yx, yy, yz };
       }

       glm::vec3 getLocalZ() const {
           float x = localRotation.x;
           float y = localRotation.y;
           float z = localRotation.z;
           float w = localRotation.w;

           float zx = 2 * x * z + 2 * w * y;
           float zy = 2 * y * z - 2 * w * x;
           float zz = 1 - 2 * x * x - 2 * y * y;

           return { zx, zy, zz };
       }

       glm::vec3 getX() const {
           float x = rotation.x;
           float y = rotation.y;
           float z = rotation.z;
           float w = rotation.w;

           float xx = 1 - 2 * y * y - 2 * z * z;
           float xy = 2 * x * y + 2 * w * z;
           float xz = 2 * x * z - 2 * w * y;

           return { xx, xy, xz };
       }

       glm::vec3 getY() const {
           float x = rotation.x;
           float y = rotation.y;
           float z = rotation.z;
           float w = rotation.w;

           float yx = 2 * x * y - 2 * w * z;
           float yy = 1 - 2 * x * x - 2 * z * z;
           float yz = 2 * y * z + 2 * w * x;

           return { yx, yy, yz };
       }

       glm::vec3 getZ() const {
           float x = rotation.x;
           float y = rotation.y;
           float z = rotation.z;
           float w = rotation.w;

           float zx = 2 * x * z + 2 * w * y;
           float zy = 2 * y * z - 2 * w * x;
           float zz = 1 - 2 * x * x - 2 * y * y;

           return { zx, zy, zz };
       }*/

        glm::vec3 getUp() const { return rotation * vec3::up; }
        glm::vec3 getForward() const { return rotation * vec3::forward; }
        glm::vec3 getRight() const { return rotation * vec3::right; }

        glm::vec3 transformDirection(const glm::vec3& direction) const { return glm::rotate(rotation, direction); }
        glm::vec3 transformVector(const glm::vec3& vector) const { return glm::rotate(rotation, scale * vector); }
        glm::vec3 transformPoint(const glm::vec3& point) const { return glm::rotate(rotation, scale * point) + translation; }
        glm::vec3 inverseTransformDirection(const glm::vec3& direction) const { return glm::rotate(glm::inverse(rotation), direction); }
        glm::vec3 inverseTransformVector(const glm::vec3& vector) const { return glm::rotate(glm::inverse(rotation), vector / scale); }
        glm::vec3 inverseTransformPoint(const glm::vec3& point) const { return glm::rotate(glm::inverse(rotation), (point - translation) / scale); }
    };

    struct BoundsComponent {
        glm::vec3 min{ 0.0f };
        glm::vec3 max{ 0.0f };
        glm::vec3 size{ 0.0f };
    };

    struct ModelComponent {
        std::string path;
        glm::vec3 scale{ 1.0f };
        glm::vec3 center{ 0.0f };
        glm::vec2 uvscale{ 1.0f };
    };

    struct CameraComponent {
        SceneCamera camera;
        bool primary{ true };
        bool fixedAspectRatio{ false };
    };
}
