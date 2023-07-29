#include "script_glue.h"
#include "script_engine.h"

#include "fusion/scene/components.h"
#include "fusion/scene/scene.h"
#include "fusion/input/input.h"

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>

using namespace fe;

static std::unordered_map<MonoType*, std::function<bool(entt::registry&, entt::entity)>> EntityHasComponentFuncs;

#define ADD_INTERNAL_CALL(Name) mono_add_internal_call("Fusion.InternalCalls::" #Name, (const void*)&(Name))

namespace Utils {
    std::string MonoStringToString(MonoString* string) {
        char* cStr = mono_string_to_utf8(string);
        std::string str{cStr};
        mono_free(cStr);
        return str;
    }
}

static void NativeLog(MonoString* string, int parameter) {
    std::string str = Utils::MonoStringToString(string);
    LOG_WARNING << str << ", " << parameter;
}

static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult) {
    LOG_WARNING << "Value: " << glm::to_string(*parameter);
    *outResult = glm::normalize(*parameter);
}

static float NativeLog_VectorDot(glm::vec3* parameter) {
    LOG_WARNING << "Value: " << glm::to_string(*parameter);
    return glm::dot(*parameter, *parameter);
}

static MonoObject* GetScriptInstance(uint32_t entityID) {
    auto entity = static_cast<entt::entity>(entityID);
    return ScriptEngine::Get()->getManagedInstance(entity);
}

static bool Entity_HasComponent(uint32_t entityID, MonoReflectionType* componentType) {
    auto scene = ScriptEngine::Get()->getSceneContext();
    assert(scene);
    auto entity = static_cast<entt::entity>(entityID);
    if (!scene->isEntityValid(entity))
        return false;

    MonoType* managedType = mono_reflection_type_get_type(componentType);
    auto it = EntityHasComponentFuncs.find(managedType);
    assert(it != EntityHasComponentFuncs.end());
    return it->second(scene->getRegistry(), entity);
}

static uint32_t Entity_FindEntityByName(MonoString* name) {
    char* nameCStr = mono_string_to_utf8(name);

    auto scene = ScriptEngine::Get()->getSceneContext();
    assert(scene);
    auto entity = scene->getEntityByName(nameCStr);
    mono_free(nameCStr);

    using type = std::underlying_type_t<entt::entity>;
    static_assert(std::is_same_v<type, uint32_t>, "Wrong entity id type");
    return static_cast<type>(entity);
}

static void TransformComponent_GetTranslation(uint32_t entityID, glm::vec3* outTranslation) {
    auto scene = ScriptEngine::Get()->getSceneContext();
    assert(scene);
    auto entity = static_cast<entt::entity>(entityID);
    if (!scene->isEntityValid(entity))
        return;

    auto& registry = scene->getRegistry();
    *outTranslation = registry.get<TransformComponent>(entity).getLocalPosition();
}

static void TransformComponent_SetTranslation(uint32_t entityID, glm::vec3* translation) {
    auto scene = ScriptEngine::Get()->getSceneContext();
    assert(scene);
    auto entity = static_cast<entt::entity>(entityID);
    if (!scene->isEntityValid(entity))
        return;

    auto& registry = scene->getRegistry();
    registry.get<TransformComponent>(entity).setLocalPosition(*translation);
}

static bool Input_IsKeyDown(Key key) {
    auto input = Input::Get();
    return input->getKeyDown(key);
}

template<typename... Component>
static void RegisterComponent() {
    ([]() {
        std::string_view typeName = typeid(Component).name();
        size_t pos = typeName.find_last_of(':');
        std::string_view structName = typeName.substr(pos + 1);
        std::string managedTypename{ "Fusion." + std::string{structName} };

        MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::Get()->getCoreAssemblyImage());
        if (!managedType) {
            LOG_ERROR << "Could not find component type " << managedTypename;
            return;
        }
        EntityHasComponentFuncs[managedType] = [](entt::registry& registry, entt::entity entity) { return registry.try_get<Component>(entity) != nullptr; };
    }(), ...);
}

template<typename... Component>
static void RegisterComponent(ComponentGroup<Component...>) {
    RegisterComponent<Component...>();
}

void ScriptGlue::RegisterComponents() {
    EntityHasComponentFuncs.clear();
    RegisterComponent(AllComponents{});
}

void ScriptGlue::RegisterFunctions() {
    ADD_INTERNAL_CALL(NativeLog);
    ADD_INTERNAL_CALL(NativeLog_Vector);
    ADD_INTERNAL_CALL(NativeLog_VectorDot);

    ADD_INTERNAL_CALL(GetScriptInstance);

    ADD_INTERNAL_CALL(Entity_HasComponent);
    ADD_INTERNAL_CALL(Entity_FindEntityByName);

    ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
    ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

    ADD_INTERNAL_CALL(Input_IsKeyDown);
}
