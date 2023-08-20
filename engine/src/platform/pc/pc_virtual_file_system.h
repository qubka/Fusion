#pragma once

#include "fusion/filesystem/virtual_file_system.h"

namespace fe::pc {
    class FUSION_API VirtualFileSystem final : public fe::VirtualFileSystem {
    public:
        VirtualFileSystem();
        ~VirtualFileSystem() override;

        void mount(const fs::path& path, const fs::path& mount) override;

        void unmount(const fs::path& path) override;

        void readBytes(const fs::path& filepath, const std::function<void(gsl::span<const uint8_t>)>& handler) const override;

        std::string readText(const fs::path& filepath) const override;

        bool writeBytes(const fs::path& filepath, gsl::span<const uint8_t> buffer) const override;

        bool writeText(const fs::path& filepath, std::string_view text) const override;

        bool isExists(const fs::path& filepath) const override;

        bool isDirectory(const fs::path& filepath) const override;

        std::vector<fs::path> getFiles(const fs::path& filepath, bool recursive = false, std::string_view ext = "") const override;

    private:
        std::unordered_map<fs::path, fs::path, PathHash> mountPoints;
    };
}