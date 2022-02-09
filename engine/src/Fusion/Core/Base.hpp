#pragma once

#ifdef FS_DYNAMIC_LINK
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

#ifdef FS_DEBUG
    #if defined(_WIN32)
		#define FS_DEBUG_BREAK __debugbreak()
	#elif defined(linux)
		#include <csignal>
		#define FS_DEBUG_BREAK raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define FS_ENABLE_ASSERTS
#else
#define FS_DEBUG_BREAK
#endif

#ifdef FS_ENABLE_ASSERTS
#define FS_CORE_ASSERT(x, ...) if(!(x)) { FS_LOG_CORE_FATAL("Assertion Failed: {0}", __VA_ARGS__); FS_DEBUG_BREAK; }
#define FS_ASSERT(x, ...) if(!x) { FS_LOG_CORE_FATAL("Assertion Failed: {0}", __VA_ARGS__); FS_DEBUG_BREAK; }
#else
#define FS_CORE_ASSERT(x, ...)
#define FS_ASSERT(x, ...)
#endif
