#include "storage.hpp"

#include <utility>

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
    ViewStorage(StoragePointer owner, size_t size, const uint8_t* data)
        : owner{std::move(owner)}
        , size{size}
        , data{data} {}
    const uint8_t* getData() const override { return data; }
    size_t getSize() const override { return size; }
    bool isFast() const override { return owner->isFast(); }

private:
    const StoragePointer owner;
    const size_t size;
    const uint8_t* data;
};

StoragePointer Storage::createView(size_t viewSize, size_t offset) const {
    auto selfSize = getSize();
    if (viewSize == 0) {
        viewSize = selfSize;
    }
    if ((viewSize + offset) > selfSize) {
        return {};
        //TODO: Disable te exception for now and return an empty storage instead.
        //throw std::runtime_error("Invalid mapping range");
    }
    return std::make_shared<ViewStorage>(shared_from_this(), viewSize, getData() + offset);
}

class MemoryStorage : public Storage {
public:
    explicit MemoryStorage(size_t size, const uint8_t* data = nullptr) {
        buffer.resize(size);
        if (data) {
            memcpy(buffer.data(), data, size);
        }
    }

    const uint8_t* getData() const override { return buffer.data(); }
    size_t getSize() const override { return buffer.size(); }
    bool isFast() const override { return true; }

private:
    std::vector<uint8_t> buffer;
};

#if FUSION_PLATFORM_ANDROID || FUSION_PLATFORM_WINDOWS
#define MAPPED_FILES 1
#else
#define MAPPED_FILES 0
#endif

#if MAPPED_FILES
class FileStorage : public Storage {
public:
    static StoragePointer create(const std::filesystem::path& filename, size_t size, const uint8_t* data);
    explicit FileStorage(const std::filesystem::path& filename);
    ~FileStorage() override;
    // Prevent copying
    FileStorage(const FileStorage& other) = delete;
    FileStorage& operator=(const FileStorage& other) = delete;

    const uint8_t* getData() const override { return mapped; }
    size_t getSize() const override { return size; }
    bool isFast() const override { return false; }

private:
    size_t size{ 0 };
    uint8_t* mapped{ nullptr };
#if FUSION_PLATFORM_ANDROID
    AAsset* asset{ nullptr };
#elif FUSION_PLATFORM_WINDOWS
    HANDLE file{ INVALID_HANDLE_VALUE };
    HANDLE mapFile{ INVALID_HANDLE_VALUE };
#else
    std::vector<uint8_t> buffer;
#endif
};

FileStorage::FileStorage(const std::filesystem::path& filename) {
#if FUSION_PLATFORM_ANDROID
    // Load shader from compressed asset
    asset = AAssetManager_open(assetManager, filename.c_str(), AASSET_MODE_BUFFER);
    assert(asset);
    size = AAsset_getLength(asset);
    assert(size > 0);
    mapped = reinterpret_cast<uint8_t*>(AAsset_getBuffer(asset));
#elif FUSION_PLATFORM_WINDOWS
    file = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to open file");
    }
    {
        DWORD dwFileSizeHigh;
        size = GetFileSize(file, &dwFileSizeHigh);
        size += (((size_t)dwFileSizeHigh) << 32);
    }
    mapFile = CreateFileMappingA(file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (mapFile == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to create mapping");
    }
    mapped = reinterpret_cast<uint8_t*>(MapViewOfFile(mapFile, FILE_MAP_READ, 0, 0, 0));
#endif
}

FileStorage::~FileStorage() {
#if FUSION_PLATFORM_ANDROID
    AAsset_close(asset);
#elif FUSION_PLATFORM_WINDOWS
    UnmapViewOfFile(mapped);
    CloseHandle(mapFile);
    CloseHandle(file);
#endif
}
#endif

StoragePointer Storage::create(size_t size, uint8_t* data)  {
    return std::make_shared<MemoryStorage>(size, data);
}

StoragePointer Storage::readFile(const std::filesystem::path& filename) {
#if MAPPED_FILES
    return std::make_shared<FileStorage>(filename);
#else
    // FIXME move to posix memory mapped files
    // open the file:
    std::ifstream is{filename, std::ios::binary};
    // Stop eating new lines in binary mode!!!
    is.unsetf(std::ios::skipws);

    std::streampos fileSize;
    is.seekg(0, std::ios::end);
    fileSize = is.tellg();
    is.seekg(0, std::ios::beg);

    std::vector<uint8_t> fileData;
    fileData.reserve(fileSize);

    fileData.insert(fileData.begin(), std::istream_iterator<uint8_t>(is), std::istream_iterator<uint8_t>());

    return std::make_shared<MemoryStorage>(fileData.size(), fileData.data());
#endif
}