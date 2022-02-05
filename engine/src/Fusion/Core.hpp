#pragma once

#if defined(_MSC_VER)
#ifdef ADD_EXPORTS
        #define FUSION_API __declspec(dllexport)
    #else
        #define FUSION_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__)
#ifdef ADD_EXPORTS
#define FUSION_API __attribute__((visibility("default")))
#else
#define FUSION_API
#endif
#else
#define FUSION_API
    #define FUSION_API
    #pragma warning Unknown dynamic link import/export semantics.
#endif
