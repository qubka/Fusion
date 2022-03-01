#pragma once

namespace fe {
    // Define available file changes
    enum class FileStatus { Created, Modified, Erased };

    class FileWatcher {
    public:
        FileWatcher(const std::string& path_to_watch, std::chrono::duration<int, std::milli> delay);

        void start(const std::function<void(std::string, FileStatus)>& action);

        bool isRunning() const { return running; }

    private:
        std::string path_to_watch;
        std::chrono::duration<int, std::milli> delay;
        std::unordered_map<std::string, std::filesystem::file_time_type> paths;
        bool running{ true };

        bool contains(const std::string& key) {
            return paths.find(key) != paths.end();
        }
    };
}
