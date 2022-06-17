#include "scene_serializer.hpp"
#include "scene.hpp"
#include "components.hpp"

#include <yaml-cpp/yaml.h>

namespace YAML {

    template<>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs) {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::ivec2>
    {
        static Node encode(const glm::ivec2& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::ivec2& rhs) {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<int>();
            rhs.y = node[1].as<int>();
            return true;
        }
    };

    template<>
    struct convert<glm::bvec2>
    {
        static Node encode(const glm::bvec2& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::bvec2& rhs) {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<bool>();
            rhs.y = node[1].as<bool>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs) {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::ivec3>
    {
        static Node encode(const glm::ivec3& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::ivec3& rhs) {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<int>();
            rhs.y = node[1].as<int>();
            rhs.z = node[2].as<int>();
            return true;
        }
    };

    template<>
    struct convert<glm::bvec3>
    {
        static Node encode(const glm::bvec3& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::bvec3& rhs) {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<bool>();
            rhs.y = node[1].as<bool>();
            rhs.z = node[2].as<bool>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs) {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::ivec4>
    {
        static Node encode(const glm::ivec4& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::ivec4& rhs) {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<int>();
            rhs.y = node[1].as<int>();
            rhs.z = node[2].as<int>();
            rhs.w = node[3].as<int>();
            return true;
        }
    };

    template<>
    struct convert<glm::bvec4>
    {
        static Node encode(const glm::bvec4& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::bvec4& rhs) {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<bool>();
            rhs.y = node[1].as<bool>();
            rhs.z = node[2].as<bool>();
            rhs.w = node[3].as<bool>();
            return true;
        }
    };

    template<>
    struct convert<glm::quat>
    {
        static Node encode(const glm::quat& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::quat& rhs) {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::mat4>
    {
        static Node encode(const glm::mat4& rhs) {
            Node node;
            for (int i = 0; i < 4; i++) {
                auto& col = rhs[i];
                node.push_back(col.x);
                node.push_back(col.y);
                node.push_back(col.z);
                node.push_back(col.w);
            }
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::mat4& rhs) {
            if (!node.IsSequence() || node.size() != 16)
                return false;

            for (int i = 0, j = 0; i < 4; i++, j+=4) {
                rhs[i] = {
                    node[j+0].as<float>(),
                    node[j+1].as<float>(),
                    node[j+2].as<float>(),
                    node[j+3].as<float>()
                };
            }
            return true;
        }
    };

    template<>
    struct convert<entt::entity>
    {
        static Node encode(const entt::entity& rhs) {
            return Node(static_cast<int>(rhs));
        }

        static bool decode(const Node& node, entt::entity& rhs) {
            if (node.IsNull())
                return false;
            rhs = static_cast<entt::entity>(node.as<int>());
            return true;
        }
    };

    Emitter& operator<<(Emitter& out, const glm::vec2& v) {
        out << Flow;
        out << BeginSeq << v.x << v.y << EndSeq;
        return out;
    }

    Emitter& operator<<(Emitter& out, const glm::vec3& v) {
        out << Flow;
        out << BeginSeq << v.x << v.y << v.z << EndSeq;
        return out;
    }

    Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
        out << Flow;
        out << BeginSeq << v.x << v.y << v.z << v.w << EndSeq;
        return out;
    }

    Emitter& operator<<(YAML::Emitter& out, const glm::quat& q) {
        out << Flow;
        out << BeginSeq << q.x << q.y << q.z << q.w << EndSeq;
        return out;
    }

    Emitter& operator<<(YAML::Emitter& out, entt::entity entity) {
        out << static_cast<int>(entity);
        return out;
    }

    Emitter& operator<<(Emitter& out, const glm::bvec3& v) {
        out << Flow;
        out << BeginSeq << v.x << v.y << v.z << EndSeq;
        return out;
    }
}

using namespace fe;

SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene) : scene{scene} {

}

void SceneSerializer::serialize(const std::string& filename) {
    assert(std::filesystem::exists(filename));

    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "Untitled";
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

    scene->registry.each([&](const auto entity) {
        out << YAML::BeginMap; // Entity
        out << YAML::Key << "Entity" << YAML::Value << entity;

        if (auto component = scene->registry.try_get<TagComponent>(entity)) {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap; // TagComponent
            out << YAML::Key << "tag" << YAML::Value << component->tag;
            out << YAML::EndMap; // TagComponent
        }

        if (auto component = scene->registry.try_get<TransformComponent>(entity)) {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap; // TransformComponent

            out << YAML::Key << "position" << YAML::Value << component->position;
            out << YAML::Key << "rotation" << YAML::Value << component->rotation;
            out << YAML::Key << "scale" << YAML::Value << component->scale;

            out << YAML::EndMap; // TransformComponent
        }

        if (auto component = scene->registry.try_get<RelationshipComponent>(entity)) {
            out << YAML::Key << "RelationshipComponent";
            out << YAML::BeginMap; // RelationshipComponent

            out << YAML::Key << "children" << YAML::Value << component->children;
            out << YAML::Key << "first" << YAML::Value << component->first;
            out << YAML::Key << "prev" << YAML::Value << component->prev;
            out << YAML::Key << "next" << YAML::Value << component->next;
            out << YAML::Key << "parent" << YAML::Value << component->parent;

            out << YAML::EndMap; // RelationshipComponent
        }

        if (auto component = scene->registry.try_get<CameraComponent>(entity)) {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap; // CameraComponent

            auto& camera = component->camera;

            out << YAML::Key << "Camera" << YAML::Value;
            out << YAML::BeginMap; // Camera
            out << YAML::Key << "projectionType" << YAML::Value << me::enum_index(camera.getProjectionType()).value_or(0);
            out << YAML::Key << "perspectiveFOV" << YAML::Value << camera.getPerspectiveVerticalFOV();
            out << YAML::Key << "perspectiveNear" << YAML::Value << camera.getPerspectiveNearClip();
            out << YAML::Key << "perspectiveFar" << YAML::Value << camera.getPerspectiveFarClip();
            out << YAML::Key << "orthographicSize" << YAML::Value << camera.getOrthographicSize();
            out << YAML::Key << "orthographicNear" << YAML::Value << camera.getOrthographicNearClip();
            out << YAML::Key << "orthographicFar" << YAML::Value << camera.getOrthographicFarClip();
            out << YAML::EndMap; // Camera

            out << YAML::Key << "primary" << YAML::Value << component->primary;
            out << YAML::Key << "fixedAspectRatio" << YAML::Value << component->fixedAspectRatio;

            out << YAML::EndMap; // CameraComponent
        }

        if (auto component = scene->registry.try_get<MeshComponent>(entity)) {
            out << YAML::Key << "MeshComponent";
            out << YAML::BeginMap; // MeshComponent

            out << YAML::Key << "path" << YAML::Value << component->path;
            //out << YAML::Key << "layout" << YAML::Value << component->layout;
            out << YAML::Key << "scale" << YAML::Value << component->scale;
            out << YAML::Key << "center" << YAML::Value << component->center;
            out << YAML::Key << "uvscale" << YAML::Value << component->uvscale;

            out << YAML::EndMap; // MeshComponent
        }

        if (auto component = scene->registry.try_get<RigidbodyComponent>(entity)) {
            out << YAML::Key << "RigidbodyComponent";
            out << YAML::BeginMap; // RigidbodyComponent

            out << YAML::Key << "type" << YAML::Value << me::enum_index(component->type).value_or(0);
            out << YAML::Key << "mass" << YAML::Value << component->mass;
            out << YAML::Key << "linearDrag" << YAML::Value << component->linearDrag;
            out << YAML::Key << "angularDrag" << YAML::Value << component->angularDrag;
            out << YAML::Key << "disableGravity" << YAML::Value << component->disableGravity;
            out << YAML::Key << "kinematic" << YAML::Value << component->kinematic;
            out << YAML::Key << "freezePosition" << YAML::Value << component->freezePosition;
            out << YAML::Key << "freezeRotation" << YAML::Value << component->freezeRotation;

            out << YAML::EndMap; // RigidbodyComponent
        }

        if (auto component = scene->registry.try_get<BoxColliderComponent>(entity)) {
            out << YAML::Key << "BoxColliderComponent";
            out << YAML::BeginMap; // BoxColliderComponent

            out << YAML::Key << "extent" << YAML::Value << component->extent;
            out << YAML::Key << "trigger" << YAML::Value << component->trigger;

            out << YAML::EndMap; // BoxColliderComponent
        }

        if (auto component = scene->registry.try_get<SphereColliderComponent>(entity)) {
            out << YAML::Key << "SphereColliderComponent";
            out << YAML::BeginMap; // SphereColliderComponent

            out << YAML::Key << "radius" << YAML::Value << component->radius;
            out << YAML::Key << "trigger" << YAML::Value << component->trigger;

            out << YAML::EndMap; // SphereColliderComponent
        }

        if (auto component = scene->registry.try_get<CapsuleColliderComponent>(entity)) {
            out << YAML::Key << "CapsuleColliderComponent";
            out << YAML::BeginMap; // CapsuleColliderComponent

            out << YAML::Key << "radius" << YAML::Value << component->radius;
            out << YAML::Key << "height" << YAML::Value << component->height;
            out << YAML::Key << "trigger" << YAML::Value << component->trigger;

            out << YAML::EndMap; // CapsuleColliderComponent
        }

        if (auto component = scene->registry.try_get<PhysicsMaterialComponent>(entity)) {
            out << YAML::Key << "PhysicsMaterialComponent";
            out << YAML::BeginMap; // PhysicsMaterialComponent

            out << YAML::Key << "dynamicFriction" << YAML::Value << component->dynamicFriction;
            out << YAML::Key << "staticFriction" << YAML::Value << component->staticFriction;
            out << YAML::Key << "restitution" << YAML::Value << component->restitution;
            out << YAML::Key << "frictionCombine" << YAML::Value << me::enum_index(component->frictionCombine).value_or(0);
            out << YAML::Key << "restitutionCombine" << YAML::Value << me::enum_index(component->restitutionCombine).value_or(0);

            out << YAML::EndMap; // PhysicsMaterialComponent
        }

        if (auto component = scene->registry.try_get<MaterialComponent>(entity)) {
            out << YAML::Key << "MaterialComponent";
            out << YAML::BeginMap; // MaterialComponent

            /*out << YAML::Key << "ambient" << YAML::Value << component->ambient;
            out << YAML::Key << "diffuse" << YAML::Value << component->diffuse;
            out << YAML::Key << "emission" << YAML::Value << component->emission;
            out << YAML::Key << "specular" << YAML::Value << component->specular;
            out << YAML::Key << "shininess" << YAML::Value << component->shininess;*/

            out << YAML::EndMap; // MaterialComponent
        }

        out << YAML::EndMap; // Entity
    });

    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream fout(filename);
    fout << out.c_str();
}

bool SceneSerializer::deserialize(const std::string& filename) {
    assert(std::filesystem::exists(filename));

    YAML::Node data;
    try {
        data = YAML::LoadFile(filename);
    } catch (YAML::ParserException& e) {
        return false;
    }

    if (!data["Scene"])
        return false;

    auto sceneName = data["Scene"].as<std::string>();
    LOG_DEBUG << "Deserializing scene:" << sceneName;

    if (const auto& entities = data["Entities"]) {
        for (const auto& entity : entities) {
            auto uuid = entity["Entity"].as<entt::entity>();
            auto deserializedEntity = scene->registry.create(uuid);

            std::string name;
            if (const auto& tagComponent = entity["TagComponent"]) {
                name = tagComponent["tag"].as<std::string>();
                scene->registry.emplace<TagComponent>(deserializedEntity, name);
            }

            LOG_DEBUG << "Deserialized entity with ID = " << static_cast<int>(uuid) << ", name = " << name;

            if (const auto& component = entity["TransformComponent"]) {
                scene->registry.emplace<TransformComponent>(deserializedEntity,
                                                            component["position"].as<glm::vec3>(),
                                                            component["rotation"].as<glm::quat>(),
                                                            component["scale"].as<glm::vec3>());
            }

            if (const auto& component = entity["RelationshipComponent"]) {
                scene->registry.emplace<RelationshipComponent>(deserializedEntity,
                                                               component["children"].as<size_t>(),
                                                               component["first"].as<entt::entity>(),
                                                               component["prev"].as<entt::entity>(),
                                                               component["next"].as<entt::entity>(),
                                                               component["parent"].as<entt::entity>());
            }

            if (const auto& component = entity["CameraComponent"]) {
                const auto& cameraProps = component["Camera"];

                SceneCamera camera;
                camera.setProjectionType(me::enum_value<SceneCamera::ProjectionType>(cameraProps["projectionType"].as<size_t>()));
                camera.setPerspectiveVerticalFOV(cameraProps["perspectiveFOV"].as<float>());
                camera.setPerspectiveNearClip(cameraProps["perspectiveNear"].as<float>());
                camera.setPerspectiveFarClip(cameraProps["perspectiveFar"].as<float>());
                camera.setOrthographicSize(cameraProps["orthographicSize"].as<float>());
                camera.setOrthographicNearClip(cameraProps["orthographicNear"].as<float>());
                camera.setOrthographicFarClip(cameraProps["orthographicFar"].as<float>());

                scene->registry.emplace<CameraComponent>(deserializedEntity,
                                                         camera,
                                                         component["primary"].as<bool>(),
                                                         component["fixedAspectRatio"].as<bool>());
            }

            if (const auto& meshComponent = entity["MeshComponent"]) {
                scene->registry.emplace<MeshComponent>(deserializedEntity,
                                                       meshComponent["path"].as<std::string>(),
                                                       //meshComponent["layout"].as<std::vector<int>>(),
                                                       meshComponent["scale"].as<glm::vec3>(),
                                                       meshComponent["center"].as<glm::vec3>(),
                                                       meshComponent["uvscale"].as<glm::vec2>());
            }

            if (const auto& component = entity["RigidbodyComponent"]) {
                scene->registry.emplace<RigidbodyComponent>(deserializedEntity,
                                                            me::enum_value<RigidbodyComponent::BodyType>(component["type"].as<size_t>()),
                                                            component["mass"].as<float>(),
                                                            component["linearDrag"].as<float>(),
                                                            component["angularDrag"].as<float>(),
                                                            component["disableGravity"].as<bool>(),
                                                            component["kinematic"].as<bool>(),
                                                            component["freezePosition"].as<glm::bvec3>(),
                                                            component["freezeRotation"].as<glm::bvec3>());
            }

            if (const auto& component = entity["BoxColliderComponent"]) {
                scene->registry.emplace<BoxColliderComponent>(deserializedEntity,
                                                              component["extent"].as<glm::vec3>(),
                                                              component["trigger"].as<bool>());
            }

            if (const auto& component = entity["SphereColliderComponent"]) {
                scene->registry.emplace<SphereColliderComponent>(deserializedEntity,
                                                                 component["radius"].as<float>(),
                                                                 component["trigger"].as<bool>());
            }

            if (const auto& component = entity["CapsuleColliderComponent"]) {
                scene->registry.emplace<CapsuleColliderComponent>(deserializedEntity,
                                                                  component["radius"].as<float>(),
                                                                  component["height"].as<float>(),
                                                                  component["trigger"].as<bool>());
            }

            if (const auto& component = entity["PhysicsMaterialComponent"]) {
                scene->registry.emplace<PhysicsMaterialComponent>(deserializedEntity,
                                                                  component["dynamicFriction"].as<float>(),
                                                                  component["staticFriction"].as<float>(),
                                                                  component["restitution"].as<float>(),
                                                                  me::enum_value<PhysicsMaterialComponent::CombineMode>(component["frictionCombine"].as<size_t>()),
                                                                  me::enum_value<PhysicsMaterialComponent::CombineMode>(component["restitutionCombine"].as<size_t>()));
            }

            if (const auto& component = entity["MaterialComponent"]) {
                /*scene->registry.emplace<MaterialComponent>(deserializedEntity,
                                                           component["ambient"].as<glm::vec4>(),
                                                           component["diffuse"].as<glm::vec4>(),
                                                           component["emission"].as<glm::vec4>(),
                                                           component["specular"].as<glm::vec3>(),
                                                           component["shininess"].as<float>());*/
            }
        }
    }

    return true;
}
