#include "storage.h"

using namespace fe;

#if FUSION_PLATFORM_WINDOWS
#include <windows.h>
#endif

#if FUSION_PLATFORM_ANDROID
#include <android/asset_manager.h>
#include "fusion/devices/device_manager.h"
#endif

class FUSION_API ViewStorage : public Storage {
public:
    ViewStorage(const StoragePointer& owner, gsl::span<const std::byte> buffer)
        : owner{owner}
        , buffer{buffer} {
    }

    const std::byte* data() const override { return buffer.data(); }
    size_t size() const override { return buffer.size(); }
    bool isFast() const override { return owner->isFast(); }

private:
    StoragePointer owner;
    gsl::span<const std::byte> buffer;
};

class FUSION_API MemoryStorage : public Storage {
public:
    explicit MemoryStorage(gsl::span<const std::byte> data)
        : buffer(data.size()) {
        if (data.data()) {
            std::memcpy(buffer.data(), data.data(), data.size());
        }
    }
    explicit MemoryStorage(ByteArray&& byteArray)
        : buffer{std::move(byteArray)} {
    }

    const std::byte* data() const override { return reinterpret_cast<const std::byte*>(buffer.data()); }
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
class FUSION_API FileStorage : public Storage {
public:
    explicit FileStorage(const fs::path& filepath);
    ~FileStorage() override;
    // Prevent copying
    FileStorage(const FileStorage& other) = delete;
    FileStorage& operator=(const FileStorage& other) = delete;

    const std::byte* data() const override { return buffer.data(); }
    size_t size() const override { return buffer.size(); }
    bool isFast() const override { return false; }

    static StoragePointer Create(gsl::span<const std::byte> buffer);

private:
    gsl::span<const std::byte> buffer;
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
    auto assetManager = static_cast<AAssetManager*>(DeviceManager::Get()->getNativeManager());
    asset = AAssetManager_open(assetManager, filepath.string().c_str(), AASSET_MODE_BUFFER);
    if (!asset)
        throw std::runtime_error("File " + filepath.string() + " could not be opened");
    buffer = { (std::byte*)AAsset_getBuffer(asset), static_cast<size_t>(AAsset_getLength(asset)) };
    if (!buffer.data())
        throw std::runtime_error("File " + filepath.string() + " is invalid");
#elif FUSION_PLATFORM_WINDOWS
    file = CreateFileA(filepath.string().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (file == INVALID_HANDLE_VALUE)
        throw std::runtime_error("File " + filepath.string() + " could not be opened");
    DWORD dwFileSizeHigh;
    size_t size = GetFileSize(file, &dwFileSizeHigh);
    size += ((static_cast<size_t>(dwFileSizeHigh)) << 32);
    mapFile = CreateFileMappingA(file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (mapFile == INVALID_HANDLE_VALUE)
        throw std::runtime_error("File " + filepath.string() + " could not be mapped");
    buffer = { static_cast<std::byte*>(MapViewOfFile(mapFile, FILE_MAP_READ, 0, 0, 0)), size };
    if (!buffer.data())
        throw std::runtime_error("File " + filepath.string() + " is invalid");
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

StoragePointer Storage::Create(gsl::span<const std::byte> buffer)  {
    return std::make_shared<MemoryStorage>(buffer);
}

StoragePointer Storage::ReadFile(const fs::path& filepath) {
#if MAPPED_FILES
    return std::make_shared<FileStorage>(filepath);
#else
    // FIXME move to posix memory mapped files
    // open the file:
    std::ifstream is{filepath, std::ios::binary};

    if (!is.is_open())
        throw std::runtime_error("File " + filepath.string() + " could not be opened");

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