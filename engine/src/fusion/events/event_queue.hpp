#pragma once

class GLFWwindow;

namespace fe {
    class Event {
    };

    class EventQueue {
        typedef std::queue<Event*> Queue;
    public:
        EventQueue() = default;
        ~EventQueue() {
            free();
        }

        template<typename EventType>
        void submit(EventType* event) {
            auto* list = events[typeid(EventType)];

            // First time initialization
            if (list == nullptr) {
                list = new Queue();
                events[typeid(EventType)] = list;
            }

            list->push(event);
        }

        template<typename EventType>
        const EventType* next(bool handle = false) {
            if (auto it { events.find(typeid(EventType)) }; it != events.end()) {
                auto& queue{ *it->second };
                if (!queue.empty()) {
                    auto* event = queue.front();
                    if (handle) queue.pop();
                    return static_cast<EventType*>(event);
                }
            }
            return nullptr;
        }

        void free() {
            for (auto [index, queue] : events) {
                while(!queue->empty()) {
                    auto* event = queue->front();
                    queue->pop();
                    delete event;
                }
            }
        }

    private:
        std::map<std::type_index, Queue*> events;
    };
}
