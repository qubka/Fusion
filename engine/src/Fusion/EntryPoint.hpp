#pragma once

extern Fusion::Application* Fusion::CreateApplication();

int main(int args, char** argv) {
    printf("Fusion Engine\n");
    auto app = Fusion::CreateApplication();
    app->run();
    delete app;
}