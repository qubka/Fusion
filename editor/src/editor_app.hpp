#pragma once

#include "fusion/core/app.hpp"

namespace fe {
    class EditorApp : public App {
    public:
        EditorApp(const std::string& name, const Version& version);
        ~EditorApp() override;

        void onStart() override;
        void onUpdate(const Time& dt) override;

    private:

    };
}
