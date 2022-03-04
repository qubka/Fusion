#pragma once

namespace fe {
    class BaseRendererSystem {
    public:
        virtual void beginScene() = 0;
        virtual void endScene() = 0;

        static void ResetStats();
        static size_t drawCalls;
        static size_t totalCalls;
        static size_t totalVertices;
    };
}
