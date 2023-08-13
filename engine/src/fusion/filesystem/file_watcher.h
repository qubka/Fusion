#include "file_system.h"

#pragma once

namespace fe {
    // Define available file changes
    enum class FileEvent : unsigned char { Init, Created, Modified, Erased };

    class FUSION_API FileWatcher {
    public:
        FileWatcher(fs::path watchPath, std::function<void(const fs::path&, FileEvent)> changeCallback, std::set<std::string> exts = {}, DateTime interval = 1s)
            : watchDirectory{std::move(watchPath)}
            , callback{std::move(changeCallback)}
            , extensions{std::move(exts)}
            , elapsedUpdate{interval} {
            for (const auto& entry: fs::recursive_directory_iterator(watchDirectory)) {
                auto& path = entry.path();
                if (extensions.empty() || extensions.find(FileSystem::GetExtension(path)) != extensions.end()) {
                    paths[path] = fs::last_write_time(path);
                    callback(path, FileEvent::Init);
                }
            }
        }

        void update() {
            if (elapsedUpdate.getElapsed() != 0) {
                auto it = paths.begin();
                while (it != paths.end()) {
                    if (!fs::exists(it->first)) {
                        callback(it->first, FileEvent::Erased);
                        it = paths.erase(it);
                    } else {
                        it++;
                    }
                }

                // Check if a file was created or modified
                for (const auto& entry: fs::recursive_directory_iterator(watchDirectory)) {
                    auto& path = entry.path();
                    auto lastWriteTime = fs::last_write_time(path);

                    // File creation
                    auto it2 = paths.find(path);
                    if (it2 == paths.end()) {
                        if (extensions.empty() || extensions.find(FileSystem::GetExtension(path)) != extensions.end()) {
                            paths[path] = lastWriteTime;
                            callback(path, FileEvent::Created);
                        }
                        // File modification
                    } else {
                        auto& time = it2->second;
                        if (time != lastWriteTime) {
                            time = lastWriteTime;
                            callback(path, FileEvent::Modified);
                        }
                    }
                }
            }
        }

        const fs::path& getWatchDirectory() const { return watchDirectory; }

    private:
        std::unordered_map<fs::path, fs::file_time_type, PathHash> paths;
        std::set<std::string> extensions;
        std::function<void(const fs::path&, FileEvent)> callback;
        fs::path watchDirectory;

        ElapsedTime elapsedUpdate;
    };
}
