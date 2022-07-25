#include "storage.hpp"

using namespace fe;

#if FUSION_PLATFORM_WINDOWS
#include <windows.h>
#endif

#if FUSION_PLATFORM_ANDROID
AAssetManager* assetManager = nullptr;
void setAssetManager(AAssetManager* assetManager) {
    vkx::storage::assetManager = assetManager;
}
#endif

class ViewStorage : public Storage {
public:
    ViewStorage(StoragePointer owner, std::span<const uint8_t> buffer)
        : owner{std::move(owner)}
        , buffer{buffer} {
    }

    const uint8_t* data() const override { return buffer.data(); }
    size_t size() const override { return buffer.size(); }
    bool isFast() const override { return owner->isFast(); }

private:
    StoragePointer owner;
    std::span<const uint8_t> buffer;
};

class MemoryStorage : public Storage {
public:
    explicit MemoryStorage(std::span<const uint8_t> data)
        : buffer(data.size()) {
        if (data.data()) {
            std::memcpy(buffer.data(), data.data(), data.size());
        }
    }
    explicit MemoryStorage(ByteArray&& byteArray)
        : buffer{std::move(byteArray)} {
    }

    const uint8_t* data() const override { return buffer.data(); }
    size_t size() const override { return buffer.size(); }
    bool isFast() const override { return true; }

private:
    ByteArray buffer;
};

#if FUSION_PLATFORM_ANDROID || FUSION_PLATFORM_WINDOWS
#define MAPPED_FILES 1
#else
#define MAPPED_FILES 0
#endif

#if MAPPED_FILES
class FileStorage : public Storage {
public:
    explicit FileStorage(const fs::path& filepath);
    ~FileStorage() override;
    // Prevent copying
    FileStorage(const FileStorage& other) = delete;
    FileStorage& operator=(const FileStorage& other) = delete;

    const uint8_t* data() const override { return buffer.data(); }
    size_t size() const override { return buffer.size(); }
    bool isFast() const override { return false; }

    static StoragePointer Create(std::span<const uint8_t> buffer);

private:
    std::span<const uint8_t> buffer;
#if FUSION_PLATFORM_ANDROID
    AAsset* asset{ nullptr };
#elif FUSION_PLATFORM_WINDOWS
    HANDLE file{ INVALID_HANDLE_VALUE };
    HANDLE mapFile{ INVALID_HANDLE_VALUE };
#else
    //ByteArray buffer;
#endif
};

FileStorage::FileStorage(const fs::path& filepath) {
#if FUSION_PLATFORM_ANDROID
    // Load shader from compressed asset
    asset = AAssetManager_open(assetManager, filepath.string().c_str(), AASSET_MODE_BUFFER);
    if (!asset) {
        throw std::runtime_error("File " + filepath.string() + " could not be opened");
    }
    buffer = { static_cast<uint8_t*>(AAsset_getBuffer(asset)), AAsset_getLength(asset) };
    if (!buffer.data()) {
        throw std::runtime_error("File " + filepath.string() + " is invalid");
    }
#elif FUSION_PLATFORM_WINDOWS
    file = CreateFileA(filepath.string().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("File " + filepath.string() + " could not be opened");
    }
    DWORD dwFileSizeHigh;
    size_t size = GetFileSize(file, &dwFileSizeHigh);
    size += ((static_cast<size_t>(dwFileSizeHigh)) << 32);
    mapFile = CreateFileMappingA(file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (mapFile == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("File " + filepath.string() + " could not be mapped");
    }
    buffer = { static_cast<uint8_t*>(MapViewOfFile(mapFile, FILE_MAP_READ, 0, 0, 0)), size };
    if (!buffer.data()) {
        throw std::runtime_error("File " + filepath.string() + " is invalid");
    }
#endif
}

FileStorage::~FileStorage() {
#if FUSION_PLATFORM_ANDROID
    AAsset_close(asset);
#elif FUSION_PLATFORM_WINDOWS
    UnmapViewOfFile(buffer.data());
    CloseHandle(mapFile);
    CloseHandle(file);
#endif
}
#endif

StoragePointer Storage::Create(std::span<const uint8_t> buffer)  {
    return std::make_shared<MemoryStorage>(buffer);
}

StoragePointer Storage::ReadFile(const fs::path& filepath) {
#if MAPPED_FILES
    return std::make_shared<FileStorage>(filepath);
#else
    // FIXME move to posix memory mapped files
    // open the file:
    std::ifstream is{filepath, std::ios::binary};

    if (!is.is_open()) {
        throw std::runtime_error("File " + filepath.string() + " could not be opened");
    }

    // Stop eating new lines in binary mode!!!
    is.unsetf(std::ios::skipws);

    std::streampos size;
    is.seekg(0, std::ios::end);
    size = is.tellg();
    is.seekg(0, std::ios::beg);

    ByteArray buffer;
    buffer.reserve(size);
    buffer.insert(buffer.begin(), std::istream_iterator<uint8_t>(is), std::istream_iterator<uint8_t>());

    return std::make_shared<MemoryStorage>(std::move(buffer));
#endif
}