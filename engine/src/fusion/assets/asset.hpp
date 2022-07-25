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
        virtual std::string toString() const = 0;

        /*virtual bool operator==(const Asset& other) const {
            return toString() == other.toString();
        }

        virtual bool operator!=(const Asset& other) const {
            return !operator==(other);
        }*/

    protected:
        uuids::uuid uuid{ uuid_random_generator() };
    };
}