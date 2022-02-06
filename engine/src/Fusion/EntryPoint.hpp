#pragma once

extern Fusion::Application* Fusion::CreateApplication();

int main(int args, char** argv) {
    Fusion::Log::init();
    FS_LOG_CORE_INFO("Initialized Log");
    FS_LOG_TRACE("Initialized Log");

    printf("Fusion Engine\n");
    auto app = Fusion::CreateApplication();
    app->run();
    delete app;
}