#include "plog/Initializers/RollingFileInitializer.h"

namespace Fusion {
    void Log::Init() {
        plog::init(plog::debug, "engine-log.txt");
        plog::init<GameLog>(plog::debug, "game-log.txt");
    }
}