#pragma once

#ifdef FUSION_SHARED_LIB
#if defined(_MSC_VER)
    #ifdef FUSION_EXPORTS
        #define FUSION_API __declspec(dllexport)
    #else
        #define FUSION_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__)
    #ifdef FUSION_EXPORTS
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

#define NONCOPYABLE(x)  x(const x&) = delete; \
                        x(x&&) = delete; \
                        x& operator=(const x&) = delete; \
                        x& operator=(x&&) = delete; \
