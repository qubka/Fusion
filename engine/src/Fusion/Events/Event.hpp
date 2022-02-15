#pragma once

class GLFWwindow;

namespace Fusion {
    class FUSION_API Event {
    };

    /*class FUSION_API EventQueue {
     * https://codereview.stackexchange.com/questions/259265/c-event-system-for-game-engine
        /// Send events to other handlers
        void dispatch_event(const Event* event) {
            for (auto& recipient : event_recipients) {
                recipient->event_queue.push(event);
            }
        }

        /// Invoke processing for events in queue when the time has come (oversimplified)
        void process_event_queue() {
            while (!event_queue.empty()) {
                event_callback(event_queue.front());
                event_queue.pop();
            }
        }

        /// Push to queue manually
        void push_queue(const IEvent* event) {
            event_queue.push(event);
        }
    };*/

    class FUSION_API HandlerFunctionBase {
    public:
        void exec(Event* event) {
            call(event);
        }
        virtual void* id() = 0;
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

        void* id() override {
            return instance;
        }

    private:
        T* instance;
        MemberFunction memberFunction;
    };

    typedef std::list<HandlerFunctionBase*> HandlerList;
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
                    //handler->exec(event);
                }
            }

            delete event;
        }

        template<class T, class EventType>
        void subscribe(T* instance, void (T::*memberFunction)(const EventType&)) {
            auto* handlers = subscribers[typeid(EventType)];

            // First time initialization
            if (handlers == nullptr) {
                handlers = new HandlerList();
                subscribers[typeid(EventType)] = handlers;
            }

            handlers->push_back(new MemberFunctionHandler(instance, memberFunction));
        }

        template<class EventType, class T>
        void destroy(T* instance) {
            auto* handlers = subscribers[typeid(EventType)];

            if (handlers == nullptr) {
                return;
            }

            handlers->remove_if([&instance](HandlerFunctionBase* h){ return h->id() == instance; });
        }
    private:
        std::map<std::type_index, HandlerList*> subscribers;
    };


}
