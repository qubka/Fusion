#pragma once

namespace fe {
    class Time;
    template<typename Base>
    /**
     * https://accu.org/journals/overload/6/27/bellingham_597/
     */
    class ModuleFactory {
    public:
        struct TCreateValue {
            std::function<std::unique_ptr<Base>()> create;
            typename Base::Stage stage;
            std::vector<std::type_index> requires;
        };
        using TRegistryMap = std::unordered_map<std::type_index, TCreateValue>;

        //ModuleFactory() = default;
        virtual ~ModuleFactory() = default;

        static TRegistryMap& Registry() {
            static TRegistryMap map;
            return map;
        }

        template<typename ... Args>
        class Requires {
            static constexpr size_t value = sizeof...(Args);
        public:
            std::vector<std::type_index> Get() const {
                std::vector<std::type_index> requires;
                requires.reserve(value);
                (requires.emplace_back(typeid(Args)), ...);
                return requires;
            }
        };

        template<typename T>
        class Registrar : public Base {
        public:
            /**
             * Gets the engines instance.
             * @return The current module instance.
             */
            static T* Get() { return Instance; }

            /**
             * Creates a new module singleton instance and registers into the module registry map.
             * @tparam Args Modules that will be initialized before this module.
             * @return A dummy value in static initialization.
             */
            template<typename ... Args>
            static bool Register(typename Base::Stage stage, Requires<Args...>&& requires = {}) {
                ModuleFactory::Registry()[typeid(T)] = { []() {
                    Instance = new T();
                    return std::unique_ptr<Base>(Instance);
                }, stage, requires.Get() };
                return true;
            }

            inline static T* Instance = nullptr;
        };
    };

    /**
     * @brief A interface used for defining engine modules.
     */
    class Module : public ModuleFactory<Module> {
    public:
        /**
         * @brief Represents the stage where the module will be updated in the engine.
         */
        enum class Stage { Never, Pre, Normal, Post, Render };

        using StageIndex = std::pair<Stage, std::type_index>;

        //Module() = default;
        virtual ~Module() = default;
        //NONCOPYABLE(Module);

        /**
         * The update function for the module.
         * @param dt The current delta time.
         */
        virtual void update(const Time& dt) = 0;
    };
}