#include "plog/Initializers/RollingFileInitializer.h"

namespace Fusion {
    void Log::Init() {
        plog::init(plog::debug, "fusion.log");
    }
}