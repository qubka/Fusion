#pragma once

namespace Fusion {

    class Event {
    };

    class HandlerFunctionBase {
    public:
        void exec(Event* event) {
            call(event);
        }
    private:
        virtual void call(Event* event) = 0;
    };
    
    template<class T, class EventType>
    class MemberFunctionHandler : public HandlerFunctionBase {
    public:
        typedef void (T::*MemberFunction)(EventType*);

        MemberFunctionHandler(T * instance, MemberFunction memberFunction)
            : instance{ instance }, memberFunction{ memberFunction } {};

        void call(Event* event) {
            (instance->*memberFunction)(static_cast<EventType*>(event));
        }
    private:
        T* instance;
        MemberFunction memberFunction;
    };
    
    typedef std::list<HandlerFunctionBase*> HandlerList;
    class EventBus {
    public:
        template<typename EventType>
        void publish(EventType* event) {
            HandlerList * handlers = subscribers[typeid(EventType)];

            if (handlers == nullptr) {
                return;
            }

            for (auto & handler : *handlers) {
                if (handler != nullptr) {
                    handler->exec(event);
                }
            }
        }

        template<class T, class EventType>
        void subscribe(T* instance, void (T::*memberFunction)(EventType*)) {
            HandlerList* handlers = subscribers[typeid(EventType)];

            //First time initialization
            if (handlers == nullptr) {
                handlers = new HandlerList();
                subscribers[typeid(EventType)] = handlers;
            }

            handlers->push_back(new MemberFunctionHandler<T, EventType>(instance, memberFunction));
        }
    private:
        std::map<std::type_index, HandlerList*> subscribers;
    };
}
