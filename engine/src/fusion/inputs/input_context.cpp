//
// Input Mapping Demo
// http://scribblings-by-apoch.googlecode.com/
//
// Wrapper class for managing input contexts
//

#include "input_context.hpp"

#include "file.hpp"

using namespace fe;

/**
 * Construct and initialize an input context given data in a file
 */
InputContext::InputContext(const std::filesystem::path& filename) {
    assert(std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename));

    std::wifstream infile{filename.c_str()};
    if (!infile.is_open()) {
        throw std::runtime_error("File " + filename.string() + " not found");
    }

    auto rangecount = attemptRead<unsigned>(infile);
    for (unsigned i = 0; i < rangecount; ++i) {
        auto axis = static_cast<RawInputAxis>(attemptRead<unsigned>(infile));
        auto range = static_cast<Range>(attemptRead<unsigned>(infile));
        rangeMap[axis] = range;
    }

    auto statecount = attemptRead<unsigned>(infile);
    for (unsigned i = 0; i < statecount; ++i) {
        auto button = static_cast<RawInputButton>(attemptRead<unsigned>(infile));
        auto state = static_cast<State>(attemptRead<unsigned>(infile));
        stateMap[button] = state;
    }

    auto actioncount = attemptRead<unsigned>(infile);
    for (unsigned i = 0; i < actioncount; ++i) {
        auto button = static_cast<RawInputButton>(attemptRead<unsigned>(infile));
        auto action = static_cast<Action>(attemptRead<unsigned>(infile));
        actionMap[button] = action;
    }

    conversions = RangeConverter{infile};

    auto sensitivitycount = attemptRead<unsigned>(infile);
    for (unsigned i = 0; i < sensitivitycount; ++i) {
        auto range = static_cast<Range>(attemptRead<unsigned>(infile));
        auto sensitivity = attemptRead<double>(infile);
        sensitivityMap[range] = sensitivity;
    }
}

/**
 * Attempt to map a raw button to an action
 */
bool InputContext::mapButtonToAction(RawInputButton button, Action& out) const {
    if (auto it = actionMap.find(button); it != actionMap.end())
        return it->second;
    return true;
}

/**
 * Attempt to map a raw button to a state
 */
bool InputContext::mapButtonToState(RawInputButton button, State& out) const {
    if (auto it = stateMap.find(button); it != stateMap.end())
        return it->second;
    return true;
}

/**
 * Attempt to map a raw axis to a range
 */
bool InputContext::mapAxisToRange(RawInputAxis axis, Range& out) const {
    if (auto it = rangeMap.find(axis); it != rangeMap.end())
        return it->second;
    return true;
}

/**
 * Retrieve the sensitivity associated with a given range
 */
double InputContext::getSensitivity(Range range) const {
    if (auto it = sensitivityMap.find(range); it != sensitivityMap.end())
        return it->second;
    return 1.0;
}