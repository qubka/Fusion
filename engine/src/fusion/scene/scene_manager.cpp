#include "scene_manager.hpp"

#include "fusion/utils/enumerate.hpp"

using namespace fe;

SceneManager::SceneManager() {
}

SceneManager::~SceneManager() {
    sceneIndex = 0;
    scenes.clear();
}

void SceneManager::switchScene() {
    switchScene((sceneIndex + 1) % sceneCount());
}

void SceneManager::switchScene(uint32_t id) {
    queuedSceneIndex = id;
    switchingScenes = true;
}

void SceneManager::switchScene(const std::string& name) {
    switchingScenes = true;

    uint32_t found = UINT32_MAX;

    for (const auto& [id, scene] : enumerate(scenes)) {
        if(scene->getName() == name) {
            found = id;
            break;
        }
    }

    if (found != UINT32_MAX) {
        switchScene(found);
    } else {
        LOG_ERROR << "Unknown Scene Alias : " << name;
    }
}

std::vector<const char*> SceneManager::getSceneNames() {
    std::vector<const char*> names;
    names.reserve(scenes.size());

    for(const auto& scene : scenes) {
        names.push_back(scene->getName().c_str());
    }

    return names;
}

void SceneManager::onUpdate() {
    if (switchingScenes) {
        applySceneSwitch();
    }

    if (!currentScene)
        return;

    if (!currentScene->created) {
        currentScene->onCreate();
        currentScene->created = true;
    }

    currentScene->onUpdate();
}

void SceneManager::applySceneSwitch() {
    if (switchingScenes == false) {
        if (currentScene)
            return;

        if (scenes.empty())
            scenes.push_back(std::make_unique<Scene>("NewScene"));

       queuedSceneIndex = 0;
    }

    if (queuedSceneIndex >= scenes.size()) {
        LOG_INFO << "Invalid Scene Index : " << queuedSceneIndex;
        queuedSceneIndex = 0;
    }

    sceneIndex = queuedSceneIndex;
    currentScene = scenes[queuedSceneIndex].get();

    /*std::string physicalPath;
    if ResolvePhysicalPath("//Scenes/" + m_CurrentScene->GetSceneName() + ".scene", physicalPath)) {
        auto newPath = StringUtilities::RemoveName(physicalPath);
        currentScene->deserialise(newPath, false);
    }*/

    LOG_INFO << "Scene switched to : " << currentScene->getName();

    switchingScenes = false;
}

void SceneManager::enqueueSceneFromFile(const std::filesystem::path& filename) {
    sceneFilePaths.push_back(filename);

    auto name = filename.filename().replace_extension().string();
    enqueueScene(name);
}

void SceneManager::enqueueScene(std::unique_ptr<Scene>&& scene) {
    const auto& self = scenes.emplace_back(std::move(scene));
    LOG_INFO << "Enqueued scene : " << self->getName();
}

void SceneManager::enqueueScene(const std::string& name) {
    const auto& self = scenes.emplace_back(std::make_unique<Scene>(name));
    LOG_INFO << "Enqueued scene : " << self->getName();
}

void SceneManager::addFileToLoadList(const std::filesystem::path& filename) {
    sceneFilePathsToLoad.push_back(filename);
}

void SceneManager::loadCurrentList() {
    for (auto& filename : sceneFilePathsToLoad) {
        enqueueSceneFromFile(filename);
    }

    sceneFilePathsToLoad.clear();
}


