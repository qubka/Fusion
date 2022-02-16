#pragma once

namespace Fusion {
	class FUSION_API RendererSystemBase {
	public:
        virtual ~RendererSystemBase() = default;
        FE_NONCOPYABLE(RendererSystemBase);

		virtual void beginScene() = 0;
		virtual void endScene() = 0;

		static void ResetStats();
		static size_t drawCalls;
		static size_t totalCalls;
		static size_t totalVertices;
	};
}
