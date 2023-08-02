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
    auto scene = ScriptEngine::Get()->getSceneContext();
    assert(scene);
    auto entity = static_cast<entt::entity>(entityID);
    if (auto scriptComponent = scene->getRegistry().try_get<ScriptComponent>(entity)) {
        auto& scriptInstance = scriptComponent->instance;
        return scriptInstance ? scriptInstance->getManagedObject() : nullptr;
    }
    return nullptr;
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

static void TransformComponent_GetPosition(uint32_t entityID, glm::vec3* outPosition) {
    auto scene = ScriptEngine::Get()->getSceneContext();
    assert(scene);
    auto entity = static_cast<entt::entity>(entityID);
    if (!scene->isEntityValid(entity))
        return;

    auto& registry = scene->getRegistry();
    *outPosition = registry.get<TransformComponent>(entity).getLocalPosition();
}

static void TransformComponent_SetPosition(uint32_t entityID, glm::vec3* position) {
    auto scene = ScriptEngine::Get()->getSceneContext();
    assert(scene);
    auto entity = static_cast<entt::entity>(entityID);
    if (!scene->isEntityValid(entity))
        return;

    auto& registry = scene->getRegistry();
    registry.get<TransformComponent>(entity).setLocalPosition(*position);
}

#if GLM_FORCE_QUAT_DATA_XYZW
static void TransformComponent_GetRotation(uint32_t entityID, glm::quat* outRotation) {
    auto scene = ScriptEngine::Get()->getSceneContext();
    assert(scene);
    auto entity = static_cast<entt::entity>(entityID);
    if (!scene->isEntityValid(entity))
        return;

    auto& registry = scene->getRegistry();
    *outRotation = registry.get<TransformComponent>(entity).getLocalOrientation();
}

static void TransformComponent_SetRotation(uint32_t entityID, glm::quat* rotation) {
    auto scene = ScriptEngine::Get()->getSceneContext();
    assert(scene);
    auto entity = static_cast<entt::entity>(entityID);
    if (!scene->isEntityValid(entity))
        return;

    auto& registry = scene->getRegistry();
    registry.get<TransformComponent>(entity).setLocalOrientation(*rotation);
}
#else
static void TransformComponent_GetRotation(uint32_t entityID, glm::vec4* outRotation) {
    auto scene = ScriptEngine::Get()->getSceneContext();
    assert(scene);
    auto entity = static_cast<entt::entity>(entityID);
    if (!scene->isEntityValid(entity))
        return;

    auto& registry = scene->getRegistry();
    auto& rotation = registry.get<TransformComponent>(entity).getLocalOrientation();
    *outRotation = { rotation.x, rotation.y, rotation.z, rotation.w };
}

static void TransformComponent_SetRotation(uint32_t entityID, glm::vec4* rotation) {
    auto scene = ScriptEngine::Get()->getSceneContext();
    assert(scene);
    auto entity = static_cast<entt::entity>(entityID);
    if (!scene->isEntityValid(entity))
        return;

    auto& registry = scene->getRegistry();
    registry.get<TransformComponent>(entity).setLocalOrientation(glm::quat{ rotation->w, rotation->x, rotation->y, rotation->z });
}
#endif

static void TransformComponent_GetScale(uint32_t entityID, glm::vec3* outScale) {
    auto scene = ScriptEngine::Get()->getSceneContext();
    assert(scene);
    auto entity = static_cast<entt::entity>(entityID);
    if (!scene->isEntityValid(entity))
        return;

    auto& registry = scene->getRegistry();
    *outScale = registry.get<TransformComponent>(entity).getLocalScale();
}

static void TransformComponent_SetScale(uint32_t entityID, glm::vec3* scale) {
    auto scene = ScriptEngine::Get()->getSceneContext();
    assert(scene);
    auto entity = static_cast<entt::entity>(entityID);
    if (!scene->isEntityValid(entity))
        return;

    auto& registry = scene->getRegistry();
    registry.get<TransformComponent>(entity).setLocalScale(*scale);
}

static bool Input_IsKeyDown(Key key) {
    auto input = Input::Get();
    return input->getKeyDown(key);
}

template<typename... Component>
static void RegisterComponent() {
    ([]() {
        std::string typeName{ String::Demangle(typeid(Component()).name()) };
        size_t pos = typeName.find_last_of(':');
        std::string managedTypename{ "Fusion." + (pos != std::string::npos ? typeName.substr(pos + 1) : typeName) };

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

    ADD_INTERNAL_CALL(TransformComponent_GetPosition);
    ADD_INTERNAL_CALL(TransformComponent_SetPosition);
    ADD_INTERNAL_CALL(TransformComponent_GetRotation);
    ADD_INTERNAL_CALL(TransformComponent_SetRotation);
    ADD_INTERNAL_CALL(TransformComponent_GetScale);
    ADD_INTERNAL_CALL(TransformComponent_SetScale);

    ADD_INTERNAL_CALL(Input_IsKeyDown);
}
