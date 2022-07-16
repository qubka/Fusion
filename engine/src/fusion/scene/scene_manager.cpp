#include "scene_manager.hpp"

#include "fusion/devices/device_manager.hpp"

using namespace fe;

SceneManager::~SceneManager() {
    //if (started) DeviceManager::Get()->getWindow(0)->OnResize().connect<&SceneManager::onWindowResize>(this);
}

void SceneManager::onStart() {
    DeviceManager::Get()->getWindow(0)->OnResize().connect<&SceneManager::onWindowResize>(this);
    // TODO: Delete hook ?
}

void SceneManager::onUpdate() {
    if (!scene)
        return;

    if (!scene->started) {
        scene->onStart();
        scene->started = true;
    }

    scene->onUpdate();
}

void SceneManager::onWindowResize(const glm::uvec2& size) {
    if (scene) {

    }
}
