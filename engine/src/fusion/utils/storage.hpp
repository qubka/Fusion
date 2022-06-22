#pragma once

#if defined(__ANDROID__)
#include <android/asset_manager.h>
#endif

namespace fe {
#if defined(__ANDROID__)
    void setAssetManager(AAssetManager* assetManager);
#endif
    class Storage;
    using StoragePointer = std::shared_ptr<const Storage>;
    using ByteArray = std::vector<uint8_t>;

    // Abstract class to represent memory that stored _somewhere_ (in system memory or in a file, for example)
    class Storage : public std::enable_shared_from_this<Storage> {
    public:
        virtual ~Storage() {}

        virtual const uint8_t* data() const = 0;
        virtual size_t size() const = 0;
        virtual bool isFast() const = 0;

        static StoragePointer Create(size_t size, uint8_t* data);
        static StoragePointer ReadFile(const std::filesystem::path& filename);
        StoragePointer createView(size_t size = 0, size_t offset = 0) const;

        // Aliases to prevent having to re-write a ton of code
        inline size_t getSize() const { return size(); }
        inline const uint8_t* readData() const { return data(); }
    };
}
