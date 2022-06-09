#include "scene_serializer.hpp"
#include "scene.hpp"

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

    /* other */

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

void SceneSerializer::serialize(const std::string& filepath) {
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "Untitled";
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

    scene->world.each([&](const auto entity) {
        out << YAML::BeginMap; // Entity
        out << YAML::Key << "Entity" << YAML::Value << entity;

        if (auto component = scene->world.try_get<TagComponent>(entity)) {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap; // TagComponent
            out << YAML::Key << "tag" << YAML::Value << component->tag;
            out << YAML::EndMap; // TagComponent
        }

        if (auto component = scene->world.try_get<TransformComponent>(entity)) {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap; // TransformComponent

            out << YAML::Key << "position" << YAML::Value << component->position;
            out << YAML::Key << "rotation" << YAML::Value << component->rotation;
            out << YAML::Key << "scale" << YAML::Value << component->scale;

            out << YAML::EndMap; // TransformComponent
        }

        if (auto component = scene->world.try_get<RelationshipComponent>(entity)) {
            out << YAML::Key << "RelationshipComponent";
            out << YAML::BeginMap; // RelationshipComponent

            out << YAML::Key << "children" << YAML::Value << component->children;
            out << YAML::Key << "first" << YAML::Value << component->first;
            out << YAML::Key << "prev" << YAML::Value << component->prev;
            out << YAML::Key << "next" << YAML::Value << component->next;
            out << YAML::Key << "parent" << YAML::Value << component->parent;

            out << YAML::EndMap; // RelationshipComponent
        }

        if (auto component = scene->world.try_get<CameraComponent>(entity)) {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap; // CameraComponent

            auto& camera = component->camera;

            out << YAML::Key << "Camera" << YAML::Value;
            out << YAML::BeginMap; // Camera
            out << YAML::Key << "projectionType" << YAML::Value << magic_enum::enum_integer(camera.getProjectionType());
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

        if (auto component = scene->world.try_get<ModelComponent>(entity)) {
            out << YAML::Key << "ModelComponent";
            out << YAML::BeginMap; // ModelComponent

            out << YAML::Key << "path" << YAML::Value << component->path;
            //out << YAML::Key << "layout" << YAML::Value << component->layout;
            out << YAML::Key << "scale" << YAML::Value << component->scale;
            out << YAML::Key << "center" << YAML::Value << component->center;
            out << YAML::Key << "uvscale" << YAML::Value << component->uvscale;

            out << YAML::EndMap; // ModelComponent
        }

        if (auto component = scene->world.try_get<RigidbodyComponent>(entity)) {
            out << YAML::Key << "RigidbodyComponent";
            out << YAML::BeginMap; // RigidbodyComponent

            out << YAML::Key << "type" << YAML::Value << magic_enum::enum_integer(component->type);
            out << YAML::Key << "mass" << YAML::Value << component->mass;
            out << YAML::Key << "linearDrag" << YAML::Value << component->linearDrag;
            out << YAML::Key << "angularDrag" << YAML::Value << component->angularDrag;
            out << YAML::Key << "disableGravity" << YAML::Value << component->disableGravity;
            out << YAML::Key << "kinematic" << YAML::Value << component->kinematic;
            out << YAML::Key << "freezePosition" << YAML::Value << component->freezePosition;
            out << YAML::Key << "freezeRotation" << YAML::Value << component->freezeRotation;

            out << YAML::EndMap; // RigidbodyComponent
        }

        if (auto component = scene->world.try_get<BoxColliderComponent>(entity)) {
            out << YAML::Key << "BoxColliderComponent";
            out << YAML::BeginMap; // BoxColliderComponent

            out << YAML::Key << "center" << YAML::Value << component->center;
            out << YAML::Key << "size" << YAML::Value << component->size;
            out << YAML::Key << "trigger" << YAML::Value << component->trigger;

            out << YAML::EndMap; // BoxColliderComponent
        }

        if (auto component = scene->world.try_get<PhysicsMaterialComponent>(entity)) {
            out << YAML::Key << "PhysicsMaterialComponent";
            out << YAML::BeginMap; // PhysicsMaterialComponent

            out << YAML::Key << "friction" << YAML::Value << component->friction;
            out << YAML::Key << "restitution" << YAML::Value << component->restitution;

            out << YAML::EndMap; // PhysicsMaterialComponent
        }

        out << YAML::EndMap; // Entity
    });

    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream fout(filepath);
    fout << out.c_str();
}

bool SceneSerializer::deserialize(const std::string& filepath) {
    YAML::Node data;
    try {
        data = YAML::LoadFile(filepath);
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
            auto deserializedEntity = scene->world.create(uuid);

            std::string name;
            if (const auto& tagComponent = entity["TagComponent"]) {
                name = tagComponent["tag"].as<std::string>();
                scene->world.emplace<TagComponent>(deserializedEntity, name);
            }

            LOG_DEBUG << "Deserialized entity with ID = " << static_cast<int>(uuid) << ", name = " << name;

            if (const auto& component = entity["TransformComponent"]) {
                scene->world.emplace<TransformComponent>(deserializedEntity,
                    component["position"].as<glm::vec3>(),
                    component["rotation"].as<glm::quat>(),
                    component["scale"].as<glm::vec3>());
            }

            if (const auto& component = entity["RelationshipComponent"]) {
                scene->world.emplace<RelationshipComponent>(deserializedEntity,
                    component["children"].as<size_t>(),
                    component["first"].as<entt::entity>(),
                    component["prev"].as<entt::entity>(),
                    component["next"].as<entt::entity>(),
                    component["parent"].as<entt::entity>());
            }

            if (const auto& component = entity["CameraComponent"]) {
                const auto& cameraProps = component["Camera"];

                SceneCamera camera;
                camera.setProjectionType(magic_enum::enum_value<SceneCamera::ProjectionType>(cameraProps["projectionType"].as<int>()));
                camera.setPerspectiveVerticalFOV(cameraProps["perspectiveFOV"].as<float>());
                camera.setPerspectiveNearClip(cameraProps["perspectiveNear"].as<float>());
                camera.setPerspectiveFarClip(cameraProps["perspectiveFar"].as<float>());
                camera.setOrthographicSize(cameraProps["orthographicSize"].as<float>());
                camera.setOrthographicNearClip(cameraProps["orthographicNear"].as<float>());
                camera.setOrthographicFarClip(cameraProps["orthographicFar"].as<float>());

                scene->world.emplace<CameraComponent>(deserializedEntity,
                    camera,
                    component["primary"].as<bool>(),
                    component["fixedAspectRatio"].as<bool>());
            }

            if (const auto& modelComponent = entity["ModelComponent"]) {
                scene->world.emplace<ModelComponent>(deserializedEntity,
                    modelComponent["path"].as<std::string>(),
                    //modelComponent["layout"].as<std::vector<int>>(),
                    modelComponent["scale"].as<glm::vec3>(),
                    modelComponent["center"].as<glm::vec3>(),
                    modelComponent["uvscale"].as<glm::vec2>());
            }

            if (const auto& component = entity["RigidbodyComponent"]) {
                scene->world.emplace<RigidbodyComponent>(deserializedEntity,
                    magic_enum::enum_value<RigidbodyComponent::BodyType>(component["type"].as<int>()),
                    component["mass"].as<float>(),
                    component["linearDrag"].as<float>(),
                    component["angularDrag"].as<float>(),
                    component["disableGravity"].as<bool>(),
                    component["kinematic"].as<bool>(),
                    component["freezePosition"].as<glm::bvec3>(),
                    component["freezeRotation"].as<glm::bvec3>());
            }

            if (const auto& component = entity["BoxColliderComponent"]) {
                scene->world.emplace<BoxColliderComponent>(deserializedEntity,
                    component["center"].as<glm::vec3>(),
                    component["size"].as<glm::vec3>(),
                    component["trigger"].as<bool>());
            }

            if (const auto& component = entity["PhysicsMaterialComponent"]) {
                scene->world.emplace<PhysicsMaterialComponent>(deserializedEntity,
                    component["friction"].as<float>(),
                    component["restitution"].as<float>());
            }
        }
    }

    return true;
}
