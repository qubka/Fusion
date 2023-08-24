#include "debug_renderer.h"

#include "fusion/geometry/aabb.h"
#include "fusion/geometry/frustum.h"
#include "fusion/geometry/sphere.h"
#include "fusion/geometry/ray.h"

#include "fusion/scene/components.h"

using namespace fe;

DebugRenderer* DebugRenderer::Instance = nullptr;

DebugRenderer::DebugRenderer() {
    Instance = this;
}

DebugRenderer::~DebugRenderer() {
    Instance = nullptr;
}

void DebugRenderer::DrawPoint(const glm::vec3& pos, float pointRadius, const glm::vec3& color) {
    GenDrawPoint<false>(pos, pointRadius, glm::vec4{color, 1.0f});
}

void DebugRenderer::DrawPoint(const glm::vec3& pos, float pointRadius, const glm::vec4& color) {
    GenDrawPoint<false>(pos, pointRadius, color);
}

void DebugRenderer::DrawPointNDT(const glm::vec3& pos, float pointRadius, const glm::vec3& color) {
    GenDrawPoint<true>(pos, pointRadius, glm::vec4{color, 1.0f});
}

void DebugRenderer::DrawPointNDT(const glm::vec3& pos, float pointRadius, const glm::vec4& color) {
    GenDrawPoint<true>(pos, pointRadius, color);
}

void DebugRenderer::DrawThickLine(const glm::vec3& start, const glm::vec3& end, float lineWidth, const glm::vec3& color) {
    GenDrawThickLine<false>(start, end, lineWidth, glm::vec4{color, 1.0f});
}

void DebugRenderer::DrawThickLine(const glm::vec3& start, const glm::vec3& end, float lineWidth, const glm::vec4& color) {
    GenDrawThickLine<false>(start, end, lineWidth, color);
}

void DebugRenderer::DrawThickLineNDT(const glm::vec3& start, const glm::vec3& end, float lineWidth, const glm::vec3& color) {
    GenDrawThickLine<true>(start, end, lineWidth, glm::vec4{color, 1.0f});
}

void DebugRenderer::DrawThickLineNDT(const glm::vec3& start, const glm::vec3& end, float lineWidth, const glm::vec4& color) {
    GenDrawThickLine<true>(start, end, lineWidth, color);
}

void DebugRenderer::DrawHairLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color) {
    GenDrawHairLine<false>(start, end, glm::vec4{color, 1.0f});
}

void DebugRenderer::DrawHairLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color) {
    GenDrawHairLine<false>(start, end, color);
}

void DebugRenderer::DrawHairLineNDT(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color) {
    GenDrawHairLine<true>(start, end, glm::vec4{color, 1.0f});
}

void DebugRenderer::DrawHairLineNDT(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color) {
    GenDrawHairLine<true>(start, end, color);
}

// Draw Matrix (x,y,z axis at pos)
void DebugRenderer::DrawMatrix(const glm::mat4& mtx) {
    glm::vec3 position{ mtx[3] };
    auto m = glm::value_ptr(mtx);
    GenDrawHairLine<false>(position, position + glm::vec3{m[0], m[1], m[2]}, glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
    GenDrawHairLine<false>(position, position + glm::vec3{m[4], m[5], m[6]}, glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
    GenDrawHairLine<false>(position, position + glm::vec3{m[8], m[9], m[10]}, glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});
}

void DebugRenderer::DrawMatrix(const glm::mat3& mtx, const glm::vec3& position) {
    GenDrawHairLine<false>(position, position + mtx[0], glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
    GenDrawHairLine<false>(position, position + mtx[1], glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
    GenDrawHairLine<false>(position, position + mtx[2], glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});
}

void DebugRenderer::DrawMatrixNDT(const glm::mat4& mtx) {
    glm::vec3 position{ mtx[3] };
    auto m = glm::value_ptr(mtx);
    GenDrawHairLine<true>(position, position + glm::vec3{m[0], m[1], m[2]}, glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
    GenDrawHairLine<true>(position, position + glm::vec3{m[4], m[5], m[6]}, glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
    GenDrawHairLine<true>(position, position + glm::vec3{m[8], m[9], m[10]}, glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});
}

void DebugRenderer::DrawMatrixNDT(const glm::mat3& mtx, const glm::vec3& position) {
    GenDrawHairLine<true>(position, position + mtx[0], glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
    GenDrawHairLine<true>(position, position + mtx[1], glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
    GenDrawHairLine<true>(position, position + mtx[2], glm::vec4{0.0f, 0.0f, 1.0f, 1.0f});
}

void DebugRenderer::DrawTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& color) {
    GenDrawTriangle<false>(v0, v1, v2, color);
}

void DebugRenderer::DrawTriangleNDT(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& color) {
    GenDrawTriangle<true>(v0, v1, v2, color);
}

// Draw Polygon (Renders as a triangle fan, so verts must be arranged in order)
void DebugRenderer::DrawPolygon(gsl::span<const glm::vec3> verts, const glm::vec4& color) {
    for (int i = 2; i < verts.size(); ++i) {
        GenDrawTriangle<false>(verts[0], verts[i - 1], verts[i], color);
    }
}

void DebugRenderer::DrawPolygonNDT(gsl::span<const glm::vec3> verts, const glm::vec4& color) {
    for (int i = 2; i < verts.size(); ++i) {
        GenDrawTriangle<true>(verts[0], verts[i - 1], verts[i], color);
    }
}

void DebugRenderer::DebugDraw(const AABB& box, const glm::vec4& edgeColour, bool cornersOnly, float width) {
    glm::vec3 uuu{ box.getMax() };
    glm::vec3 lll{ box.getMin() };

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

void DebugRenderer::DebugDraw(const LightComponent& light, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& direction, const glm::vec4& color) {
    // Directional
    switch (light.type) {
        case LightComponent::LightType::Point:
            DebugDrawSphere(light.radius * 0.5f, position, color);
            break;
        case LightComponent::LightType::Spot:
            DebugDrawCone(20, 4, light.outerCutOff, 3.0f, position, rotation, color);
            break;
        case LightComponent::LightType::Directional:
            glm::vec3 offset{0.0f, 0.1f, 0.0f};
            DrawHairLine(position + offset, (position + direction * 2.0f) + offset, color);
            DrawHairLine(position - offset, (position + direction * 2.0f) - offset, color);
            DrawHairLine(position, (position + direction * 2.0f), color);
            DebugDrawCone(20, 4, 15.0f, 1.5f, (position - direction * 1.5f), rotation, color);
            break;
    }
}

/*void DebugRenderer::DebugDraw(SoundNode* sound, const glm::vec4& color) {
    DrawPoint(sound->GetPosition(), sound->GetRadius(), color);
}*/

void DebugRenderer::DebugDrawCircle(int numVerts, float radius, const glm::vec3& position, const glm::quat& rotation, const glm::vec4& color) {
    float step = glm::two_pi<float>() / static_cast<float>(numVerts);

    for (int i = 0; i < numVerts; ++i) {
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
    float radius = glm::tan(glm::radians(angle) * 0.5f) * length;
    glm::vec3 forward{ -(rotation * vec3::forward) };
    glm::vec3 endPosition{ position + forward * length };
    DebugDrawCircle(numCircleVerts, radius, endPosition, rotation, color);

    float step = glm::two_pi<float>() / static_cast<float>(numLinesToCircle);

    for (int i = 0; i < numLinesToCircle; ++i) {
        float a = static_cast<float>(i);
        float x = glm::cos(step * a) * radius;
        float y = glm::sin(step * a) * radius;

        glm::vec3 point{ rotation * glm::vec3{x, y, 0.0f} };
        DrawHairLine(position, position + point + forward * length, color);
    }
}

void DebugRenderer::DebugDrawArc(int numVerts, float radius, const glm::vec3& start, const glm::vec3& end, const glm::quat& rotation, const glm::vec4& color) {
    float step = 180.0f / static_cast<float>(numVerts);
    glm::quat rot{ glm::lookAt(rotation * start, rotation * end, vec3::up) };
    rot = rotation * rot;

    glm::vec3 arcCentre{ (start + end) * 0.5f };
    for (int i = 0; i < numVerts; ++i) {
        float a = static_cast<float>(i);
        float cx = glm::cos(step * a) * radius;
        float cy = glm::sin(step * a) * radius;
        glm::vec3 current{cx, cy, 0.0f};

        float nx = glm::cos(step * (a + 1.0f)) * radius;
        float ny = glm::sin(step * (a + 1.0f)) * radius;
        glm::vec3 next{nx, ny, 0.0f};

        DrawHairLine(arcCentre + (rot * current), arcCentre + (rot * next), color);
    }
}

void DebugRenderer::DebugDrawCapsule(const glm::vec3& position, const glm::quat& rotation, float height, float radius, const glm::vec4& color) {
    glm::vec3 up{ rotation * glm::vec3{ 0.0f, 1.0f, 0.0f }};

    glm::vec3 topSphereCentre{ position + up * (height * 0.5f) };
    glm::vec3 bottomSphereCentre{ position - up * (height * 0.5f) };

    glm::quat forward{ glm::vec3{glm::radians(90.0f), 0.0f, 0.0f} };
    glm::quat down{ glm::quat{glm::vec3{glm::radians(180.0f), 0.0f, 0.0f}} };

    DebugDrawCircle(20, radius, topSphereCentre, rotation * forward, color);
    DebugDrawCircle(20, radius, bottomSphereCentre, rotation * forward, color);

    // Draw 10 arcs
    // Sides
    constexpr float step = glm::two_pi<float>() / 20.0f;

    for (int i = 0; i < 20; ++i) {
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
            DebugDrawArc(20, radius, bottomSphereCentre + offset, bottomSphereCentre + offset2, rotation * down, color);
        }
    }
}

void DebugRenderer::DebugDraw(const Ray& ray, const glm::vec4& color, float distance) {
    DrawHairLine(ray.getOrigin(), ray.getPoint(distance), color);
}

void DebugRenderer::onStart() {
}

void DebugRenderer::onUpdate() {
    for (auto& drawList: drawLists) {
        drawList.clear();
    }
}

void DebugRenderer::onStop() {
}
