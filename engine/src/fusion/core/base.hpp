#pragma once

#include "platform.hpp"

#if FUSION_SHARED_LIB
#if defined(_MSC_VER)
    #if FUSION_EXPORTS
        #define FUSION_API __declspec(dllexport)
    #else
        #define FUSION_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__)
    #if FUSION_EXPORTS
        #define FUSION_API __attribute__((visibility("default")))
    #else
        #define FUSION_API
    #endif
#else
    #error "Unknown dynamic link import/export semantics."
#endif
#else
#define FUSION_API
#endif

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

#define MEM_ALIGNMENT 16
#ifdef FUSION_PLATFORM_WINDOWS
#define MEM_ALIGN __declspec(align(MEM_ALIGNMENT))
#else
#define MEM_ALIGN __attribute__((aligned(MEM_ALIGNMENT)))
#endif

// TODO: Move from here ?

#include <uuid.h>

namespace fe {
    inline static uuids::uuid_random_generator uuid_random_generator{ Random::engine() };
}

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
}