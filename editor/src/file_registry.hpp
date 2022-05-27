#pragma once

#include "database.hpp"
#include "file_watcher.hpp"

namespace fe {
    class FileRegistry {
    public:
        FileRegistry() = default;
        ~FileRegistry() = default;

        //using Model = std::shared_ptr<vkx::model::Model>;
        //using Texture = std::shared_ptr<vkx::texture::Texture>;

        void start() {
            cache();
            std::thread t1 ([&] {
                watcher.start([&](const std::string& path, FileStatus status) {
                    // Process only regular files, all other file types are ignored
                    if (!std::filesystem::is_regular_file(path) && status != FileStatus::Erased) {
                        return;
                    }

                    switch(status) {
                        case FileStatus::Created:
                            onFileCreated(path);
                            break;
                        case FileStatus::Modified:
                            onFileModified(path);
                            break;
                        case FileStatus::Erased:
                            onFileErased(path);
                            break;
                        default:
                            LOG_INFO << "Error! Unknown file status.";
                    }
                });
            });
            t1.detach();
        }

        void destroy() {
        }

        void onFileCreated(const std::string& path) {
            LOG_INFO << "File created: " << path;

        }

        void onFileModified(const std::string& path) {
            LOG_INFO << "File modified: " << path;

        }

        void onFileErased(const std::string& path) {
            LOG_INFO << "File erased: " << path;

        }

        void cache() {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(getAssetPath()))
                LOG_INFO << entry.path();
        }

    private:
        FileWatcher watcher{getAssetPath(), std::chrono::milliseconds(1000)};
        //Database database{};

        //std::unordered_map<std::string, Model> models;
        //std::unordered_map<std::string, Texture> textures;
    };
}
