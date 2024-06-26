#pragma once

#if FUSION_SCRIPTING

extern "C" {
    typedef struct _MonoClass MonoClass;
    typedef struct _MonoObject MonoObject;
    typedef struct _MonoMethod MonoMethod;
    typedef struct _MonoAssembly MonoAssembly;
    typedef struct _MonoImage MonoImage;
    typedef struct _MonoClassField MonoClassField;
    typedef struct _MonoString MonoString;
    typedef struct _MonoDomain MonoDomain;
}

namespace fe {
    class Scene;

    enum class ScriptFieldType : unsigned char {
        None = 0,
        Float,
        Double,
        Bool,
        Char,
        Byte,
        Short,
        Int,
        Long,
        UByte,
        UShort,
        UInt,
        ULong,
        Vector2,
        Vector3,
        Vector4,
        Quaternion,
        Entity
    };

    struct ScriptField {
        ScriptFieldType type;
        std::string name;
        MonoClassField* classField{ nullptr };
    };

    struct ScriptFieldInstance {
        ScriptField field;

        template<typename T>
        T getValue() {
            static_assert(sizeof(T) <= 16, "Type too large!");
            return *(T*)buffer;
        }

        template<typename T>
        void setValue(T value) {
            static_assert(sizeof(T) <= 16, "Type too large!");
            std::memcpy(buffer, &value, sizeof(T));
        }

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(cereal::make_nvp("type", field.type));
            switch (field.type) {
                case ScriptFieldType::Float:
                    archive(cereal::make_nvp("value", *reinterpret_cast<float*>(buffer)));
                    break;
                case ScriptFieldType::Double:
                    archive(cereal::make_nvp("value", *reinterpret_cast<double*>(buffer)));
                    break;
                case ScriptFieldType::Bool:
                    archive(cereal::make_nvp("value", *reinterpret_cast<bool*>(buffer)));
                    break;
                case ScriptFieldType::Char:
                    archive(cereal::make_nvp("value", *reinterpret_cast<char*>(buffer)));
                    break;
                case ScriptFieldType::Byte:
                    archive(cereal::make_nvp("value", *reinterpret_cast<int8_t*>(buffer)));
                    break;
                case ScriptFieldType::Short:
                    archive(cereal::make_nvp("value", *reinterpret_cast<int16_t*>(buffer)));
                    break;
                case ScriptFieldType::Int:
                    archive(cereal::make_nvp("value", *reinterpret_cast<int32_t*>(buffer)));
                    break;
                case ScriptFieldType::Long:
                    archive(cereal::make_nvp("value", *reinterpret_cast<int64_t*>(buffer)));
                    break;
                case ScriptFieldType::UByte:
                    archive(cereal::make_nvp("value", *reinterpret_cast<uint8_t*>(buffer)));
                    break;
                case ScriptFieldType::UShort:
                    archive(cereal::make_nvp("value", *reinterpret_cast<uint16_t*>(buffer)));
                    break;
                case ScriptFieldType::UInt:
                    archive(cereal::make_nvp("value", *reinterpret_cast<uint32_t*>(buffer)));
                    break;
                case ScriptFieldType::ULong:
                    archive(cereal::make_nvp("value", *reinterpret_cast<uint64_t*>(buffer)));
                    break;
                case ScriptFieldType::Vector2:
                    archive(cereal::make_nvp("value", *reinterpret_cast<glm::vec2*>(buffer)));
                    break;
                case ScriptFieldType::Vector3:
                    archive(cereal::make_nvp("value", *reinterpret_cast<glm::vec3*>(buffer)));
                    break;
                case ScriptFieldType::Vector4:
                case ScriptFieldType::Quaternion:
                    archive(cereal::make_nvp("value", *reinterpret_cast<glm::vec4*>(buffer)));
                    break;
                default:
                    break;
            }
        }

    private:
        uint8_t buffer[16]{};

        friend class ScriptEngine;
        friend class ScriptInstance;
    };

    class FUSION_API ScriptClass {
    public:
        ScriptClass() = default;
        ScriptClass(std::string classNamespace, std::string className, bool isCore = false);

        MonoObject* instantiate();

        MonoMethod* getMethod(std::string_view name, int parameterCount);
        MonoObject* invokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

        const std::unordered_map<std::string, ScriptField>& getFields() const { return fields; }

    private:
        std::string classNamespace;
        std::string className;

        std::unordered_map<std::string, ScriptField> fields;

        MonoClass* monoClass{ nullptr };

        friend class ScriptEngine;
    };

    class FUSION_API ScriptInstance {
    public:
        ScriptInstance(std::shared_ptr<ScriptClass> scriptClass, entt::entity entity);

        void invokeOnCreate();
        void invokeOnUpdate(float ts);

        std::shared_ptr<ScriptClass>& getScriptClass() { return scriptClass; }

        template<typename T>
        T getFieldValue(const std::string& name) {
            static_assert(sizeof(T) <= 16, "Type too large!");

            bool success = getFieldValueInternal(name, FieldValueBuffer);
            if (!success)
                return T{};

            return *(T*)FieldValueBuffer;
        }

        template<typename T>
        void setFieldValue(const std::string& name, T value) {
            static_assert(sizeof(T) <= 16, "Type too large!");

            setFieldValueInternal(name, &value);
        }

        MonoObject* getManagedObject() { return instance; }

    private:
        bool getFieldValueInternal(const std::string& name, void* buffer);
        bool setFieldValueInternal(const std::string& name, const void* value);

    private:
        std::shared_ptr<ScriptClass> scriptClass;

        MonoObject* instance{ nullptr };
        MonoMethod* constructor{ nullptr };
        MonoMethod* onCreateMethod{ nullptr };
        MonoMethod* onUpdateMethod{ nullptr };

        inline static char FieldValueBuffer[16];

        friend class ScriptEngine;
        friend struct ScriptFieldInstance;
    };

    class ScriptComponent;
    using ScriptFieldMap = fst::unordered_flatmap<std::string, ScriptFieldInstance>;

    template<typename T>
    class Module;

    class FUSION_API ScriptEngine {
        friend class Module<ScriptEngine>;
    private:
        ScriptEngine();
        ~ScriptEngine();

    public:
        static ScriptEngine* Get() { return Instance; }

        void reloadAssembly();
        void unloadAssembly();

        void onRuntimeStart();
        void onRuntimeStop();

        void onCreateEntity(entt::entity entity, ScriptComponent& script);
        void onUpdateEntity();

        bool entityClassExists(const std::string& fullClassName);
        void setAssemblyPaths(fs::path coreFilepath, fs::path appFilepath);

        Scene* getSceneContext();

        std::shared_ptr<ScriptClass> getEntityClass(const std::string& name);
        std::unordered_map<std::string, std::shared_ptr<ScriptClass>>& getEntityClasses();

        MonoImage* getCoreAssemblyImage();
        MonoString* createString(const char* string);

    private:
        void onStart();
        void onUpdate();
        void onStop();

        void initMono();
        void shutdownMono();

        MonoObject* instantiateClass(MonoClass* monoClass);
        void loadAssemblyClasses();

        friend class ScriptClass;
        friend class ScriptGlue;
        friend class ScriptInstance;

    private:
        MonoDomain* rootDomain{ nullptr };
        MonoDomain* appDomain{ nullptr };

        MonoAssembly* coreAssembly{ nullptr };
        MonoAssembly* appAssembly{ nullptr };

        MonoImage* coreAssemblyImage{ nullptr };
        MonoImage* appAssemblyImage{ nullptr };

        fs::path coreAssemblyFilepath;
        fs::path appAssemblyFilepath;

        ScriptClass entityCoreClass;

        std::unordered_map<std::string, std::shared_ptr<ScriptClass>> entityClasses;

        // Runtime
        Scene* sceneContext{ nullptr };

#ifdef FUSION_DEBUG
        bool enableDebugging{ true };
#else
        bool enableDebugging{ false };
#endif

        static ScriptEngine* Instance;
    };
}

#endif