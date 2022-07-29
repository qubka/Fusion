#pragma once

namespace fe {
    /**
     * @brief A managed resource object.
     */
    class Asset {
    public:
        Asset() = default;
        virtual ~Asset() = default;
        NONCOPYABLE(Asset);

        virtual type_index getTypeIndex() const = 0;
        virtual const std::string& getName() const = 0;
        virtual const fs::path& getPath() const = 0;

        const uuids::uuid& getUUID() const { return uuid; }

    protected:
        uuids::uuid uuid{ uuid_random_generator() };
    };
}