#pragma once

#include "../Core.hpp"

namespace Fusion {
    enum class EventType {
        None = 0,

        WindowClose,
        WindowResize,
        WindowFocus,
        WindowLostFocus,
        WindowMoved,

        KeyPressed,
        KeyReleased,

        MouseMoved,
        MouseScrolled,
        MouseButtonPressed,
        MouseButtonReleased
    };

    enum EventCategory {
        EventCategoryNone = 0,

        EventCategoryApplication = (1 << 0),
        EventCategoryInput       = (1 << 1),
        EventCategoryKeyboard    = (1 << 2),
        EventCategoryMouse       = (1 << 3),
        EventCategoryMouseButton = (1 << 4)
    };

#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::type; } \
                               EventType getType() const override { return getStaticType(); } \
                               const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) int getCategoryFlags() const override { return category; }

    class FUSION_API Event {
        friend class EventDispatcher;
    public:
        virtual EventType getType() const = 0;
        virtual const char* getName() const = 0;
        virtual int getCategoryFlags() const = 0;
        virtual std::string toString() const { return getName(); }

        bool isInCategory(EventCategory category) const { return getCategoryFlags() & category; }
        bool isHandled() const { return handled; }

    protected:
        bool handled = false;
    };

    class FUSION_API EventDispatcher {
    public:
        template<typename T>
        using EventFn = std::function<bool(T&)>;
    public:
        EventDispatcher(Event& event) : event{event} {}

        template<typename T>
        bool dispatch(EventFn<T> func) {
            if (event.getType() == T::getStaticType()) {
                event.handled = func(*(T*)&event);
                return true;
            }
            return false;
        }

    private:
        Event& event;
    };
}