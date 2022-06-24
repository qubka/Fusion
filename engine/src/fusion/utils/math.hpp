#pragma once

namespace fe {
    class Math {
    public:
        Math() = delete;

        /**
         * Combines a seed into a hash and modifies the seed by the new hash.
         * @param seed The seed.
         * @param v The value to hash.
         * https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
         */
        template<typename T>
        static void HashCombine(size_t& seed, const T& v) noexcept {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };
}