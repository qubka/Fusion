#pragma once

#ifdef FUSION_DEBUG
    #if FUSION_PLATFORM_WINDOWS
		#define FE_DEBUGBREAK() __debugbreak()
	#else
		#include <csignal>
		#define FE_DEBUGBREAK() raise(SIGTRAP)
    #endif
	#define FE_ENABLE_ASSERTS
#else
#define FE_DEBUGBREAK()
#endif

#define FE_EXPAND_MACRO(x) x
#define FE_STRINGIFY_MACRO(x) #x

#ifdef FE_ENABLE_ASSERTS
    // Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define FE_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { FE##type##ERROR(msg, __VA_ARGS__); FE_DEBUGBREAK(); } }
	#define FE_INTERNAL_ASSERT_WITH_MSG(type, check, ...) FE_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define FE_INTERNAL_ASSERT_NO_MSG(type, check) FE_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", FE_STRINGIFY_MACRO(check), fs::path(__FILE__).filename(), __LINE__)

	#define FE_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define FE_INTERNAL_ASSERT_GET_MACRO(...) FE_EXPAND_MACRO( FE_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, FE_INTERNAL_ASSERT_WITH_MSG, FE_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define FE_ASSERT(...) FE_EXPAND_MACRO( FE_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_LOG_, __VA_ARGS__) )
#else
#define FE_ASSERT(...)
#endif