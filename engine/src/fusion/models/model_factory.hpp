#pragma once

namespace fe {
    template<typename Base>
    class ModelFactory {
    public:
        using TLoadMethod = std::function<void(Base&, const std::filesystem::path&)>;
        using TWriteMethod = std::function<void(const Base&, const std::filesystem::path&)>;
        using TRegistryMap = std::unordered_map<std::string, std::pair<TLoadMethod, TWriteMethod>>;

        //ModelFactory() = default;
        virtual ~ModelFactory() = default;

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
                    ModelFactory::Registry()[name] = std::make_pair(&T::Load, &T::Write);
                return true;
            }
        };
    };
}