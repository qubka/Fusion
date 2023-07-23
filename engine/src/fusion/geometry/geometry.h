#pragma once

namespace fe {
    class Plane;

    class Geometry {
    public:
        static glm::vec3 IntersectionPoint(const Plane& a, const Plane& b, const Plane& c);
    };
}