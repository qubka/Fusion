#include "fusion/core/engine.hpp"

#include "editor_app.hpp"

int main(int args, char** argv) {
    // Creates the engine.
    auto engine = std::make_unique<fe::Engine>(fe::CommandLineArgs{args, argv});

    // Sets the application to the engine.
    engine->setApp(std::make_unique<fe::EditorApp>("Editor Application", fe::Version{0, 0, 0}));

    // Runs the game loop.
    auto exitCode = engine->run();
    engine = nullptr;

    // Pauses the console.
    std::cout << "Press enter to continue...";
    std::cin.get();
    return exitCode;
}