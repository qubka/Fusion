//
// Input Mapping Demo
// http://scribblings-by-apoch.googlecode.com/
//
// Wrapper class for managing input contexts
//

#pragma once

#include "raw_input_constants.hpp"
#include "input_constants.hpp"
#include "range_converter.hpp"

namespace fe {
    class InputContext {
        // Construction and destruction
    public:
        explicit InputContext(const std::filesystem::path& filename);
        ~InputContext() = default;

        // Mapping interface
    public:
        bool mapButtonToAction(RawInputButton button, Action& out) const;
        bool mapButtonToState(RawInputButton button, State& out) const;
        bool mapAxisToRange(RawInputAxis axis, Range& out) const;

        double getSensitivity(Range range) const;

        const RangeConverter& getConversions() const { return conversions; }

        // Internal tracking
    private:
        std::map<RawInputButton, Action> actionMap;
        std::map<RawInputButton, State> stateMap;
        std::map<RawInputAxis, Range> rangeMap;

        std::map<Range, double> sensitivityMap;
        RangeConverter conversions;
    };
}