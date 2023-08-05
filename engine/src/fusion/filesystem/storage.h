#pragma once

namespace fe {
    class Storage;
    using StoragePointer = std::shared_ptr<const Storage>;
    using ByteArray = std::vector<uint8_t>;

    // Abstract class to represent memory that stored _somewhere_ (in system memory or in a file, for example)
    class FUSION_API Storage : public std::enable_shared_from_this<Storage> {
    public:
        Storage() = default;
        virtual ~Storage() = default;

        operator bool() const { return data(); }
        operator gsl::span<const std::byte>() const { return { data(), size() }; }

        virtual const std::byte* data() const = 0;
        virtual size_t size() const = 0;
        virtual bool isFast() const = 0;

        static StoragePointer Create(gsl::span<const std::byte> buffer);
        static StoragePointer ReadFile(const fs::path& filename);
    };
}
