#include "plog/Initializers/RollingFileInitializer.h"

namespace Fusion {
    void Log::Init() {
        const char* file = "fusion.log";
        std::ofstream ofs;
        ofs.open(file, std::ofstream::out | std::ofstream::trunc);
        ofs.close();
        plog::init(plog::debug, file);
    }
}