#pragma once

int main(int args, char** argv) {
    fe::Log::Init();
    LOG_INFO << "Fusion Engine";
    LOG_INFO << "Game Started";

    auto app = fe::CreateApplication({args, argv});
    app->run();
    delete app;

    return EXIT_SUCCESS;
}