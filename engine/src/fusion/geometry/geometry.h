#pragma once

namespace fe {
    class Plane;

    class FUSION_API Geometry {
    public:
        static glm::vec3 IntersectionPoint(const Plane& a, const Plane& b, const Plane& c);
    };
}