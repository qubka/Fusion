//
// Input Mapping Demo
// http://scribblings-by-apoch.googlecode.com/
//
// Wrapper class for mapping inputs
//

#include "input_mapper.hpp"
#include "input_context.hpp"
#include "file.hpp"

using namespace fe;

/**
 * Construct and initialize an input mapper
 */
InputMapper::InputMapper(const std::filesystem::path& filename) {
    assert(std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename));

    std::wifstream infile{filename.c_str()};
    if (!infile.is_open()) {
        throw std::runtime_error("File " + filename.string() + " not found");
    }

    unsigned count;
    if (!(infile >> count))
        throw std::runtime_error("Failed to read " + filename.string());
    for (unsigned i = 0; i < count; ++i) {
        auto name = attemptRead<std::wstring>(infile);
        auto file = attemptRead<std::wstring>(infile);
        inputContexts[name] = std::make_unique<InputContext>(file);
    }
}

/**
 * Clear all mapped input
 */
void InputMapper::clear() {
    currentMappedInput.actions.clear();
    currentMappedInput.ranges.clear();
    // Note: we do NOT clear states, because they need to remain set
    // across frames so that they don't accidentally show "off" for
    // a tick or two while the raw input is still pending.
}

/**
 * Set the state of a raw button
 */
void InputMapper::setRawButtonState(RawInputButton button, bool pressed, bool previouslypressed) {

    if (pressed && !previouslypressed) {
        Action action;
        if (mapButtonToAction(button, action)) {
            currentMappedInput.actions.insert(action);
            return;
        }
    }

    if (pressed) {
        State state;
        if (mapButtonToState(button, state)) {
            currentMappedInput.states.insert(state);
            return;
        }
    }

    mapAndEatButton(button);
}

/**
 * Set the raw axis value of a given axis
 */
void InputMapper::setRawAxisValue(RawInputAxis axis, double value) {
    for (const auto context : activeContexts) {
        Range range;
        if (context->mapAxisToRange(axis, range)) {
            currentMappedInput.ranges[range] = context->getConversions().convert(range, value * context->getSensitivity(range));
            break;
        }
    }
}

/**
 * Dispatch input to all registered callbacks
 */
void InputMapper::dispatch() const {
    MappedInput input = currentMappedInput;
    for (auto it : callbackTable)
        (*it.second)(input);
}

/**
 * Add a callback to the dispatch table
 */
void InputMapper::addCallback(InputCallback callback, int priority) {
    callbackTable.insert(std::make_pair(priority, callback));
}

/**
 * Push an active input context onto the stack
 */
void InputMapper::pushContext(const std::wstring& name) {
    auto it = inputContexts.find(name);
    if (it == inputContexts.end())
        throw std::runtime_error("Invalid input context pushed");

    activeContexts.push_front(it->second.get());
}

/**
 * Pop the current input context off the stack
 */
void InputMapper::popContext() {
    if (activeContexts.empty())
        throw std::runtime_error("Cannot pop input context, no contexts active!");

    activeContexts.pop_front();
}

/**
 * Helper: map a button to an action in the active context(s)
 */
bool InputMapper::mapButtonToAction(RawInputButton button, Action& action) const {
    for (const auto context : activeContexts) {
        if (context->mapButtonToAction(button, action))
            return true;
    }

    return false;
}

/**
 * Helper: map a button to a state in the active context(s)
 */
bool InputMapper::mapButtonToState(RawInputButton button, State& state) const {
    for (const auto context : activeContexts) {
        if (context->mapButtonToState(button, state))
            return true;
    }

    return false;
}

/**
 * Helper: eat all input mapped to a given button
 */
void InputMapper::mapAndEatButton(RawInputButton button) {
    Action action;
    if (mapButtonToAction(button, action))
        currentMappedInput.eatAction(action);

    State state;
    if (mapButtonToState(button, state))
        currentMappedInput.eatState(state);
}