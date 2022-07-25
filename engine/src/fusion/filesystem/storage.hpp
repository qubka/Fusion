#pragma once

namespace fe {
#if FUSION_PLATFORM_ANDROID
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

        operator bool() const { return data(); }
        operator std::span<const uint8_t>() const { return { data(), size() }; }

        virtual const uint8_t* data() const = 0;
        virtual size_t size() const = 0;
        virtual bool isFast() const = 0;

        static StoragePointer Create(std::span<const uint8_t> buffer);
        static StoragePointer ReadFile(const fs::path& filename);
    };
}
