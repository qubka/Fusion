#include "scene_serializer.hpp"

#include "components.hpp"
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
            if (!node.IsSequence() || node.size() != 3)
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
}

using namespace fe;

SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene) : scene{scene} {

}

void serializeEntity(YAML::Emitter& out, entt::registry& registry, entt::entity entity) {
    out << YAML::BeginMap; // Entity
    if (auto component = registry.try_get<IdComponent>(entity)) {
        out << YAML::Key << "Entity" << YAML::Value << component->id;
    }

    if (auto component = registry.try_get<TagComponent>(entity)) {
        out << YAML::Key << "TagComponent";
        out << YAML::BeginMap; // TagComponent
        out << YAML::Key << "tag" << YAML::Value << component->tag;
        out << YAML::EndMap; // TagComponent
    }

    if (auto component = registry.try_get<TransformComponent>(entity)) {
        out << YAML::Key << "TransformComponent";
        out << YAML::BeginMap; // TransformComponent

        out << YAML::Key << "translation" << YAML::Value << component->translation;
        out << YAML::Key << "rotation" << YAML::Value << component->rotation;
        out << YAML::Key << "scale" << YAML::Value << component->scale;

        out << YAML::EndMap; // TransformComponent
    }

    if (auto component = registry.try_get<CameraComponent>(entity)) {
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

    if (auto component = registry.try_get<ModelComponent>(entity)) {
        out << YAML::Key << "ModelComponent";
        out << YAML::BeginMap; // ModelComponent

        out << YAML::Key << "path" << YAML::Value << component->path;
        //out << YAML::Key << "layout" << YAML::Value << component->layout;
        out << YAML::Key << "scale" << YAML::Value << component->scale;
        out << YAML::Key << "center" << YAML::Value << component->center;
        out << YAML::Key << "uvscale" << YAML::Value << component->uvscale;

        out << YAML::EndMap; // ModelComponent
    }

    out << YAML::EndMap; // Entity
}

void SceneSerializer::serialize(const std::string& filepath) {
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "Untitled";
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
    scene->registry.each([&](auto entity) {
        serializeEntity(out, scene->registry, entity);
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
            auto uuid = entity["Entity"].as<uint64_t>();
            auto deserializedEntity = scene->registry.create();
            scene->registry.emplace<IdComponent>(deserializedEntity, uuid);

            std::string name;
            if (const auto& tagComponent = entity["TagComponent"]) {
                name = tagComponent["tag"].as<std::string>();
                scene->registry.emplace<TagComponent>(deserializedEntity, name);
            }

            LOG_DEBUG << "Deserialized entity with ID = " << uuid << ", name = " << name;

            if (const auto& transformComponent = entity["TransformComponent"]) {
                scene->registry.emplace<TransformComponent>(deserializedEntity,
                       transformComponent["translation"].as<glm::vec3>(),
                       transformComponent["rotation"].as<glm::vec3>(),
                       transformComponent["scale"].as<glm::vec3>());
            }

            if (const auto& cameraComponent = entity["CameraComponent"]) {
                const auto& cameraProps = cameraComponent["Camera"];

                SceneCamera camera;
                camera.setProjectionType(magic_enum::enum_value<SceneCamera::ProjectionType>(cameraProps["projectionType"].as<int>()));
                camera.setPerspectiveVerticalFOV(cameraProps["perspectiveFOV"].as<float>());
                camera.setPerspectiveNearClip(cameraProps["perspectiveNear"].as<float>());
                camera.setPerspectiveFarClip(cameraProps["perspectiveFar"].as<float>());
                camera.setOrthographicSize(cameraProps["orthographicSize"].as<float>());
                camera.setOrthographicNearClip(cameraProps["orthographicNear"].as<float>());
                camera.setOrthographicFarClip(cameraProps["orthographicFar"].as<float>());

                scene->registry.emplace<CameraComponent>(deserializedEntity,
                        camera,
                        cameraComponent["primary"].as<bool>(),
                        cameraComponent["fixedAspectRatio"].as<bool>()
                );
            }

            if (const auto& modelComponent = entity["ModelComponent"]) {
                scene->registry.emplace<ModelComponent>(deserializedEntity,
                    modelComponent["path"].as<std::string>(),
                    //modelComponent["layout"].as<std::vector<int>>(),
                    modelComponent["scale"].as<glm::vec3>(),
                    modelComponent["center"].as<glm::vec3>(),
                    modelComponent["uvscale"].as<glm::vec2>()
                );
            }

        }
    }

    return true;
}
