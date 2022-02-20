#pragma once

int main(int args, char** argv) {
    Fusion::Log::Init();
    FE_LOG_INFO << "Fusion Engine";
    FE_LOG_INFO << "Game Started";

    auto app = Fusion::CreateApplication({args, argv});
    app->run();
    delete app;

    return EXIT_SUCCESS;
}