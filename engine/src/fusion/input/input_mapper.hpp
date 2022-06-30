//
// Input Mapping Demo
// http://scribblings-by-apoch.googlecode.com/
//
// Wrapper class for mapping inputs
//

#pragma once

#include "raw_input_constants.hpp"
#include "input_constants.hpp"

namespace fe {
    // Forward declarations
    class InputContext;

    // Helper structure
    struct MappedInput {
        std::set<Action> actions;
        std::set<State> states;
        std::map<Range, double> ranges;

        // Consumption helpers
        void eatAction(Action action) { actions.erase(action); }
        void eatState(State state)    { states.erase(state); }
        void eatRange(Range range) {
            if (auto it = ranges.find(range); it != ranges.end())
                ranges.erase(it);
        }
    };

    // Handy type shortcuts
    typedef void (*InputCallback)(MappedInput& inputs);

    class InputMapper {
        // Construction and destruction
    public:
        explicit InputMapper(const std::filesystem::path& filename);
        ~InputMapper() = default;

        // Raw input interface
    public:
        void clear();
        void setRawButtonState(RawInputButton button, bool pressed, bool previouslypressed);
        void setRawAxisValue(RawInputAxis axis, double value);

        // Input dispatching interface
    public:
        void dispatch() const;

        // Input callback registration interface
    public:
        void addCallback(InputCallback callback, int32_t priority);

        // Context management interface
    public:
        void pushContext(const std::wstring& name);
        void popContext();

        // Internal helpers
    private:
        bool mapButtonToAction(RawInputButton button, Action& action) const;
        bool mapButtonToState(RawInputButton button, State& state) const;
        void mapAndEatButton(RawInputButton button);

        // Internal tracking
    private:
        std::unordered_map<std::wstring, std::unique_ptr<InputContext>> inputContexts;
        std::list<InputContext*> activeContexts; // just used to monitor active contexts

        std::multimap<int32_t, InputCallback> callbackTable;

        MappedInput currentMappedInput;
    };
}