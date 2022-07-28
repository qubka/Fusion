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

    protected:
        uuids::uuid uuid{ uuid_random_generator() };
    };
}