#include "thread_pool.hpp"

using namespace fe;

ThreadPool::ThreadPool(uint32_t threadCount) {
    workers.reserve(threadCount);

    for (size_t i = 0; i < threadCount; ++i) {
        workers.emplace_back([&]{
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock{queueMutex};
                    condition.wait(lock, [this] {
                        return stop || !tasks.empty();
                    });

                    if (stop && tasks.empty())
                        return;

                    task = std::move(tasks.front());
                    tasks.pop();
                }

                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock{queueMutex};
        stop = true;
    }

    condition.notify_all();

    for (auto& worker: workers)
        worker.join();
}

void ThreadPool::wait() {
    std::unique_lock<std::mutex> lock{queueMutex};

    condition.wait(lock, [&](){
        return tasks.empty();
    });
}