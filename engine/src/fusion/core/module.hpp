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
        };
        using TRegistryMap = std::vector<std::pair<type_index, TCreateValue>>;

        //ModuleFactory() = default;
        virtual ~ModuleFactory() = default;

        static TRegistryMap& Registry() {
            static TRegistryMap map;
            return map;
        }

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
            static bool Register(std::string_view name, typename Base::Stage stage) {
                auto& registry = ModuleFactory::Registry();

                auto it = std::find_if(registry.begin(), registry.end(), [](const auto& p) {
                    return p.first == type_id<T>;
                });

                if (it == registry.end()) {
                    // Then, add the module
                    registry.emplace_back(type_id<T>, TCreateValue{ []() {
                        ModuleInstance = new T();
                        return std::unique_ptr<Base>(ModuleInstance);
                    }, name, stage});
                } else {
                    LOG_WARNING << "Module already registered";
                    return false;
                }

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

        /**
         * The stop function for the module.
         */
        virtual void onStop() {};

    private:
        bool started{ false };
    };
}