#include "virtual_file_system.hpp"
#include "file_system.hpp"

using namespace fe;

fs::path VirtualFileSystem::resolvePhysicalPath(const fs::path& path) {
    if (auto it = mountPoints.find(*path.begin()); it != mountPoints.end()) {
        return it->second / strip_root(path);
    }
    LOG_WARNING << "Physical path: \"" << path << "\" was not found";
    return {};
}

void VirtualFileSystem::readBytes(const fs::path& path, const std::function<void(std::span<const uint8_t>)>& handler) {
    FileSystem::ReadBytes(resolvePhysicalPath(path), handler);
}

std::string VirtualFileSystem::readText(const fs::path& path) {
    return FileSystem::ReadText(resolvePhysicalPath(path));
}

bool VirtualFileSystem::writeBytes(const fs::path& path, std::span<const uint8_t> buffer) {
    return FileSystem::WriteBytes(resolvePhysicalPath(path), buffer);
}

bool VirtualFileSystem::writeText(const fs::path& path, std::string_view text) {
    return FileSystem::WriteText(resolvePhysicalPath(path), text);
}

void VirtualFileSystem::mount(const fs::path& virtualPath, const fs::path& physicalPath) {
    mountPoints[virtualPath] = physicalPath;
}

void VirtualFileSystem::unmount(const fs::path& path) {
    mountPoints[path].clear();
}