#include "script_system.h"

#include "fusion/scene/scene_manager.h"
#include "fusion/scripting/script_engine.h"

using namespace fe;

ScriptSystem::ScriptSystem(entt::registry& registry) : System{registry} {
}

ScriptSystem::~ScriptSystem() {
}

void ScriptSystem::onPlay() {
    ScriptEngine::Get()->onRuntimeStart();
}

void ScriptSystem::onUpdate() {
    ScriptEngine::Get()->onUpdateEntity();
}

void ScriptSystem::onStop() {
    ScriptEngine::Get()->onRuntimeStop();
}
