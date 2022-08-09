#include "debug_renderer.hpp"

#include "fusion/geometry/aabb.hpp"
#include "fusion/geometry/frustum.hpp"
#include "fusion/geometry/sphere.hpp"
#include "fusion/geometry/ray.hpp"

using namespace fe;

// Draw Point (circle)
void DebugRenderer::GenDrawPoint(bool ndt, const glm::vec3& pos, float pointRadius, const glm::vec4& color) {
    auto& points = ndt ? ModuleInstance->drawListNDT.points : ModuleInstance->drawList.points;
    points.emplace_back(pos, color, pointRadius);
    points.emplace_back(pos, color, pointRadius);
}

void DebugRenderer::DrawPoint(const glm::vec3& pos, float pointRadius, const glm::vec3& color) {
    GenDrawPoint(false, pos, pointRadius, glm::vec4{color, 1.0f});
}

void DebugRenderer::DrawPoint(const glm::vec3& pos, float pointRadius, const glm::vec4& color) {
    GenDrawPoint(false, pos, pointRadius, color);
}

void DebugRenderer::DrawPointNDT(const glm::vec3& pos, float pointRadius, const glm::vec3& color) {
    GenDrawPoint(true, pos, pointRadius, glm::vec4{color, 1.0f});
}

void DebugRenderer::DrawPointNDT(const glm::vec3& pos, float pointRadius, const glm::vec4& color) {
    GenDrawPoint(true, pos, pointRadius, color);
}

// Draw Line with a given thickness
void DebugRenderer::GenDrawThickLine(bool ndt, const glm::vec3& start, const glm::vec3& end, float lineWidth, const glm::vec4& color) {
    auto& thickLines = ndt ? ModuleInstance->drawListNDT.thickLines : ModuleInstance->drawList.thickLines;
    thickLines.emplace_back(start, color);
    thickLines.emplace_back(end, color);
}

void DebugRenderer::DrawThickLine(const glm::vec3& start, const glm::vec3& end, float lineWidth, const glm::vec3& color) {
    GenDrawThickLine(false, start, end, lineWidth, glm::vec4{color, 1.0f});
}

void DebugRenderer::DrawThickLine(const glm::vec3& start, const glm::vec3& end, float lineWidth, const glm::vec4& color) {
    GenDrawThickLine(false, start, end, lineWidth, color);
}

void DebugRenderer::DrawThickLineNDT(const glm::vec3& start, const glm::vec3& end, float lineWidth, const glm::vec3& color) {
    GenDrawThickLine(true, start, end, lineWidth, glm::vec4{color, 1.0f});
}

void DebugRenderer::DrawThickLineNDT(const glm::vec3& start, const glm::vec3& end, float lineWidth, const glm::vec4& color) {
    GenDrawThickLine(true, start, end, lineWidth, color);
}

// Draw line with thickness of 1 screen pixel regardless of distance from camera
void DebugRenderer::GenDrawHairLine(bool ndt, const glm::vec3& start, const glm::vec3& end, const glm::vec4& color) {
    auto& lines = ndt ? ModuleInstance->drawListNDT.lines : ModuleInstance->drawList.lines;
    lines.emplace_back(start, color);
    lines.emplace_back(end, color);
}

void DebugRenderer::DrawHairLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color) {
    GenDrawHairLine(false, start, end, glm::vec4{color, 1.0f});
}

void DebugRenderer::DrawHairLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color) {
    GenDrawHairLine(false, start, end, color);
}

void DebugRenderer::DrawHairLineNDT(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color) {
    GenDrawHairLine(true, start, end, glm::vec4{color, 1.0f});
}

void DebugRenderer::DrawHairLineNDT(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color) {
    GenDrawHairLine(true, start, end, color);
}

// Draw Matrix (x,y,z axis at pos)
void DebugRenderer::DrawMatrix(const glm::mat4& mtx) {
    glm::vec3 position{ mtx[3] };
    auto m = glm::value_ptr(mtx);
    GenDrawHairLine(false, position, position + glm::vec3{m[0], m[1], m[2]}, glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
    GenDrawHairLine(false, position, position + glm::vec3{m[4], m[5], m[6]}, glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
    GenDrawHairLine(false, position, position + glm::vec3{m[8], m[9], m[10]}, glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});
}

void DebugRenderer::DrawMatrix(const glm::mat3& mtx, const glm::vec3& position) {
    GenDrawHairLine(false, position, position + mtx[0], glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
    GenDrawHairLine(false, position, position + mtx[1], glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
    GenDrawHairLine(false, position, position + mtx[2], glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});
}

void DebugRenderer::DrawMatrixNDT(const glm::mat4& mtx) {
    glm::vec3 position{ mtx[3] };
    auto m = glm::value_ptr(mtx);
    GenDrawHairLine(true, position, position + glm::vec3{m[0], m[1], m[2]}, glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
    GenDrawHairLine(true, position, position + glm::vec3{m[4], m[5], m[6]}, glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
    GenDrawHairLine(true, position, position + glm::vec3{m[8], m[9], m[10]}, glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});
}

void DebugRenderer::DrawMatrixNDT(const glm::mat3& mtx, const glm::vec3& position) {
    GenDrawHairLine(true, position, position + mtx[0], glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
    GenDrawHairLine(true, position, position + mtx[1], glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
    GenDrawHairLine(true, position, position + mtx[2], glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});
}

// Draw Triangle
void DebugRenderer::GenDrawTriangle(bool ndt, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& color) {
    auto& triangles = ndt ? ModuleInstance->drawListNDT.triangles : ModuleInstance->drawList.triangles;
    triangles.emplace_back(v0, color);
    triangles.emplace_back(v1, color);
    triangles.emplace_back(v2, color);
}

void DebugRenderer::DrawTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& color) {
    GenDrawTriangle(false, v0, v1, v2, color);
}

void DebugRenderer::DrawTriangleNDT(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& color) {
    GenDrawTriangle(true, v0, v1, v2, color);
}

// Draw Polygon (Renders as a triangle fan, so verts must be arranged in order)
void DebugRenderer::DrawPolygon(std::span<const glm::vec3> verts, const glm::vec4& color) {
    for (int i = 2; i < verts.size(); ++i) {
        GenDrawTriangle(false, verts[0], verts[i - 1], verts[i], color);
    }
}

void DebugRenderer::DrawPolygonNDT(std::span<const glm::vec3> verts, const glm::vec4& color) {
    for (int i = 2; i < verts.size(); ++i) {
        GenDrawTriangle(true, verts[0], verts[i - 1], verts[i], color);
    }
}

void DebugRenderer::DebugDraw(const AABB& box, const glm::vec4& edgeColour, bool cornersOnly, float width) {
    glm::vec3 uuu {box.getMax()};
    glm::vec3 lll {box.getMin()};

    glm::vec3 ull{uuu.x, lll.y, lll.z};
    glm::vec3 uul{uuu.x, uuu.y, lll.z};
    glm::vec3 ulu{uuu.x, lll.y, uuu.z};

    glm::vec3 luu{lll.x, uuu.y, uuu.z};
    glm::vec3 llu{lll.x, lll.y, uuu.z};
    glm::vec3 lul{lll.x, uuu.y, lll.z};

    // Draw edges
    if (!cornersOnly) {
        DrawThickLineNDT(luu, uuu, width, edgeColour);
        DrawThickLineNDT(lul, uul, width, edgeColour);
        DrawThickLineNDT(llu, ulu, width, edgeColour);
        DrawThickLineNDT(lll, ull, width, edgeColour);

        DrawThickLineNDT(lul, lll, width, edgeColour);
        DrawThickLineNDT(uul, ull, width, edgeColour);
        DrawThickLineNDT(luu, llu, width, edgeColour);
        DrawThickLineNDT(uuu, ulu, width, edgeColour);

        DrawThickLineNDT(lll, llu, width, edgeColour);
        DrawThickLineNDT(ull, ulu, width, edgeColour);
        DrawThickLineNDT(lul, luu, width, edgeColour);
        DrawThickLineNDT(uul, uuu, width, edgeColour);
    } else {
        DrawThickLineNDT(luu, luu + (uuu - luu) * 0.25f, width, edgeColour);
        DrawThickLineNDT(luu + (uuu - luu) * 0.75f, uuu, width, edgeColour);

        DrawThickLineNDT(lul, lul + (uul - lul) * 0.25f, width, edgeColour);
        DrawThickLineNDT(lul + (uul - lul) * 0.75f, uul, width, edgeColour);

        DrawThickLineNDT(llu, llu + (ulu - llu) * 0.25f, width, edgeColour);
        DrawThickLineNDT(llu + (ulu - llu) * 0.75f, ulu, width, edgeColour);

        DrawThickLineNDT(lll, lll + (ull - lll) * 0.25f, width, edgeColour);
        DrawThickLineNDT(lll + (ull - lll) * 0.75f, ull, width, edgeColour);

        DrawThickLineNDT(lul, lul + (lll - lul) * 0.25f, width, edgeColour);
        DrawThickLineNDT(lul + (lll - lul) * 0.75f, lll, width, edgeColour);

        DrawThickLineNDT(uul, uul + (ull - uul) * 0.25f, width, edgeColour);
        DrawThickLineNDT(uul + (ull - uul) * 0.75f, ull, width, edgeColour);

        DrawThickLineNDT(luu, luu + (llu - luu) * 0.25f, width, edgeColour);
        DrawThickLineNDT(luu + (llu - luu) * 0.75f, llu, width, edgeColour);

        DrawThickLineNDT(uuu, uuu + (ulu - uuu) * 0.25f, width, edgeColour);
        DrawThickLineNDT(uuu + (ulu - uuu) * 0.75f, ulu, width, edgeColour);

        DrawThickLineNDT(lll, lll + (llu - lll) * 0.25f, width, edgeColour);
        DrawThickLineNDT(lll + (llu - lll) * 0.75f, llu, width, edgeColour);

        DrawThickLineNDT(ull, ull + (ulu - ull) * 0.25f, width, edgeColour);
        DrawThickLineNDT(ull + (ulu - ull) * 0.75f, ulu, width, edgeColour);

        DrawThickLineNDT(lul, lul + (luu - lul) * 0.25f, width, edgeColour);
        DrawThickLineNDT(lul + (luu - lul) * 0.75f, luu, width, edgeColour);

        DrawThickLineNDT(uul, uul + (uuu - uul) * 0.25f, width, edgeColour);
        DrawThickLineNDT(uul + (uuu - uul) * 0.75f, uuu, width, edgeColour);
    }
}

void DebugRenderer::DebugDraw(const Sphere& sphere, const glm::vec4& color) {
    DebugRenderer::DrawPointNDT(sphere.getCenter(), sphere.getRadius(), color);
}

void DebugRenderer::DebugDraw(const Frustum& frustum, const glm::vec4& color) {
    const auto& vertices = frustum.getVertices();

    DrawHairLine(vertices[0], vertices[1], color);
    DrawHairLine(vertices[1], vertices[2], color);
    DrawHairLine(vertices[2], vertices[3], color);
    DrawHairLine(vertices[3], vertices[0], color);
    DrawHairLine(vertices[4], vertices[5], color);
    DrawHairLine(vertices[5], vertices[6], color);
    DrawHairLine(vertices[6], vertices[7], color);
    DrawHairLine(vertices[7], vertices[4], color);
    DrawHairLine(vertices[0], vertices[4], color);
    DrawHairLine(vertices[1], vertices[5], color);
    DrawHairLine(vertices[2], vertices[6], color);
    DrawHairLine(vertices[3], vertices[7], color);
}

/*void DebugRenderer::DebugDraw(Graphics::Light* light, const glm::quat& rotation, const glm::vec4& color) {
    // Directional
    if (light->Type < 0.1f) {
        glm::vec3 offset{0.0f, 0.1f, 0.0f};
        DrawHairLine(glm::vec3{light->Position} + offset, glm::vec3{light->Position + (light->Direction) * 2.0f} + offset, color);
        DrawHairLine(glm::vec3{light->Position} - offset, glm::vec3{light->Position + (light->Direction) * 2.0f} - offset, color);
        DrawHairLine(glm::vec3{light->Position}, glm::vec3{light->Position + (light->Direction) * 2.0f}, color);
        DebugDrawCone(20, 4, 30.0f, 1.5f, (light->Position - (light->Direction) * 1.5f), rotation, color);
    }
    // Spot
    else if (light->Type < 1.1f) {
        DebugDrawCone(20, 4, light->Angle * RADTODEG, light->Intensity, light->Position, rotation, color);
    }
    // Point
    else {
        DebugDrawSphere(light->Radius * 0.5f, light->Position, color);
    }
}*/

/*void DebugRenderer::DebugDraw(SoundNode* sound, const glm::vec4& color) {
    DrawPoint(sound->GetPosition(), sound->GetRadius(), color);
}*/

void DebugRenderer::DebugDrawCircle(int numVerts, float radius, const glm::vec3& position, const glm::quat& rotation, const glm::vec4& color) {
    float step = 360.0f / static_cast<float>(numVerts);

    for (int i = 0; i < numVerts; i++) {
        float a = static_cast<float>(i);
        float cx = glm::cos(step * a) * radius;
        float cy = glm::sin(step * a) * radius;
        glm::vec3 current{cx, cy, 0.0f};

        float nx = glm::cos(step * (a + 1.0f)) * radius;
        float ny = glm::sin(step * (a + 1.0f)) * radius;
        glm::vec3 next{nx, ny, 0.0f};

        DrawHairLine(position + (rotation * current), position + (rotation * next), color);
    }
}

void DebugRenderer::DebugDrawSphere(float radius, const glm::vec3& position, const glm::vec4& color) {
    DebugDrawCircle(20, radius, position, glm::quat{glm::vec3{0.0f, 0.0f, 0.0f}}, color);
    DebugDrawCircle(20, radius, position, glm::quat{glm::vec3{90.0f, 0.0f, 0.0f}}, color);
    DebugDrawCircle(20, radius, position, glm::quat{glm::vec3{0.0f, 90.0f, 90.0f}}, color);
}

void DebugRenderer::DebugDrawCone(int numCircleVerts, int numLinesToCircle, float angle, float length, const glm::vec3& position, const glm::quat& rotation, const glm::vec4& color) {
    float endAngle = glm::tan(angle * 0.5f) * length;
    glm::vec3 forward{ -(rotation * glm::vec3{ 0.0f, 0.0f, -1.0f }) };
    glm::vec3 endPosition{ position + forward * length };
    DebugDrawCircle(numCircleVerts, endAngle, endPosition, rotation, color);

    for (int i = 0; i < numLinesToCircle; i++) {
        float a = static_cast<float>(i) * 90.0f;
        glm::vec3 point{ rotation * glm::vec3{ glm::cos(a), glm::sin(a), 0.0f } * endAngle };
        DrawHairLine(position, position + point + forward * length, color);
    }
}

void DebugDrawArc(int numVerts, float radius, const glm::vec3& start, const glm::vec3& end, const glm::quat& rotation, const glm::vec4& color) {
    float step = 180.0f / static_cast<float>(numVerts);
    glm::quat rot = glm::lookAt(rotation * start, rotation * end, glm::vec3{0.0f, 1.0f, 0.0f});
    rot = rotation * rot;

    glm::vec3 arcCentre = (start + end) * 0.5f;
    for (int i = 0; i < numVerts; i++) {
        float a = static_cast<float>(i);
        float cx = glm::cos(step * a) * radius;
        float cy = glm::sin(step * a) * radius;
        glm::vec3 current = glm::vec3{cx, cy, 0.0f};

        float nx = glm::cos(step * (a + 1.0f)) * radius;
        float ny = glm::sin(step * (a + 1.0f)) * radius;
        glm::vec3 next = glm::vec3{nx, ny, 0.0f};

        DebugRenderer::DrawHairLine(arcCentre + (rot * current), arcCentre + (rot * next), color);
    }
}

void DebugRenderer::DebugDrawCapsule(const glm::vec3& position, const glm::quat& rotation, float height, float radius, const glm::vec4& color) {
    glm::vec3 up{ rotation * glm::vec3{ 0.0f, 1.0f, 0.0f }};

    glm::vec3 topSphereCentre{ position + up * (height * 0.5f) };
    glm::vec3 bottomSphereCentre{ position - up * (height * 0.5f) };

    DebugDrawCircle(20, radius, topSphereCentre, rotation * glm::quat{glm::vec3{glm::radians(90.0f), 0.0f, 0.0f}}, color);
    DebugDrawCircle(20, radius, bottomSphereCentre, rotation * glm::quat{glm::vec3{glm::radians(90.0f), 0.0f, 0.0f}}, color);

    // Draw 10 arcs
    // Sides
    static constexpr float step = 360.0f / 20.0f;
    for (int i = 0; i < 20; i++) {
        float a = static_cast<float>(i);
        float z = glm::cos(step * a) * radius;
        float x = glm::sin(step * a) * radius;

        glm::vec3 offset{ rotation * glm::vec4{ x, 0.0f, z, 0.0f }};
        DrawHairLine(bottomSphereCentre + offset, topSphereCentre + offset, color);

        if (i < 10) {
            float z2 = glm::cos(step * (a + 10.0f)) * radius;
            float x2 = glm::sin(step * (a + 10.0f)) * radius;

            glm::vec3 offset2{ rotation * glm::vec4{ x2, 0.0f, z2, 0.0f }};
            // Top Hemishpere
            DebugDrawArc(20, radius, topSphereCentre + offset, topSphereCentre + offset2, rotation, color);
            // Bottom Hemisphere
            DebugDrawArc(20, radius, bottomSphereCentre + offset, bottomSphereCentre + offset2, rotation * glm::quat{glm::vec3{glm::radians(180.0f), 0.0f, 0.0f}}, color);
        }
    }
}

void DebugRenderer::DebugDraw(const Ray& ray, const glm::vec4& color, float distance) {
    DrawHairLine(ray.getOrigin(), ray.getPoint(distance), color);
}

void DebugRenderer::onUpdate() {
    drawList.clear();
    drawListNDT.clear();
}