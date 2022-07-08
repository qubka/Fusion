#pragma once

#if FUSION_PROFILE
#ifdef FUSION_PLATFORM_WINDOWS
#define TRACY_CALLSTACK 1
#endif
#include <tracy/Tracy.hpp>
#define FUSION_PROFILE_SCOPE(name) ZoneScopedN(name)
#define FUSION_PROFILE_FUNCTION() ZoneScoped
#define FUSION_PROFILE_FRAMEMARKER() FrameMark
#define FUSION_PROFILE_LOCK(type, var, name) TracyLockableN(type, var, name)
#define FUSION_PROFILE_LOCKMARKER(var) LockMark(var)
#define FUSION_PROFILE_SETTHREADNAME(name) tracy::SetThreadName(name)
#else
#define FUSION_PROFILE_SCOPE(name)
#define FUSION_PROFILE_FUNCTION()
#define FUSION_PROFILE_FRAMEMARKER()
#define FUSION_PROFILE_LOCK(type, var, name) type var
#define FUSION_PROFILE_LOCKMARKER(var)
#define FUSION_PROFILE_SETTHREADNAME(name)
#endif