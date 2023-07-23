#pragma once

namespace fe {
    /**
     * http://reedbeta.com/blog/python-like-enumerate-in-cpp17/
     */
    template<typename T,
            typename TIter = decltype(std::begin(std::declval<T>())),
            typename = decltype(std::end(std::declval<T>()))>
    constexpr auto enumerate(T&& iterable) {
        struct iterator {
            size_t i;
            TIter it;
            bool operator!=(const iterator& rhs) const { return it != rhs.it; }
            void operator++() { ++i; ++it; }
            auto operator*() const { return std::tie(i, *it); }
        };
        struct iterable_wrapper {
            T iterable;
            auto begin() { return iterator{0, std::begin(iterable)}; }
            auto end() { return iterator{0, std::end(iterable)}; }
        };
        return iterable_wrapper{std::forward<T>(iterable)};
    }
}