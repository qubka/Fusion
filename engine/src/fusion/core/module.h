#pragma once

namespace fe {
    template<typename Base>
    /**
     * https://accu.org/journals/overload/6/27/bellingham_597/
     */
    class FUSION_API ModuleFactory {
    public:
        struct TCreateValue {
            std::function<std::unique_ptr<Base>()> create;
            std::string_view name;
            typename Base::Stage stage;
        };
        using TRegistryMap = fst::unordered_flatmap<type_index, TCreateValue>;

        //ModuleFactory() = default;
        virtual ~ModuleFactory() = default;

        static TRegistryMap& Registry() {
            static TRegistryMap map;
            return map;
        }

        template<typename T>
        class FUSION_API Registrar : public Base {
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
            static bool Register(std::string_view name, typename Base::Stage stage) {

                // Then, add the module
                ModuleFactory::Registry().emplace(type_id<T>, TCreateValue{ []() {
                    ModuleInstance = new T();
                    return std::unique_ptr<Base>(ModuleInstance);
                }, name, stage});

                return true;
            }

        protected:
            inline static T* ModuleInstance = nullptr;
        };
    };

    /**
     * @brief A interface used for defining engine modules.
     */
    class FUSION_API Module : public ModuleFactory<Module> {
        friend class Engine;
    public:
        /**
         * @brief Represents the stage where the module will be updated in the engine.
         */
        enum class Stage : unsigned char { Never, Pre, /*Main,*/ Post, Render };

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

        /**
         * The stop function for the module.
         */
        virtual void onStop() {};

    private:
        bool started{ false };
    };
}