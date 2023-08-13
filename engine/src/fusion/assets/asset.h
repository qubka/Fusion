#pragma once

namespace fe {
    /**
     * @brief A managed resource object.
     */
    class FUSION_API Asset {
    public:
        Asset() = default;
        virtual ~Asset() = default;
        NONCOPYABLE(Asset);

        virtual type_index getType() const = 0;
        virtual const fs::path& getPath() const = 0;
        virtual const std::string& getName() const = 0;

        virtual void loadResource() = 0;
    };
    /*class FUSION_API Asset {
    public:
        Asset() = default;
        virtual ~Asset() = default;
        NONCOPYABLE(Asset); // TEMP

        type_index getType() const { return type; }
        const fs::path& getPath() const { return path; }

        virtual void load() = 0;
        virtual void unload() = 0;

    protected:
        fs::path path;
        type_index type;
        bool loaded{ false };
        bool internal{ false };
    };*/
}