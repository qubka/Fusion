#include "virtual_file_system.hpp"
#include "file_system.hpp"

using namespace fe;

fs::path VirtualFileSystem::resolvePhysicalPath(const fs::path& path) {
    auto dir = *path.begin();
    if (auto it = mountPoints.find(dir); it != mountPoints.end()) {
        return it->second / strip_root(path);
    }
    LOG_WARNING << "Physical path: " << path << " was not found";
    return {};
}

std::vector<uint8_t> VirtualFileSystem::readBytes(const fs::path& path) {
    return FileSystem::ReadBytes(resolvePhysicalPath(path));
}

std::string VirtualFileSystem::readText(const fs::path& path) {
    return FileSystem::ReadText(resolvePhysicalPath(path));
}

bool VirtualFileSystem::writeBytes(const fs::path& path, uint8_t* buffer, size_t size) {
    return FileSystem::WriteBytes(resolvePhysicalPath(path), buffer, size);
}

bool VirtualFileSystem::writeText(const fs::path& path, const std::string& text) {
    return FileSystem::WriteText(resolvePhysicalPath(path), text);
}

void VirtualFileSystem::mount(const fs::path& virtualPath, const fs::path& physicalPath) {
    mountPoints[virtualPath] = physicalPath;
}

void VirtualFileSystem::unmount(const fs::path& path) {
    mountPoints[path].clear();
}