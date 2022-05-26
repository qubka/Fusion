#pragma once

int main(int args, char** argv) {
    fe::Log::Init();
    LOG_INFO << "Fusion Engine Started";

    auto app = fe::CreateApplication(fe::CommandLineArgs{args, argv});
    app->run();
    delete app;

    return EXIT_SUCCESS;
}