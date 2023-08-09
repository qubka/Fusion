#pragma once

#include "platform.h"
#include "asserts.h"
#include "profiler.h"

#define NONCOPYABLE(x) x(const x&) = delete; \
                       x(x&&) = delete; \
                       x& operator=(const x&) = delete; \
                       x& operator=(x&&) = delete;

#define ITERATABLE(t, o) std::vector<t>::iterator begin() { return o.begin(); } \
                         std::vector<t>::iterator end() { return o.end(); } \
                         std::vector<t>::reverse_iterator rbegin() { return o.rbegin(); } \
                         std::vector<t>::reverse_iterator rend() { return o.rend(); } \
                         [[nodiscard]] std::vector<t>::const_iterator begin() const { return o.begin(); } \
                         [[nodiscard]] std::vector<t>::const_iterator end() const { return o.end(); } \
                         [[nodiscard]] std::vector<t>::const_reverse_iterator rbegin() const { return o.rbegin(); } \
                         [[nodiscard]] std::vector<t>::const_reverse_iterator rend() const { return o.rend(); } \


#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#if FUSION_PLATFORM_ANDROID
#include "platform/android/android_log.h"
#define FUSION_ASSET_PATH
#define FUSION_VIRTUAL_FS true
#elif FUSION_PLATFORM_LINUX || FUSION_PLATFORM_WINDOWS || FUSION_PLATFORM_APPLE
#include "platform/pc/pc_log.h"
#define FUSION_ASSET_PATH "assets/"
#define FUSION_VIRTUAL_FS false
#else
#pragma error("Unknown platform!");
#endif

namespace fe {
    using type_index = uint32_t;
    /// https://mikejsavage.co.uk/blog/cpp-tricks-type-id.html
    inline type_index type_id_seq = 0;
    template<typename T> inline const type_index type_id = type_id_seq++;
}

namespace fe {
    /**
     * Combines a seed into a hash and modifies the seed by the new hash.
     * @param seed The seed.
     * @param v The value to hash.
     * https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
     */
    inline void hash_combine(size_t& seed) { }

    template <typename T, typename... Rest>
    inline void hash_combine(size_t& seed, const T& v, Rest... rest) {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        hash_combine(seed, rest...);
    }

    /* Clang fix */
    struct PathHash {
        auto operator()(const fs::path& p) const noexcept {
            return std::filesystem::hash_value(p);
        }
    };
}