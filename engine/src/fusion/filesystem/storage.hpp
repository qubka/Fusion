#pragma once

namespace fe {
#if PLATFORM_ANDROID
    void setAssetManager(AAssetManager* assetManager);
#endif
    class Storage;
    using StoragePointer = std::shared_ptr<const Storage>;
    using ByteArray = std::vector<uint8_t>;

    // Abstract class to represent memory that stored _somewhere_ (in system memory or in a file, for example)
    class Storage : public std::enable_shared_from_this<Storage> {
    public:
        Storage() = default;
        virtual ~Storage() = default;

        virtual const uint8_t* getData() const = 0;
        virtual size_t getSize() const = 0;
        virtual bool isFast() const = 0;

        static StoragePointer create(size_t size, uint8_t* data);
        static StoragePointer readFile(const std::filesystem::path& filename);
        StoragePointer createView(size_t size = 0, size_t offset = 0) const;
    };
}
