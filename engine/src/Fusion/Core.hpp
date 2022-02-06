#pragma once

#if defined(_MSC_VER)
    #ifdef ADD_EXPORTS
        #define FUSION_API __declspec(dllexport)
    #else
        #define FUSION_API __declspec(dllimport)
    #endif
    #define DEBUG_BREAK __debugbreak()
#elif defined(__GNUC__)
    #ifdef FS_ADD_EXPORTS
        #define FUSION_API __attribute__((visibility("default")))
    #else
        #define FUSION_API
    #endif
    #define DEBUG_BREAK __builtin_trap()
#else
    #define FUSION_API
    #define DEBUG_BREAK
    #pragma warning Unknown dynamic link import/export semantics.
#endif

#ifdef FS_ENABLE_ASSERTS
#define FS_CORE_ASSERT(x, ...) { if(!x) { FS_LOG_CORE_FATAL("Assertion Failed: {0}", __VA_ARGS__); DEBUG_BREAK; }}
#define FS_ASSERT(x, ...) { if(!x) { FS_LOG_CORE_FATAL("Assertion Failed: {0}", __VA_ARGS__); DEBUG_BREAK; }}
#else
#define FS_CORE_ASSERT(x, ...)
#define FS_ASSERT(x, ...)
#endif
