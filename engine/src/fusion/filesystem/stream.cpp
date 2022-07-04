#include "stream.hpp"
#include "stream_buf.hpp"

#include <physfs.h>

using namespace fe;

PHYSFS_File* OpenWithMode(const std::filesystem::path& filename, FileMode openMode) {
    PHYSFS_File* file = nullptr;

    auto pathStr = filename.string();
    std::replace(pathStr.begin(), pathStr.end(), '\\', '/');

    switch (openMode) {
        case FileMode::Write:
            file = PHYSFS_openWrite(pathStr.c_str());
            break;
        case FileMode::Append:
            file = PHYSFS_openAppend(pathStr.c_str());
            break;
        case FileMode::Read:
            file = PHYSFS_openRead(pathStr.c_str());
    }

    if (file == nullptr)
        throw std::invalid_argument("File could not be found");

    return file;
}

BaseFileStream::BaseFileStream(PHYSFS_File* file) : file{file} {
    if (file == nullptr) {
        throw std::invalid_argument("Attempted to construct fstream with a NULL file");
    }
}

BaseFileStream::~BaseFileStream() {
    PHYSFS_close(file);
}

size_t BaseFileStream::length() {
    return PHYSFS_fileLength(file);
}

IFileStream::IFileStream(const std::filesystem::path& filename)
    : BaseFileStream{OpenWithMode(filename, FileMode::Read)}
    , std::istream{new IFileStreambuf{file}} {
}

IFileStream::~IFileStream() {
    delete rdbuf();
}

OFileStream::OFileStream(const std::filesystem::path& filename, FileMode writeMode)
    : BaseFileStream{OpenWithMode(filename, writeMode)}
    , std::ostream{new OFileStreambuf{file}} {
}

OFileStream::~OFileStream() {
    delete rdbuf();
}