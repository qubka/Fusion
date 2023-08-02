#include <utility>

#pragma once

namespace fe {
    // Define available file changes
    enum class FileStatus : unsigned char { Created, Modified, Erased };

    class FileWatcher {
    public:
        explicit FileWatcher(fs::path watchPath, std::function<void(const fs::path&, FileStatus)> callback, const DateTime& interval = 1s)
            : watchPath{std::move(watchPath)}
            , callback{std::move(callback)}
            , elapsedUpdate{interval} {
            for (const auto& entry: fs::recursive_directory_iterator(watchPath)) {
                auto& file = entry.path();
                paths[file] = fs::last_write_time(file);
            }
        }

        void update() {
            if (elapsedUpdate.getElapsed() != 0) {
                auto it = paths.begin();
                while (it != paths.end()) {
                    if (!fs::exists(it->first)) {
                        callback(it->first, FileStatus::Erased);
                        it = paths.erase(it);
                    } else {
                        it++;
                    }
                }

                // Check if a file was created or modified
                for (const auto& entry: fs::recursive_directory_iterator(watchPath)) {
                    auto& path = entry.path();
                    auto lastWriteTime = fs::last_write_time(path);

                    // File creation
                    auto it2 = paths.find(path);
                    if (it2 == paths.end()) {
                        paths[path] = lastWriteTime;
                        callback(path, FileStatus::Created);
                        // File modification
                    } else {
                        auto& time = it2->second;
                        if (time != lastWriteTime) {
                            time = lastWriteTime;
                            callback(path, FileStatus::Modified);
                        }
                    }
                }
            }
        }

        const fs::path& getWatchPath() const { return watchPath; }

    private:
        std::unordered_map<fs::path, fs::file_time_type, PathHash> paths;
        std::function<void(const fs::path&, FileStatus)> callback;
        fs::path watchPath;

        ElapsedTime elapsedUpdate;
    };
}
