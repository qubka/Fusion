#pragma once

namespace Fusion {
    class window {
    public:
        window();
        ~window();
        window(const window&) = delete;
        window(window&&) = delete;
        window& operator=(const window&) = delete;
        window& operator=(window&&) = delete;
    private:
    };
}
