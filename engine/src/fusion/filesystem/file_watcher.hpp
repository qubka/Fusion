#pragma once

namespace fe {
    // Define available file changes
    enum class FileStatus : uint8_t { Created, Modified, Erased };

    class FileWatcher {
    public:
        // Keep a record of files from the base directory and their last modification time
        FileWatcher(const fs::path& watchPath, std::chrono::duration<int, std::milli> delay) : watchPath{watchPath}, delay{delay} {
            for (auto& entry: fs::recursive_directory_iterator(watchPath)) {
                auto& file = entry.path();
                paths[file] = fs::last_write_time(file);
            }
        }

        // Monitor "path_to_watch" for changes and in case of a change execute the user supplied "action" function
        void start(const std::function<void(const fs::path&, FileStatus)>& action) {
            while (running) {
                // Wait for "delay" milliseconds
                std::this_thread::sleep_for(delay);

                auto it = paths.begin();
                while (it != paths.end()) {
                    if (!fs::exists(it->first)) {
                        action(it->first, FileStatus::Erased);
                        it = paths.erase(it);
                    } else {
                        it++;
                    }
                }

                // Check if a file was created or modified
                for (auto& entry: fs::recursive_directory_iterator(watchPath)) {
                    auto& path = entry.path();
                    auto lastWriteTime = fs::last_write_time(path);

                    // File creation
                    if (paths.find(path) == paths.end()) {
                        paths[path] = lastWriteTime;
                        action(path, FileStatus::Created);
                        // File modification
                    } else {
                        if (paths[path] != lastWriteTime) {
                            paths[path] = lastWriteTime;
                            action(path, FileStatus::Modified);
                        }
                    }
                }
            }
        }

        const fs::path& getWatchPath() const { return watchPath; }

    private:
        std::unordered_map<fs::path, fs::file_time_type> paths;
        fs::path watchPath;
        std::chrono::duration<int, std::milli> delay{}; // Time interval at which we check the base folder for changes
        bool running{ true };
    };
}
