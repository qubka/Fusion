#include "fusion/core/engine.hpp"

#include "editor_app.hpp"

int main(int args, char** argv) {
    using namespace fe;

    // Creates the engine.
    Engine engine{CommandLineArgs{args, argv}};

    // Sets the application to the engine.
    engine.setApp(std::make_unique<EditorApp>("Editor Application", Version{1, 1, 1}));

    // Runs the game loop.
    auto exitCode = engine.run();

    // Pauses the console.
    //std::cout << "Press enter to continue...";
    //std::cin.get();
    return exitCode;
}