#pragma once

extern Fusion::Application* Fusion::CreateApplication();

int main(int args, char** argv) {
    Fusion::Log::init();
    Fusion::Log::getCoreLogger()->warn("Initialized Log");
    Fusion::Log::getClientLogger()->warn("Initialized Log");

    printf("Fusion Engine\n");
    auto app = Fusion::CreateApplication();
    app->run();
    delete app;
}