#include "SceneSerializer.hpp"

#include "Components.hpp"
#include "Scene.hpp"

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <magic_enum.hpp>

using namespace Fusion;

SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene) : scene{scene} {

}

namespace rapidjson {
    struct Key {
        std::string key;
    };

    enum CmdType {
        StartObject,
        EndObject,
        StartArray,
        EndArray
    };

    static inline Writer<StringBuffer>& operator<<(Writer<StringBuffer>& out, const CmdType& t) {
        switch (t) {
            case StartObject:
                out.StartObject();
                break;
            case EndObject:
                out.EndObject();
                break;
            case StartArray:
                out.StartArray();
                break;
            case EndArray:
                out.EndArray();
                break;
        }
        return out;
    }

    static inline Writer<StringBuffer>& operator<<(Writer<StringBuffer>& out, const Key& k) {
        out.Key(k.key.c_str());
        return out;
    }

    static inline Writer<StringBuffer>& operator<<(Writer<StringBuffer>& out, const glm::vec2& v){
        out.StartArray();
        out.Double(v.x);
        out.Double(v.y);
        out.EndArray();
        return out;
    }

    static inline Writer<StringBuffer>& operator<<(Writer<StringBuffer>& out, const glm::vec3& v) {
        out.StartArray();
        out.Double(v.x);
        out.Double(v.y);
        out.Double(v.z);
        out.EndArray();
        return out;
    }

    static inline Writer<StringBuffer>& operator<<(Writer<StringBuffer>& out, const glm::vec4& v) {
        out.StartArray();
        out.Double(v.x);
        out.Double(v.y);
        out.Double(v.z);
        out.Double(v.w);
        out.EndArray();
        return out;
    }

    static inline Writer<StringBuffer>& operator<<(Writer<StringBuffer>& out, const glm::quat& q) {
        out.StartArray();
        out.Double(q.w);
        out.Double(q.x);
        out.Double(q.y);
        out.Double(q.z);
        out.EndArray();
        return out;
    }

    static inline Writer<StringBuffer>& operator<<(Writer<StringBuffer>& out, const char* s) {
        out.String(s);
        return out;
    }

    static inline Writer<StringBuffer>& operator<<(Writer<StringBuffer>& out, const std::string& s) {
        out.String(s.c_str());
        return out;
    }

    static inline Writer<StringBuffer>& operator<<(Writer<StringBuffer>& out, int i) {
        out.Int(i);
        return out;
    }

    static inline Writer<StringBuffer>& operator<<(Writer<StringBuffer>& out, float f) {
        out.Double(static_cast<float>(f));
        return out;
    }

    static inline Writer<StringBuffer>& operator<<(Writer<StringBuffer>& out, bool b) {
        out.Bool(b);
        return out;
    }
}

void serializeEntity(rapidjson::Writer<rapidjson::StringBuffer>& out, entt::registry& registry, entt::entity entity)
{
    out << rapidjson::StartObject; // Entity
    out << rapidjson::Key{"Entity"} << "12837192831273"; // TODO: Entity ID goes here

    if (auto* component = registry.try_get<TagComponent>(entity)) {
        out << rapidjson::Key{"TagComponent"};
        out << rapidjson::StartObject; // TagComponent
        out << rapidjson::Key{"Tag"} << component->tag;
        out << rapidjson::EndObject; // TagComponent
    }

    if (auto* component = registry.try_get<TransformComponent>(entity)) {
        out << rapidjson::Key{"TransformComponent"};
        out << rapidjson::StartObject; // TransformComponent

        out << rapidjson::Key{"Translation"} << component->translation;
        out << rapidjson::Key{"Rotation"} << component->rotation;
        out << rapidjson::Key{"Scale"} << component->scale;

        out << rapidjson::EndObject; // TransformComponent
    }

    if (auto* component = registry.try_get<CameraComponent>(entity)) {
        out << rapidjson::Key{"CameraComponent"};
        out << rapidjson::StartObject; // CameraComponent

        out << rapidjson::Key{"Camera"};
        out << rapidjson::StartObject; // Camera
        auto& camera = component->camera;
        out << rapidjson::Key{"ProjectionType"} << magic_enum::enum_integer(camera.getProjectionType());
        out << rapidjson::Key{"PerspectiveFOV"} << camera.getPerspectiveVerticalFOV();
        out << rapidjson::Key{"PerspectiveNear"} << camera.getPerspectiveNearClip();
        out << rapidjson::Key{"PerspectiveFar"} << camera.getPerspectiveFarClip();
        out << rapidjson::Key{"OrthographicSize"} << camera.getOrthographicSize();
        out << rapidjson::Key{"OrthographicNear"} << camera.getOrthographicNearClip();
        out << rapidjson::Key{"OrthographicFar"} << camera.getOrthographicFarClip();
        out << rapidjson::EndObject; // Camera

        out << rapidjson::Key{"Primary"} << component->primary;
        out << rapidjson::Key{"FixedAspectRatio"} << component->fixedAspectRatio;

        out << rapidjson::EndObject; // CameraComponent
    }

    out << rapidjson::EndObject; // Entity
}

void SceneSerializer::serialize(const std::string& filepath)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter out{buffer};
    out.SetMaxDecimalPlaces(2);

    out << rapidjson::StartObject;
    out << rapidjson::Key{"Scene"} << "Untitled";
    out << rapidjson::Key{"Entities"};
    out << rapidjson::StartArray;
    scene->registry.each([&](auto entity) {
        serializeEntity(out, scene->registry, entity);
    });
    out << rapidjson::EndArray;
    out << rapidjson::EndObject;

    //std::ofstream fout(filepath);
    //fout << out.c_str();

    std::cout << printf("%s", buffer.GetString()) << std::endl;
}
