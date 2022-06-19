#include "context.hpp"

using namespace vkx;

#ifdef WIN32
__declspec(thread) vk::CommandPool Context::s_cmdPool;
#else
thread_local vk::CommandPool Context::s_cmdPool;
#endif