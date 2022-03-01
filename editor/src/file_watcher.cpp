#include "file_watcher.hpp"

using namespace fe;

FileWatcher::FileWatcher(const std::string& path_to_watch, std::chrono::duration<int, std::milli> delay)
    : path_to_watch{ path_to_watch }
    , delay{ delay }
{
    for (auto& file: std::filesystem::recursive_directory_iterator(path_to_watch)) {
        paths[file.path().string()] = std::filesystem::last_write_time(file);
    }
}

void FileWatcher::start(const std::function<void(std::string, FileStatus)>& action) {
    while (running) {
        std::this_thread::sleep_for(delay);

        auto it = paths.begin();
        while (it != paths.end()) {
            if (!std::filesystem::exists(it->first)) {
                action(it->first, FileStatus::Erased);
                it = paths.erase(it);
            } else {
                it++;
            }
        }

        for (auto& file: std::filesystem::recursive_directory_iterator(path_to_watch)) {
            auto current_file_last_write_time = std::filesystem::last_write_time(file);
            const std::string& key = file.path().string();

            if (!contains(key)) {
                paths[key] = current_file_last_write_time;
                action(key, FileStatus::Created);
            } else {
                if (paths[key] != current_file_last_write_time) {
                    paths[key] = current_file_last_write_time;
                    action(key, FileStatus::Modified);
                }
            }
        }
    }
}