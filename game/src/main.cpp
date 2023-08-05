#include "platform/pc/pc_engine.h"

#include "sandbox.h"

int main(int args, char** argv) {
    using namespace fe;

    // Creates the engine
    pc::Engine engine{{args, argv}};

    // Sets the application to the engine
    engine.setApp(std::make_unique<Sandbox>("Sandbox Application"));

    // Runs the game loop
    auto result = engine.run();
    return result;
}