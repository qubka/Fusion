#pragma once

#include "fusion/graphics/pipelines/vertex.hpp"

namespace fe {
    class Mesh;
    class MeshFactory {
    public:
        enum class PrimitiveType : uint8_t {
            Plane = 0,
            Quad = 1,
            Cube = 2,
            Pyramid = 3,
            Sphere = 4,
            Capsule = 5,
            Cylinder = 6,
            Terrain = 7,
            File = 8,
            None = 9
        };

        /*Mesh* CreateQuad();
        Mesh* CreateQuad(float x, float y, float width, float height);
        Mesh* CreateQuad(const glm::vec2& position, const glm::vec2& size);

        Mesh* CreatePyramid();
        Mesh* CreatePlane(float width, float height, const glm::vec3& normal);

        static std::shared_ptr<Mesh> CreateCylinder(const Vertex::Layout& layout, float bottomRadius = 0.5f, float topRadius = 0.5f, float height = 1.0f, uint32_t radialSegments = 64, uint32_t rings = 8);
        static std::shared_ptr<Mesh> CreateCapsule(const Vertex::Layout& layout, float radius = 0.5f, float midHeight = 2.0f, uint32_t radialSegments = 64, uint32_t rings = 8);

        static std::shared_ptr<Mesh> CreateCuboid(const Vertex::Layout& layout, const glm::vec3& halfExtents = {1.0f, 1.0f, 1.0f}, bool inwards = false);
        static std::shared_ptr<Mesh> CreateSphere(const Vertex::Layout& layout, uint32_t stacks = 64, uint32_t slices = 64, float radius = 5.0f);
        //static std::shared_ptr<Mesh> CreateQuad(const Vertex::Layout& layout, const glm::vec2& extent = {1.0f, 1.0f});
        static std::shared_ptr<Mesh> CreateOctahedron(const Vertex::Layout& layout, const glm::vec3& extent = {1.0f, 2.0f, 1.0f});
        static std::shared_ptr<Mesh> CreateTetrahedron(const Vertex::Layout& layout, const glm::vec3& extent = {1.0f, 3.0f, 1.0f});
        static std::shared_ptr<Mesh> CreateTorus(const Vertex::Layout& layout, uint32_t sides = 24, uint32_t outerRides = 72, float radius = 35.0f, float outerRadius = 7.5f);*/
    };
}
