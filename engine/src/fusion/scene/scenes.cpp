#include "scenes.hpp"

using namespace fe;

void Scenes::onUpdate() {
    if (!scene)
        return;

    if (!scene->started) {
        scene->onStart();
        scene->started = true;
    }

    scene->onUpdate();
}
