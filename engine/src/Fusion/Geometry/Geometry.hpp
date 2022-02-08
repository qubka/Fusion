#pragma once

#include "Fusion/Core/Base.hpp"

namespace Fusion {
    class Plane;

    class FUSION_API Geometry {
    public:
        static glm::vec3 intersectionPoint(const Plane& a, const Plane& b, const Plane& c);
    };
}