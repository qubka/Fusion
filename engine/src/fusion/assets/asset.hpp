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

        virtual type_index getType() const = 0;
        const fs::path& getPath() const { return path; }
        const std::string& getName() const { return name; }

    protected:
        fs::path path;
        std::string name;
    };
}