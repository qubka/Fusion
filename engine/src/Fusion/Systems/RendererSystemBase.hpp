#pragma once

namespace Fusion {
    class Camera;
	class FUSION_API RendererSystemBase {
	public:
        virtual ~RendererSystemBase() = default;

		virtual void beginScene(const Camera& camera) = 0;
		virtual void endScene() = 0;
		virtual void flush() = 0;

		static void ResetStats();
		static size_t drawCalls;
		static size_t totalCalls;
		static size_t totalVertices;
	};
}
