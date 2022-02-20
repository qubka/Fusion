#pragma once

#ifdef FE_SHARED_LIB
#if defined(_MSC_VER)
    #ifdef FS_ADD_EXPORTS
        #define FUSION_API __declspec(dllexport)
    #else
        #define FUSION_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__)
    #ifdef FS_ADD_EXPORTS
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

#ifdef FE_DEBUG
#define FE_ASSERT(...) assert(__VA_ARGS__)
#else
#define FE_ASSERT(...)
#endif

#ifdef FE_DEBUG
#define FE_ASSERT(...) assert(__VA_ARGS__)
#else
#define FE_ASSERT(...)
#endif

#define FE_NONCOPYABLE(x) x(const x&) = delete; \
                          x(x&&) = delete; \
                          x& operator=(const x&) = delete; \
                          x& operator=(x&&) = delete; \
