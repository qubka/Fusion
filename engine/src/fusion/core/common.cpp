#include "common.hpp"

const std::string& fe::getAssetPath() {
#if defined(__ANDROID__)
    static const std::string NOTHING;
    return NOTHING;
#else
    static std::string path;
    static std::once_flag once;
    std::call_once(once, [] {
        path = "data/";
    });
    return path;
#endif
}
