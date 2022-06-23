#include "common.hpp"

const std::string& fe::getAssetPath() {
#if PLATFORM_ANDROID
    static const std::string NOTHING;
    return NOTHING;
#else
    static std::string path;
    static std::once_flag once;
    std::call_once(once, [] {
        path = "assets";
    });
    return path;
#endif
}
