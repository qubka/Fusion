#include "storage.hpp"

#include <utility>

#if defined(WIN32)
#include <windows.h>
#endif

using namespace fe;

#if defined(__ANDROID__)
AAssetManager* assetManager{ nullptr };
void setAssetManager(AAssetManager* assetManager) {
    vkx::storage::assetManager = assetManager;
}
#endif
class ViewStorage : public Storage {
public:
    ViewStorage(StoragePointer owner, size_t size, const uint8_t* data)
        : owner_{std::move(owner)}
        , size_{size}
        , data_{data} {
    }
    const uint8_t* data() const override { return data_; }
    size_t size() const override { return size_; }
    bool isFast() const override { return owner_->isFast(); }

private:
    const StoragePointer owner_;
    const size_t size_;
    const uint8_t* data_;
};

StoragePointer Storage::createView(size_t viewSize, size_t offset) const {
    auto selfSize = size();
    if (0 == viewSize) {
        viewSize = selfSize;
    }
    if ((viewSize + offset) > selfSize) {
        return {};
        //TODO: Disable te exception for now and return an empty storage instead.
        //throw std::runtime_error("Invalid mapping range");
    }
    return std::make_shared<ViewStorage>(shared_from_this(), viewSize, data() + offset);
}

class MemoryStorage : public Storage {
public:
    MemoryStorage(size_t size, const uint8_t* data = nullptr) {
        data_.resize(size);
        if (data) {
            memcpy(data_.data(), data, size);
        }
    }

    const uint8_t* data() const override { return data_.data(); }
    size_t size() const override { return data_.size(); }
    bool isFast() const override { return true; }

private:
    ByteArray data_;
};

#if defined(__ANDROID__) || defined(WIN32)
#define MAPPED_FILES 1
#else
#define MAPPED_FILES 0
#endif

#if MAPPED_FILES
class FileStorage : public Storage {
public:
    static StoragePointer create(const std::string& filename, size_t size, const uint8_t* data);
    FileStorage(const std::string& filename);
    ~FileStorage();
    // Prevent copying
    FileStorage(const FileStorage& other) = delete;
    FileStorage& operator=(const FileStorage& other) = delete;

    const uint8_t* data() const override { return _mapped; }
    size_t size() const override { return _size; }
    bool isFast() const override { return false; }

private:
    size_t _size{ 0 };
    uint8_t* _mapped{ nullptr };
#if defined(__ANDROID__)
    AAsset* _asset{ nullptr };
#elif (WIN32)
    HANDLE _file{ INVALID_HANDLE_VALUE };
    HANDLE _mapFile{ INVALID_HANDLE_VALUE };
#else
    ByteArray _data;
#endif
};

FileStorage::FileStorage(const std::filesystem::path& filename) {
    assert(std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename));
#if defined(__ANDROID__)
    // Load shader from compressed asset
    _asset = AAssetManager_open(assetManager, filename.c_str(), AASSET_MODE_BUFFER);
    assert(_asset);
    _size = AAsset_getLength(_asset);
    assert(_size > 0);
    _mapped = (uint8_t*)(AAsset_getBuffer(_asset));
#elif (WIN32)
    _file = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (_file == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to open file");
    }
    {
        DWORD dwFileSizeHigh;
        _size = GetFileSize(_file, &dwFileSizeHigh);
        _size += (((size_t)dwFileSizeHigh) << 32);
    }
    _mapFile = CreateFileMappingA(_file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (_mapFile == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to create mapping");
    }
    _mapped = (uint8_t*)MapViewOfFile(_mapFile, FILE_MAP_READ, 0, 0, 0);
#endif
}

FileStorage::~FileStorage() {
#if defined(__ANDROID__)
    AAsset_close(_asset);
#elif (WIN32)
    UnmapViewOfFile(_mapped);
    CloseHandle(_mapFile);
    CloseHandle(_file);
#endif
}
#endif

StoragePointer Storage::Create(size_t size, uint8_t* data) {
    return std::make_shared<MemoryStorage>(size, data);
}

StoragePointer Storage::ReadFile(const std::filesystem::path& filename) {
    assert(std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename));
#if MAPPED_FILES
    return std::make_shared<FileStorage>(filename);
#else
    // FIXME move to posix memory mapped files
    // open the file:
    std::ifstream infile{filename, std::ios::binary};
    if (!infile.is_open()) {
        throw std::runtime_error("File " + filename.string() + " not found");
    }
    // Stop eating new lines in binary mode!!!
    infile.unsetf(std::ios::skipws);

    // get its size:
    std::streampos filesize;

    infile.seekg(0, std::ios::end);
    filesize = infile.tellg();
    infile.seekg(0, std::ios::beg);

    ByteArray filedata;
    // reserve capacity
    filedata.reserve(filesize);
    // read the data:
    filedata.insert(filedata.begin(), std::istream_iterator<uint8_t>(infile), std::istream_iterator<uint8_t>());
    infile.close();

    return std::make_shared<MemoryStorage>(filedata.size(), filedata.data());
#endif
}