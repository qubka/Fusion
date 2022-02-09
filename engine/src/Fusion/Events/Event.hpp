#pragma once

#include "Fusion/Core/Base.hpp"

namespace Fusion {

    class FUSION_API Event {
    };

    class FUSION_API HandlerFunctionBase {
    public:
        void exec(Event* event) {
            call(event);
        }
    private:
        virtual void call(Event* event) = 0;
    };
    
    template<class T, class EventType>
    class FUSION_API MemberFunctionHandler : public HandlerFunctionBase {
    public:
        typedef void (T::*MemberFunction)(const EventType&);

        MemberFunctionHandler(T * instance, MemberFunction memberFunction)
            : instance{instance}, memberFunction{memberFunction} {};

        void call(Event* event) override {
            (instance->*memberFunction)(static_cast<const EventType&>(*event));
        }
    private:
        T* instance;
        MemberFunction memberFunction;
    };
    
    typedef std::vector<HandlerFunctionBase*> HandlerList; // std::list
    class FUSION_API EventBus {
    public:
        template<typename EventType>
        void publish(EventType* event) {
            auto* handlers = subscribers[typeid(EventType)];

            if (handlers == nullptr) {
                return;
            }

            for (auto& handler : *handlers) {
                if (handler != nullptr) {
                    handler->exec(event);
                }
            }
        }

        template<class T, class EventType>
        void subscribe(T* instance, void (T::*memberFunction)(const EventType&)) {
            auto* handlers = subscribers[typeid(EventType)];

            // First time initialization
            if (handlers == nullptr) {
                handlers = new HandlerList();
                subscribers[typeid(EventType)] = handlers;
            }

            handlers->emplace_back(instance, memberFunction);
        }
    private:
        std::map<std::type_index, HandlerList*> subscribers;
    };
}
