#pragma once

namespace fe {
    /**
     * https://www.cppstories.com/2018/02/factory-selfregister/
     */
    template<typename Base>
    class BitmapFactory {
    public:
        using TLoadMethod = std::function<void(Base&, const fs::path&)>;
        using TWriteMethod = std::function<void(const Base&, const fs::path&)>;
        using TRegistryMap = std::unordered_map<std::string, std::pair<TLoadMethod, TWriteMethod>>;

        //BitmapFactory() = default;
        virtual ~BitmapFactory() = default;

        static TRegistryMap& Registry() {
            static TRegistryMap map;
            return map;
        }

        template<typename T>
        class Registrar {
        protected:
            template<typename ...Args>
            static bool Register(Args&& ... names) {
                for (std::string&& name : { names... })
                    BitmapFactory::Registry()[name] = std::make_pair(&T::Load, &T::Write);
                return true;
            }
        };
    };
}
