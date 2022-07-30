#pragma once

namespace fe {
    /**
     * @brief A managed resource object.
     */
    class Asset {
    public:
        Asset() = default;
        Asset(fs::path path, std::string name) : path{std::move(path)}, name{std::move(name)} {}
        virtual ~Asset() = default;
        NONCOPYABLE(Asset);

        virtual type_index getType() const = 0;
        const uuids::uuid& getUUID() const { return uuid; }
        const std::string& getName() const { return name; }
        const fs::path& getPath() const { return path; }

        /*template<typename Archive>
        void load(Archive& archive) {
            std::string str;
            archive(cereal::make_nvp("uuid", str),
                    cereal::make_nvp("name", name),
                    cereal::make_nvp("path", path));
            uuid = uuids::uuid::from_string(str).value_or(uuid_random_generator());
        }

        template<typename Archive>
        void save(Archive& archive) const {
            archive(cereal::make_nvp("uuid", uuids::to_string(uuid)),
                    cereal::make_nvp("name", name),
                    cereal::make_nvp("path", path));
        }*/

    protected:
        uuids::uuid uuid{ uuid_random_generator() };
        std::string name;
        fs::path path;
    };
}