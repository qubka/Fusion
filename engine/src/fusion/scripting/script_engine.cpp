#include "script_engine.h"
#include "script_glue.h"

#if FUSION_SCRIPTING

#include "fusion/filesystem/file_system.h"
#include "fusion/core/engine.h"
#include "fusion/scene/scene.h"
#include "fusion/scene/components.h"
#include "fusion/scene/scene_manager.h"
#include "fusion/core/module.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/threads.h>

using namespace fe;

static std::unordered_map<std::string_view, ScriptFieldType> ScriptFieldTypeMap = {
    { "System.Single", ScriptFieldType::Float },
    { "System.Double", ScriptFieldType::Double },
    { "System.Boolean", ScriptFieldType::Bool },
    { "System.Char", ScriptFieldType::Char },
    { "System.Int16", ScriptFieldType::Short },
    { "System.Int32", ScriptFieldType::Int },
    { "System.Int64", ScriptFieldType::Long },
    { "System.Byte", ScriptFieldType::Byte },
    { "System.UInt16", ScriptFieldType::UShort },
    { "System.UInt32", ScriptFieldType::UInt },
    { "System.UInt64", ScriptFieldType::ULong },

    { "System.Numerics.Vector2", ScriptFieldType::Vector2 },
    { "System.Numerics.Vector3", ScriptFieldType::Vector3 },
    { "System.Numerics.Vector4", ScriptFieldType::Vector4 },
    { "System.Numerics.Quaternion", ScriptFieldType::Quaternion },

    { "Fusion.Entity", ScriptFieldType::Entity },
};

namespace Utils {
    static MonoAssembly* LoadMonoAssembly(const fs::path& assemblyPath, bool loadPDB = false) {
        // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
        MonoImageOpenStatus status;
        MonoImage* image = nullptr;
        // fix leak
        FileSystem::ReadBytes(assemblyPath, [&](gsl::span<const std::byte> buffer) {
            image = mono_image_open_from_data_full((char*) buffer.data(), buffer.size(), 1, &status, 0);
        });

        if (status != MONO_IMAGE_OK) {
            const char* errorMessage = mono_image_strerror(status);
            LOG_ERROR <<  "Failed to load assembly file: \"" << errorMessage << "\"";
            return nullptr;
        }

        if (loadPDB) {
            fs::path pdbPath{ assemblyPath };
            pdbPath.replace_extension(".pdb");

            if (fs::exists(pdbPath)) {
                FileSystem::ReadBytes(pdbPath, [&](gsl::span<const std::byte> buffer) {
                    mono_debug_open_image_from_memory(image, reinterpret_cast<const mono_byte*>(buffer.data()), static_cast<int>(buffer.size()));
                    LOG_INFO << "Loaded PDB:" << pdbPath;
                });
            }
        }

        std::string pathString{ assemblyPath.string() };
        MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
        mono_image_close(image);

        return assembly;
    }

    void PrintAssemblyTypes(MonoAssembly* assembly) {
        MonoImage* image = mono_assembly_get_image(assembly);
        const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

        for (int32_t i = 0; i < numTypes; ++i) {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

            const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
            const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
            LOG_VERBOSE << nameSpace << "." << name;
        }
    }

    ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType) {
        const char* typeName = mono_type_get_name(monoType);

        auto it = ScriptFieldTypeMap.find(typeName);
        if (it == ScriptFieldTypeMap.end()) {
            LOG_ERROR << "Unknown type: " << typeName;
            return ScriptFieldType::None;
        }

        return it->second;
    }
}

/*static void OnAppAssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type) {
    if (!assemblyReloadPending && change_type == filewatch::Event::modified) {
        assemblyReloadPending = true;

        Application::Get().SubmitToMainThread([]() {
            AppAssemblyFileWatcher.reset();
            ScriptEngine::ReloadAssembly();
        });
    }
}*/

void ScriptEngine::onStart() {
    initMono();

    ScriptGlue::RegisterFunctions();

    //loadAssembly();
}

void ScriptEngine::onStop() {
    shutdownMono();
}

void ScriptEngine::initMono() {
    mono_set_assemblies_path("engine/mono/lib");

    if (enableDebugging) {
        const char* argv[2] = {
            "--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=MonoDebugger.log",
            "--soft-breakpoints"
        };

        mono_jit_parse_options(2, (char**)argv);
        mono_debug_init(MONO_DEBUG_FORMAT_MONO);
    }

    rootDomain = mono_jit_init("FusionJITRuntime");
    assert(rootDomain);

    if (enableDebugging)
        mono_debug_domain_create(rootDomain);

    mono_thread_set_main(mono_thread_current());
}

void ScriptEngine::shutdownMono() {
    mono_domain_set(mono_get_root_domain(), false);

    if (appDomain) {
        mono_domain_unload(appDomain);
        appDomain = nullptr;
    }

    if (rootDomain) {
        mono_jit_cleanup(rootDomain);
        rootDomain = nullptr;
    }
}

bool ScriptEngine::loadCoreAssembly(const fs::path& filepath) {
    // Create an App Domain
    char name[] = "FusionScriptRuntime";
    appDomain = mono_domain_create_appdomain(name, nullptr);
    mono_domain_set(appDomain, true);

    //coreAssemblyFilepath = filepath;
    coreAssembly = Utils::LoadMonoAssembly(filepath, enableDebugging);
    if (coreAssembly == nullptr)
        return false;

    coreAssemblyImage = mono_assembly_get_image(coreAssembly);
    return true;
}

bool ScriptEngine::loadAppAssembly(const fs::path& filepath) {
    //appAssemblyFilepath = filepath;
    appAssembly = Utils::LoadMonoAssembly(filepath, enableDebugging);
    if (appAssembly == nullptr)
        return false;

    appAssemblyImage = mono_assembly_get_image(appAssembly);

    //appAssemblyFileWatcher = CreateScope<filewatch::FileWatch<std::string>>(filepath.string(), OnAppAssemblyFileSystemEvent);
    assemblyReloadPending = false;
    return true;
}

void ScriptEngine::reloadAssembly() {
    mono_domain_set(mono_get_root_domain(), false);

    if (appDomain) {
        mono_domain_unload(appDomain);
        appDomain = nullptr;
    }

    bool status = loadCoreAssembly(coreAssemblyFilepath);
    if (!status) {
        LOG_ERROR << "Could not load \"" << coreAssemblyFilepath << "\" assembly.";
        return;
    } else {
        LOG_INFO << "Assembly: \"" << coreAssemblyFilepath << "\" was loaded successfully!";
    }

    status = loadAppAssembly(appAssemblyFilepath);
    if (!status) {
        LOG_ERROR << "Could not load app \"" << appAssemblyFilepath << "\" assembly.";
        return;
    } else {
        LOG_INFO << "App assembly: \"" << appAssemblyFilepath << "\" was loaded successfully!";
    }

    loadAssemblyClasses();

    ScriptGlue::RegisterComponents();

    // Retrieve and instantiate class
    entityCoreClass FUSION_API = {"Fusion", "Entity", true};
}

void ScriptEngine::setAssemblyPaths(fs::path coreFilepath, fs::path appFilepath) {
    coreAssemblyFilepath = std::move(coreFilepath);
    appAssemblyFilepath = std::move(appFilepath);
}

void ScriptEngine::onRuntimeStart() {
    sceneContext = SceneManager::Get()->getScene();

    auto& registry = sceneContext->getRegistry();
    auto view = registry.view<ScriptComponent>();
    for (auto [entity, script] : view.each()) {
        onCreateEntity(entity, script);
    }
}

void ScriptEngine::onRuntimeStop() {
    auto& registry = sceneContext->getRegistry();
    auto view = registry.view<ScriptComponent>();
    for (auto [entity, script] : view.each()) {
        script.instance.reset();
    }

    sceneContext = nullptr;
}

void ScriptEngine::onCreateEntity(entt::entity entity, ScriptComponent& script) {
    if (!sceneContext || !sceneContext->isRuntime())
        return;

    if (auto scriptClass = getEntityClass(FUSION_API script.className)) {
        auto instance = std::make_shared<ScriptInstance>(scriptClass, entity);

        // Copy field values
        for (const auto& [name, fieldInstance] : script.fields) {
            instance->setFieldValueInternal(name, fieldInstance.buffer);
        }

        instance->invokeOnCreate();

        script.instance = instance;
    }
}

void ScriptEngine::onUpdateEntity() {
    if (!sceneContext || !sceneContext->isRuntime())
        return;

    auto& registry = sceneContext->getRegistry();
    auto view = registry.view<ScriptComponent>(entt::exclude<ActiveComponent>);
    for (auto [entity, script] : view.each()) {
        script.instance->invokeOnUpdate(Time::DeltaTime().asSeconds());
    }
}

Scene* ScriptEngine::getSceneContext() {
    return sceneContext;
}

std::shared_ptr<ScriptClass> ScriptEngine::getEntityClass(const std::string& name) {
    if (auto it = entityClasses.find(name); it != entityClasses.end())
        return it->second;
    return nullptr;
}

bool ScriptEngine::entityClassExists(const std::string& fullClassName) {
    return entityClasses.find(fullClassName) != entityClasses.end();
}

std::unordered_map<std::string, std::shared_ptr<ScriptClass>>& ScriptEngine::getEntityClasses() {
    return entityClasses;
}

void ScriptEngine::loadAssemblyClasses() {
    entityClasses.clear();

    const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(appAssemblyImage, MONO_TABLE_TYPEDEF);
    int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
    MonoClass* entityClass = mono_class_from_name(coreAssemblyImage, "Fusion", "Entity");

    for (int32_t i = 0; i < numTypes; ++i) {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char* nameSpace = mono_metadata_string_heap(appAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
        const char* className = mono_metadata_string_heap(appAssemblyImage, cols[MONO_TYPEDEF_NAME]);
        std::string fullName;
        if (strlen(nameSpace) != 0)
            fullName = nameSpace + "."s + className;
        else
            fullName = className;

        MonoClass* monoClass = mono_class_from_name(appAssemblyImage, nameSpace, className);
        if (monoClass == entityClass)
            continue;

        bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
        if (!isEntity)
            continue;

        std::shared_ptr<ScriptClass> scriptClass = std::make_shared<ScriptClass>(nameSpace, className);
        entityClasses[std::move(fullName)] = scriptClass;

        // This routine is an iterator routine for retrieving the fields in a class.
        // You must pass a gpointer that points to zero and is treated as an opaque handle
        // to iterate over all the elements. When no more values are available, the return value is NULL.

        int fieldCount = mono_class_num_fields(monoClass);
        LOG_WARNING << className << " has " << fieldCount << " fields: ";
        void* iterator = nullptr;
        while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator)) {
            const char* fieldName = mono_field_get_name(field);
            uint32_t flags = mono_field_get_flags(field);
            if (flags & MONO_FIELD_ATTR_PUBLIC) {
                MonoType* type = mono_field_get_type(field);
                ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
                LOG_WARNING << fieldName << " (" << me::enum_name(fieldType) << ")";

                scriptClass->fields[fieldName] = { fieldType, fieldName, field };
            }
        }
    }
}

MonoImage* ScriptEngine::getCoreAssemblyImage() {
    return coreAssemblyImage;
}

MonoString* ScriptEngine::createString(const char* string) {
    return mono_string_new(appDomain, string);
}

MonoObject* ScriptEngine::instantiateClass(MonoClass* monoClass) {
    MonoObject* instance = mono_object_new(appDomain, monoClass);
    mono_runtime_object_init(instance);
    return instance;
}

/*_________________________________________________*/

ScriptClass::ScriptClass(std::string _classNamespace, std::string _className, bool isCore) : classNamespace{std::move(_classNamespace)}, className{std::move(_className)} {
    monoClass = mono_class_from_name(isCore ? ScriptEngine::Get()->coreAssemblyImage : ScriptEngine::Get()->appAssemblyImage, classNamespace.c_str(), className.c_str());
}

MonoObject* ScriptClass::instantiate() {
    return ScriptEngine::Get()->instantiateClass(monoClass);
}

MonoMethod* ScriptClass::getMethod(const std::string& name, int parameterCount) {
    return mono_class_get_method_from_name(monoClass, name.c_str(), parameterCount);
}

MonoObject* ScriptClass::invokeMethod(MonoObject* instance, MonoMethod* method, void** params) {
    MonoObject* exception = nullptr;
    return mono_runtime_invoke(method, instance, params, &exception);
}

/*_________________________________________________*/

ScriptInstance::ScriptInstance(std::shared_ptr<ScriptClass> _scriptClass, entt::entity entity) : scriptClass{FUSION_API std::move(_scriptClass)FUSION_API } {
    instance = scriptClass->instantiate();

    constructor = ScriptEngine::Get()->entityCoreClass.getMethod(".ctor", 1);
    onCreateMethod = scriptClass->getMethod("OnCreate", 0);
    onUpdateMethod = scriptClass->getMethod("OnUpdate", 1);

    // Call Entity constructor
    {
        void* param = &entity;
        scriptClass->invokeMethod(instance, constructor, &param);
    }
}

void ScriptInstance::invokeOnCreate() {
    if (onCreateMethod)
        scriptClass->invokeMethod(instance, onCreateMethod);
}

void ScriptInstance::invokeOnUpdate(float ts) {
    if (onUpdateMethod) {
        void* param = &ts;
        scriptClass->invokeMethod(instance, onUpdateMethod, &param);
    }
}

bool ScriptInstance::getFieldValueInternal(const std::string& name, void* buffer) {
    const auto& fields = scriptClass->getFields();
    auto it = fields.find(name);
    if (it == fields.end())
        return false;

    const ScriptField& field = it->second;
    mono_field_get_value(instance, field.classField, buffer);
    return true;
}

bool ScriptInstance::setFieldValueInternal(const std::string& name, const void* value) {
    const auto& fields = scriptClass->getFields();
    auto it = fields.find(name);
    if (it == fields.end())
        return false;

    const ScriptField& field = it->second;
    mono_field_set_value(instance, field.classField, (void*)value);
    return true;
}

#endif