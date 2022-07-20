#pragma once

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

/// https://github.com/steinwurf/platform

// Here we create a number of defines to make it easy to choose between
// different compilers, operatings systems and CPU architectures.
// Some information about the defines used can be found here:
// http://sourceforge.net/p/predef/wiki/Architectures/

// Detect operating systems
#if defined(__linux__)
#define FUSION_PLATFORM_LINUX 1
#if defined(__ANDROID__)
#define FUSION_PLATFORM_ANDROID 1
#endif
#elif defined(_WIN32)
#define FUSION_PLATFORM_WINDOWS 1
#if defined(WINAPI_FAMILY)
#include <winapifamily.h>
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PHONE_APP)
#define FUSION_PLATFORM_WINDOWS_PHONE 1
#endif
#endif
#elif defined(__APPLE__)
#define FUSION_PLATFORM_APPLE 1
// Detect iOS before MacOSX (__MACH__ is also defined for iOS)
#if defined(IPHONE)
#define FUSION_PLATFORM_IOS 1
#elif defined(__MACH__)
#define FUSION_PLATFORM_MAC 1
#endif
#elif defined(__EMSCRIPTEN__)
#define FUSION_PLATFORM_EMSCRIPTEN 1
#else
#error "Unable to determine operating system"
#endif

// Detect compilers and CPU architectures
// Note: clang also defines __GNUC__ since it aims to be compatible with GCC.
// Therefore we need to check for __clang__ or __llvm__ first.
#if defined(__clang__) || defined(__llvm__)
#define FUSION_PLATFORM_CLANG 1
#define FUSION_PLATFORM_GCC_COMPATIBLE 1
#if defined(__i386__) || defined(__x86_64__)
#define FUSION_PLATFORM_X86 1
#define FUSION_PLATFORM_CLANG_X86 1
#define FUSION_PLATFORM_GCC_COMPATIBLE_X86 1
#elif defined(__arm__) || defined(__arm64__) || defined(__aarch64__)
#define FUSION_PLATFORM_ARM 1
#define FUSION_PLATFORM_CLANG_ARM 1
#define FUSION_PLATFORM_GCC_COMPATIBLE_ARM 1
#elif defined(__mips__)
#define FUSION_PLATFORM_MIPS 1
#define FUSION_PLATFORM_CLANG_MIPS 1
#define FUSION_PLATFORM_GCC_COMPATIBLE_MIPS 1
#elif defined(__asmjs__)
#define FUSION_PLATFORM_ASMJS 1
#define FUSION_PLATFORM_CLANG_ASMJS 1
#define FUSION_PLATFORM_GCC_COMPATIBLE_ASMJS 1
#endif
#elif defined(__GNUC__)
#define FUSION_PLATFORM_GCC 1
#define FUSION_PLATFORM_GCC_COMPATIBLE 1
#if defined(__i386__) || defined(__x86_64__)
#define FUSION_PLATFORM_X86 1
#define FUSION_PLATFORM_GCC_X86 1
#define FUSION_PLATFORM_GCC_COMPATIBLE_X86 1
#elif defined(__arm__) || defined(__arm64__) || defined(__aarch64__)
#define FUSION_PLATFORM_ARM 1
#define FUSION_PLATFORM_GCC_ARM 1
#define FUSION_PLATFORM_GCC_COMPATIBLE_ARM 1
#elif defined(__mips__)
#define FUSION_PLATFORM_MIPS 1
#define FUSION_PLATFORM_GCC_MIPS 1
#define FUSION_PLATFORM_GCC_COMPATIBLE_MIPS 1
#endif
#elif defined(_MSC_VER)
#define FUSION_PLATFORM_MSVC 1
#if defined(_M_IX86) || defined(_M_X64)
#define FUSION_PLATFORM_X86 1
#define FUSION_PLATFORM_MSVC_X86 1
#elif defined(_M_ARM) || defined(_M_ARMT)
#define FUSION_PLATFORM_ARM 1
#define FUSION_PLATFORM_MSVC_ARM 1
#endif
#else
#error "Unable to determine compiler"
#endif

// Define macros for supported CPU instruction sets
#if defined(FUSION_PLATFORM_GCC_COMPATIBLE)
#if defined(__MMX__)
#define FUSION_PLATFORM_MMX 1
#endif
#if defined(__SSE__)
#define FUSION_PLATFORM_SSE 1
#endif
#if defined(__SSE2__)
#define FUSION_PLATFORM_SSE2 1
#endif
#if defined(__SSE3__)
#define FUSION_PLATFORM_SSE3 1
#endif
#if defined(__SSSE3__)
#define FUSION_PLATFORM_SSSE3 1
#endif
#if defined(__SSE4_1__)
#define FUSION_PLATFORM_SSE41 1
#endif
#if defined(__SSE4_2__)
#define FUSION_PLATFORM_SSE42 1
#endif
#if defined(__PCLMUL__)
#define FUSION_PLATFORM_PCLMUL 1
#endif
#if defined(__AVX__)
#define FUSION_PLATFORM_AVX 1
#endif
#if defined(__AVX2__)
#define FUSION_PLATFORM_AVX2 1
#endif
#if defined(__ARM_NEON__) || defined(__ARM_NEON)
#define FUSION_PLATFORM_NEON 1
#endif
// First, check the PLATFORM_WINDOWS_PHONE define, because
// the X86 instructions sets are not supported on the Windows Phone emulator
#elif defined(FUSION_PLATFORM_WINDOWS_PHONE)
#if defined(FUSION_PLATFORM_MSVC_ARM)
// NEON introduced in VS2012
#if (_MSC_VER >= 1700)
#define FUSION_PLATFORM_NEON 1
#endif
#endif
#elif defined(FUSION_PLATFORM_MSVC_X86)
// MMX, SSE and SSE2 introduced in VS2003
#if (_MSC_VER >= 1310)
#define FUSION_PLATFORM_MMX 1
#define FUSION_PLATFORM_SSE 1
#define FUSION_PLATFORM_SSE2 1
#endif
// SSE3 introduced in VS2005
#if (_MSC_VER >= 1400)
#define FUSION_PLATFORM_SSE3 1
#endif
// SSSE3, SSE4.1, SSE4.2, PCLMUL introduced in VS2008
#if (_MSC_VER >= 1500)
#define FUSION_PLATFORM_SSSE3 1
#define FUSION_PLATFORM_SSE41 1
#define FUSION_PLATFORM_SSE42 1
#define FUSION_PLATFORM_PCLMUL 1
#endif
// AVX and AVX2 introduced in VS2012
#if (_MSC_VER >= 1700)
#define FUSION_PLATFORM_AVX 1
#define FUSION_PLATFORM_AVX2 1
#endif
#endif