#pragma once

namespace fe {
    template<typename Base>
    /**
     * https://accu.org/journals/overload/6/27/bellingham_597/
     */
    class ModuleFactory {
    public:
        struct TCreateValue {
            std::function<std::unique_ptr<Base>()> create;
            std::string_view name;
            typename Base::Stage stage;
            std::vector<type_index> requires;
        };
        using TRegistryMap = std::unordered_map<type_index, TCreateValue>;

        //ModuleFactory() = default;
        virtual ~ModuleFactory() = default;

        static TRegistryMap& Registry() {
            static TRegistryMap map;
            return map;
        }

        template<typename ... Args>
        class Requires {
        public:
            std::vector<type_index> get() const {
                std::vector<type_index> requires;
                requires.reserve(sizeof...(Args));
                (requires.emplace_back(type_id<Args>), ...);
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
            static T* Get() { return ModuleInstance; }

            /**
             * Creates a new module singleton instance and registers into the module registry map.
             * @param name Module name string for debugging purpose.
             * @tparam Args Modules that will be initialized before this module.
             * @return A dummy value in static initialization.
             */
            template<typename ... Args>
            static bool Register(std::string_view name, typename Base::Stage stage, Requires<Args...>&& requires = {}) {
                ModuleFactory::Registry()[type_id<T>] = { []() {
                        ModuleInstance = new T();
                        return std::unique_ptr<Base>(ModuleInstance);
                    }, name, stage, requires.get()
                };
                return true;
            }

        protected:
            inline static T* ModuleInstance = nullptr;
        };
    };

    /**
     * @brief A interface used for defining engine modules.
     */
    class Module : public ModuleFactory<Module> {
        friend class Engine;
    public:
        /**
         * @brief Represents the stage where the module will be updated in the engine.
         */
        enum class Stage : unsigned char { Never, Pre, Normal, Post, Render };

        Module() = default;
        ~Module() override = default;
        NONCOPYABLE(Module);

    protected:
        /**
         * The start function for the module.
         */
        virtual void onStart() {};

        /**
         * The update function for the module.
         */
        virtual void onUpdate() {};

    private:
        bool started{ false };
    };
}