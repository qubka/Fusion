#pragma once

#include "base_input.hpp"
#include "pad_codes.hpp"

namespace fe {
    class JoystickInput : public BaseInput<PadCode> {
    public:
        JoystickInput(const std::initializer_list<PadCode>& keysToMonitor)
                : BaseInput<PadCode>{keysToMonitor} {};
        ~JoystickInput() = default;
    private:

    };
}
