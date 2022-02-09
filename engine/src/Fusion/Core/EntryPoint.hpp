#pragma once

//Fusion::Application* Fusion::CreateApplication();

int main(int args, char** argv) {
    Fusion::Log::Init();
    FS_LOG_CORE_INFO("Fusion Engine");

    auto app = Fusion::CreateApplication({args, argv});
    app->run();
    delete app;

    return EXIT_SUCCESS;
}