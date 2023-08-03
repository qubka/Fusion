#include "script_system.h"

#include "fusion/scene/scene_manager.h"
#include "fusion/scripting/script_engine.h"

using namespace fe;

ScriptSystem::ScriptSystem(entt::registry& registry) : System{registry} {
}

ScriptSystem::~ScriptSystem() {
}

void ScriptSystem::onPlay() {
#if FUSION_SCRIPTING
    ScriptEngine::Get()->onRuntimeStart();
#endif
}

void ScriptSystem::onUpdate() {
#if FUSION_SCRIPTING
    ScriptEngine::Get()->onUpdateEntity();
#endif
}

void ScriptSystem::onStop() {
#if FUSION_SCRIPTING
    ScriptEngine::Get()->onRuntimeStop();
#endif
}
