#pragma once

template <glm::length_t L, typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<L, T, Q>>: formatter<std::string_view> {
    template<typename FormatContext>
    auto format(const glm::vec<L, T, Q>& v, FormatContext& ctx) {
        return formatter<std::string_view>::format(glm::to_string(v), ctx);
    }
};

template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
struct fmt::formatter<glm::mat<C, R, T, Q>>: formatter<std::string_view> {
    template<typename FormatContext>
    auto format(const glm::mat<C, R, T, Q>& m, FormatContext& ctx) {
        return formatter<std::string_view>::format(glm::to_string(m), ctx);
    }
};

template <typename T, glm::qualifier Q>
struct fmt::formatter<glm::qua<T, Q>>: formatter<std::string_view> {
    template<typename FormatContext>
    auto format(const glm::qua<T, Q>& q, FormatContext& ctx) {
        return formatter<std::string_view>::format(glm::to_string(q), ctx);
    }
};

template <>
struct fmt::formatter<fs::path>: formatter<std::string_view> {
    template<typename FormatContext>
    auto format(const fs::path& path, FormatContext& ctx) {
        return formatter<std::string_view>::format(path.string(), ctx);
    }
};


template <>
struct fmt::formatter<uuids::uuid>: formatter<std::string_view> {
    template<typename FormatContext>
    auto format(uuids::uuid uuid, FormatContext& ctx) {
        return formatter<std::string_view>::format(uuids::to_string(uuid), ctx);
    }
};

