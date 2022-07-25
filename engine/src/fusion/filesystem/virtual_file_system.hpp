#pragma once

#include "fusion/core/module.hpp"

namespace fe {
    /** https://github.com/anthony-y/tiny-vfs */
    class VirtualFileSystem : public Module::Registrar<VirtualFileSystem> {
    public:
        VirtualFileSystem() = default;
        ~VirtualFileSystem() override = default;

        void mount(const fs::path& virtualPath, const fs::path& physicalPath);
        void unmount(const fs::path& path);

        fs::path resolvePhysicalPath(const fs::path& path);

        void readBytes(const fs::path& path, const std::function<void(std::span<const uint8_t>)>& handler);
        std::string readText(const fs::path& path);
        bool writeBytes(const fs::path& path, std::span<const uint8_t> buffer);
        bool writeText(const fs::path& path, std::string_view text);

        const std::unordered_map<fs::path, fs::path>& getMounted() { return mountPoints; };

    private:
        std::unordered_map<fs::path, fs::path> mountPoints;
    };
}
