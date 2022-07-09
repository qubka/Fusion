#include "fusion/core/engine.hpp"

#include "editor.hpp"

int main(int args, char** argv) {
    using namespace fe;

    // Creates the engine
    Engine engine{{args, argv}};

    // Sets the application to the engine
    engine.setApp(std::make_unique<Editor>("Editor Application"));

    // Runs the game loop
    return engine.run();
}