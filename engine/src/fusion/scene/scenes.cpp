#include "scenes.hpp"
#include "scene.hpp"

using namespace fe;

void Scenes::update() {
    if (!scene)
        return;

    if (!scene->started) {
        scene->start();
        scene->started = true;
    }

    scene->update();
}
