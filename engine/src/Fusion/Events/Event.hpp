#pragma once

class GLFWwindow;

namespace Fusion {
    class FUSION_API Event {
    };

    typedef std::queue<Event*> EventList;
    class FUSION_API EventQueue {
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
                list = new EventList();
                events[typeid(EventType)] = list;
            }

            list->push(std::move(event));
        }

        template<typename EventType>
        EventType* next(bool handle = false) {
            if (auto it = events.find(typeid(EventType)); it != events.end()) {
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
                while(!queue->empty())
                    queue->pop();
            }
        }

    private:
        std::map<std::type_index, EventList*> events;
    };
}
