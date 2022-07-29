#pragma once

#include <cereal/cereal.hpp>

namespace std::filesystem {
    template<class Archive> string save_minimal(const Archive&, const path& p) { return p.string(); }
    template<class Archive> void load_minimal(const Archive&, path& p, const string& in) { p = in; };
}
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(std::filesystem::path, cereal::specialization::non_member_load_save_minimal);

namespace cereal {
    template<class Archive> void serialize(Archive& archive, glm::vec2& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y)); }
    template<class Archive> void serialize(Archive& archive, glm::vec3& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y), make_nvp("z", v.z)); }
    template<class Archive> void serialize(Archive& archive, glm::vec4& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y), make_nvp("z", v.z), make_nvp("w", v.w)); }
    template<class Archive> void serialize(Archive& archive, glm::ivec2& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y)); }
    template<class Archive> void serialize(Archive& archive, glm::ivec3& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y), make_nvp("z", v.z)); }
    template<class Archive> void serialize(Archive& archive, glm::ivec4& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y), make_nvp("z", v.z), make_nvp("w", v.w)); }
    template<class Archive> void serialize(Archive& archive, glm::uvec2& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y)); }
    template<class Archive> void serialize(Archive& archive, glm::uvec3& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y), make_nvp("z", v.z)); }
    template<class Archive> void serialize(Archive& archive, glm::uvec4& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y), make_nvp("z", v.z), make_nvp("w", v.w)); }
    template<class Archive> void serialize(Archive& archive, glm::dvec2& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y)); }
    template<class Archive> void serialize(Archive& archive, glm::dvec3& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y), make_nvp("z", v.z)); }
    template<class Archive> void serialize(Archive& archive, glm::dvec4& v) { archive(make_nvp("x", v.x), make_nvp("y", v.y), make_nvp("z", v.z), make_nvp("w", v.w)); }

    // glm matrices serialization
    template<class Archive> void serialize(Archive& archive, glm::mat2& m) { archive(make_nvp("c0", m[0]), make_nvp("c1", m[1])); }
    template<class Archive> void serialize(Archive& archive, glm::dmat2& m) { archive(make_nvp("c0", m[0]), make_nvp("c1", m[1])); }
    template<class Archive> void serialize(Archive& archive, glm::mat2x3& m) { archive(make_nvp("c0", m[0]), make_nvp("c1", m[1])); }
    template<class Archive> void serialize(Archive& archive, glm::dmat2x3& m) { archive(make_nvp("c0", m[0]), make_nvp("c1", m[1])); }
    template<class Archive> void serialize(Archive& archive, glm::mat2x4& m) { archive(make_nvp("c0", m[0]), make_nvp("c1", m[1])); }
    template<class Archive> void serialize(Archive& archive, glm::dmat2x4& m) { archive(make_nvp("c0", m[0]), make_nvp("c1", m[1])); }
    template<class Archive> void serialize(Archive& archive, glm::mat3x2& m) { archive(make_nvp("c0", m[0]), make_nvp("c1", m[1]), make_nvp("c2", m[2])); }
    template<class Archive> void serialize(Archive& archive, glm::dmat3x2& m) { archive(make_nvp("c0", m[0]), make_nvp("c1", m[1]), make_nvp("c2", m[2])); }
    template<class Archive> void serialize(Archive& archive, glm::mat3& m) { archive(make_nvp("c0", m[0]), make_nvp("c1", m[1]), make_nvp("c2", m[2])); }
    template<class Archive> void serialize(Archive& archive, glm::dmat3& m) { archive(make_nvp("c0", m[0]), make_nvp("c1", m[1]), make_nvp("c2", m[2])); }
    template<class Archive> void serialize(Archive& archive, glm::mat4& m) { archive(make_nvp("c0", m[0]), make_nvp("c1", m[1]), make_nvp("c2", m[2]), make_nvp("c3", m[3])); }
    template<class Archive> void serialize(Archive& archive, glm::dmat4& m) { archive(make_nvp("c0", m[0]), make_nvp("c1", m[1]), make_nvp("c2", m[2]), make_nvp("c3", m[3])); }
    template<class Archive> void serialize(Archive& archive, glm::mat4x2& m) { archive(make_nvp("c0", m[0]), make_nvp("c1", m[1]), make_nvp("c2", m[2]), make_nvp("c3", m[3])); }
    template<class Archive> void serialize(Archive& archive, glm::dmat4x2& m) { archive(make_nvp("c0", m[0]), make_nvp("c1", m[1]), make_nvp("c2", m[2]), make_nvp("c3", m[3])); }

    template<class Archive> void serialize(Archive& archive, glm::quat& q) { archive(make_nvp("x", q.x), make_nvp("y", q.y), make_nvp("z", q.z), make_nvp("w", q.w)); }
    template<class Archive> void serialize(Archive& archive, glm::dquat& q) { archive(make_nvp("x", q.x), make_nvp("y", q.y), make_nvp("z", q.z), make_nvp("w", q.w)); }

    template<class Archive>
    std::string to_string(Archive& archive) {
        std::stringstream ss;
        cereal::JSONOutputArchive output{ss};
        output(archive);
        return ss.str();
    }

    template<typename T, typename... Args>
    std::string to_string(Args... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        std::stringstream ss;
        cereal::JSONOutputArchive output{ss, cereal::JSONOutputArchive::Options::NoIndent()};
        output(*ptr);
        return ss.str();
    }
}