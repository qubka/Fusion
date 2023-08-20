#pragma once

namespace fe {
    class Ray;
    class Frustum;
    class AABB;
    class Sphere;
    class LightComponent;

    template<typename T>
    class Module;

    class FUSION_API DebugRenderer {
        friend class Module<DebugRenderer>;
    private:
        DebugRenderer();
        ~DebugRenderer();

    public:
        static DebugRenderer* Get() { return Instance; }

        // Draw Point (circle)
        static void DrawPoint(const glm::vec3& pos, float pointRadius, const glm::vec3& color);
        static void DrawPoint(const glm::vec3& pos, float pointRadius, const glm::vec4& color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
        static void DrawPointNDT(const glm::vec3& pos, float pointRadius, const glm::vec3& color);
        static void DrawPointNDT(const glm::vec3& pos, float pointRadius, const glm::vec4& color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});

        // Draw Line with a given thickness
        static void DrawThickLine(const glm::vec3& start, const glm::vec3& end, float lineWidth, const glm::vec3& color);
        static void DrawThickLine(const glm::vec3& start, const glm::vec3& end, float lineWidth, const glm::vec4& color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
        static void DrawThickLineNDT(const glm::vec3& start, const glm::vec3& end, float lineWidth, const glm::vec3& color);
        static void DrawThickLineNDT(const glm::vec3& start, const glm::vec3& end, float lineWidth, const glm::vec4& color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});

        // Draw line with thickness of 1 screen pixel regardless of distance from camera
        static void DrawHairLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);
        static void DrawHairLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
        static void DrawHairLineNDT(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);
        static void DrawHairLineNDT(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});

        // Draw Matrix (x,y,z axis at pos)
        static void DrawMatrix(const glm::mat4& transform);
        static void DrawMatrix(const glm::mat3& rotation, const glm::vec3& position);
        static void DrawMatrixNDT(const glm::mat4& transform);
        static void DrawMatrixNDT(const glm::mat3& rotation, const glm::vec3& position);

        // Draw Triangle
        static void DrawTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
        static void DrawTriangleNDT(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});

        // Draw Polygon (Renders as a triangle fan, so verts must be arranged in order)
        static void DrawPolygon(gsl::span<const glm::vec3> verts, const glm::vec4& color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
        static void DrawPolygonNDT(gsl::span<const glm::vec3> verts, const glm::vec4& color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});

        static void DebugDraw(const AABB& box, const glm::vec4& edgeColour, bool cornersOnly = false, float width = 0.02f);
        static void DebugDraw(const Sphere& sphere, const glm::vec4& color);
        static void DebugDraw(const Frustum& frustum, const glm::vec4& color);
        static void DebugDraw(const LightComponent& light, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& direction, const glm::vec4& color);
        static void DebugDraw(const Ray& ray, const glm::vec4& color = glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, float distance = 1000.0f);
        //static void DebugDraw(SoundNode* sound, const glm::vec4& color);
        static void DebugDrawSphere(float radius, const glm::vec3& position, const glm::vec4& color);
        static void DebugDrawCircle(int numVerts, float radius, const glm::vec3& position, const glm::quat& rotation, const glm::vec4& color);
        static void DebugDrawCone(int numCircleVerts, int numLinesToCircle, float angle, float length, const glm::vec3& position, const glm::quat& rotation, const glm::vec4& color);
        static void DebugDrawCapsule(const glm::vec3& position, const glm::quat& rotation, float height, float radius, const glm::vec4& color);

        struct DrawVertex {
            glm::vec3 pos;
            uint32_t color;

            DrawVertex(const glm::vec3& pos, const glm::vec4& color)
                : pos{pos}, color{glm::rgbaColor(color)} {
            }

            bool operator==(const DrawVertex& rhs) const {
                return pos == rhs.pos && color == rhs.color;
            }

            bool operator!=(const DrawVertex& rhs) const {
                return !operator==(rhs);
            }
        };

        struct DrawSpatialVertex {
            glm::vec3 pos;
            uint32_t color;
            float size;

            DrawSpatialVertex(const glm::vec3& pos, const glm::vec4& color, float size)
                : pos{pos}, color{glm::rgbaColor(color)}, size{size} {}

            bool operator==(const DrawSpatialVertex& rhs) const {
                return pos == rhs.pos && color == rhs.color && size == rhs.size;
            }

            bool operator!=(const DrawSpatialVertex& rhs) const {
                return !operator==(rhs);
            }
        };

        const std::vector<DrawVertex>& getTriangles(bool depthTested = false) const { return drawLists[!depthTested].triangles; }
        const std::vector<DrawVertex>& getLines(bool depthTested = false) const { return drawLists[!depthTested].lines; }
        const std::vector<DrawVertex>& getThickLines(bool depthTested = false) const { return drawLists[!depthTested].thickLines; }
        const std::vector<DrawSpatialVertex>& getPoints(bool depthTested = false) const { return drawLists[!depthTested].points; }

    private:
        void onStart();
        void onUpdate();
        void onStop();

        template<bool NDT>
        static void GenDrawPoint(const glm::vec3& pos, float pointRadius, const glm::vec4& color) {
            auto& points = Instance->drawLists[NDT].points;
            points.emplace_back(pos, color, pointRadius);
            points.emplace_back(pos, color, pointRadius);
        }
        template<bool NDT>
        static void GenDrawThickLine(const glm::vec3& start, const glm::vec3& end, float lineWidth, const glm::vec4& color) {
            auto& thickLines = Instance->drawLists[NDT].thickLines;
            thickLines.emplace_back(start, color);
            thickLines.emplace_back(end, color);
        }
        template<bool NDT>
        static void GenDrawHairLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color) {
            auto& lines = Instance->drawLists[NDT].lines;
            lines.emplace_back(start, color);
            lines.emplace_back(end, color);
        }

        template<bool NDT>
        static void GenDrawTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& color) {
            auto& triangles = Instance->drawLists[NDT].triangles;
            triangles.emplace_back(v0, color);
            triangles.emplace_back(v1, color);
            triangles.emplace_back(v2, color);
        }

        struct DrawList {
            std::vector<DrawVertex> triangles;
            std::vector<DrawVertex> lines;
            std::vector<DrawVertex> thickLines;
            std::vector<DrawSpatialVertex> points;

            void clear() {
                triangles.clear();
                lines.clear();
                thickLines.clear();
                points.clear();
            }
        };

        std::array<DrawList, 2> drawLists;

        static DebugRenderer* Instance;
    };
}
