#include "rect.hpp"

using namespace fe;

Rect::Rect(std::span<const glm::vec2> points) {
    x1 = FLT_MAX;
    x2 = -FLT_MAX;
    y1 = FLT_MAX;
    y2 = -FLT_MAX;
    for (auto& point : points)
        include(point);
}

void Rect::set(float aX1, float aY1, float aX2, float aY2) {
    x1 = aX1;
    y1 = aY1;
    x2 = aX2;
    y2 = aY2;
}

void Rect::canonicalize() {
    if (x1 > x2) {
        float temp = x1;
        x1 = x2;
        x2 = temp;
    }

    if (y1 > y2) {
        float temp = y1;
        y1 = y2;
        y2 = temp;
    }
}

Rect Rect::canonicalized() const {
    Rect result{*this};
    result.canonicalize();
    return result;
}

void Rect::clipBy(const Rect& clip) {
    if (x1 < clip.x1)
        x1 = clip.x1;
    if (x2 < clip.x1)
        x2 = clip.x1;
    if (x1 > clip.x2)
        x1 = clip.x2;
    if (x2 > clip.x2)
        x2 = clip.x2;

    if (y1 < clip.y1)
        y1 = clip.y1;
    if (y2 < clip.y1)
        y2 = clip.y1;
    if (y1 > clip.y2)
        y1 = clip.y2;
    if (y2 > clip.y2)
        y2 = clip.y2;
}

Rect Rect::getClipBy(const Rect& clip) const {
    Rect result(*this);
    result.clipBy(Rect(clip));
    return result;
}

void Rect::offset(const glm::vec2& offset) {
    x1 += offset.x;
    x2 += offset.x;
    y1 += offset.y;
    y2 += offset.y;
}

Rect Rect::getOffset(const glm::vec2& off) const {
    Rect result(*this);
    result.offset(off);
    return result;
}

void Rect::moveULTo(const glm::vec2& newUL) {
    set(newUL.x, newUL.y, newUL.x + getWidth(), newUL.y + getHeight());
}

Rect Rect::getMoveULTo(const glm::vec2& newUL) const {
    Rect result{*this};
    result.moveULTo(newUL);
    return result;
}

void Rect::inflate(const glm::vec2& amount) {
    x1 -= amount.x;
    x2 += amount.x;
    y1 -= amount.y; // assume canonical rect has y1 < y2
    y2 += amount.y;
}

Rect Rect::inflated(const glm::vec2& amount) const {
    Rect result{*this};
    result.inflate(amount);
    return result;
}

void Rect::scaleCentered(const glm::vec2& scale) {
    const float halfWidth = getWidth() * scale.x / 2.0f;
    const float halfHeight = getHeight() * scale.y / 2.0f;
    glm::vec2 center {getCenter()};
    x1 = center.x - halfWidth;
    x2 = center.x + halfWidth;
    y1 = center.y - halfHeight;
    y2 = center.y + halfHeight;
}

void Rect::scaleCentered(float scale) {
    const float halfWidth = getWidth() * scale / 2;
    const float halfHeight = getHeight() * scale / 2;
    glm::vec2 center {getCenter()};
    x1 = center.x - halfWidth;
    x2 = center.x + halfWidth;
    y1 = center.y - halfHeight;
    y2 = center.y + halfHeight;
}

Rect Rect::scaledCentered(const glm::vec2& scale) const {
    const float halfWidth = getWidth() * scale.x / 2;
    const float halfHeight = getHeight() * scale.y / 2;
    glm::vec2 center {getCenter()};
    return { center.x - halfWidth, center.y - halfHeight, center.x + halfWidth, center.y + halfHeight };
}

Rect Rect::scaledCentered(float scale) const {
    const float halfWidth = getWidth() * scale / 2;
    const float halfHeight = getHeight() * scale / 2;
    glm::vec2 center {getCenter()};
    return { center.x - halfWidth, center.y - halfHeight, center.x + halfWidth, center.y + halfHeight };
}

void Rect::scale(float s) {
    x1 *= s;
    x2 *= s;
    y1 *= s;
    y2 *= s;
}

void Rect::scale(const glm::vec2& scale) {
    x1 *= scale.x;
    y1 *= scale.y;
    x2 *= scale.x;
    y2 *= scale.y;
}

Rect Rect::scaled(float s) const {
    return { x1 * s, y1 * s, x2 * s, y2 * s };
}

Rect Rect::scaled(const glm::vec2& scale) const {
    return { x1 * scale.x, y1 * scale.y, x2 * scale.x, y2 * scale.y };
}

Rect Rect::transformed(const glm::mat3& matrix) const {
    glm::vec2 center{ glm::vec2(x1 + x2, y1 + y2) / 2.0f };
    glm::vec2 extents{ glm::abs(glm::vec2(x2, y2) - center) };

    glm::vec3 x = matrix * glm::vec3{extents.x, 0, 0};
    glm::vec3 y = matrix * glm::vec3{0, extents.y, 0};

    extents = glm::vec2{ glm::abs(x) + glm::abs(y) };
    center = glm::vec2{matrix * glm::vec3{center, 1}};

    return { center.x - extents.x, center.y - extents.y, center.x + extents.x, center.y + extents.y };
}

void Rect::transform(const glm::mat3& matrix) {
    glm::vec2 center{ glm::vec2{ x1 + x2, y1 + y2 } / 2.0f };
    glm::vec2 extents{ glm::abs(glm::vec2{ x2, y2 } - center) };

    glm::vec3 x{ matrix * glm::vec3{ extents.x, 0, 0 }};
    glm::vec3 y{ matrix * glm::vec3{ 0, extents.y, 0 }};

    extents = glm::vec2{glm::abs(x) + glm::abs(y)};
    center = glm::vec2{matrix * glm::vec3{center, 1}};

    x1 = center.x - extents.x;
    y1 = center.y - extents.y;
    x2 = center.x + extents.x;
    y2 = center.y + extents.y;
}

bool Rect::intersects(const Rect& rect) const {
    if ((x1 > rect.x2) || (x2 < rect.x1) || (y1 > rect.y2) || (y2 < rect.y1))
        return false;
    else
        return true;
}

float Rect::distance(const glm::vec2& pt) const {
    float squaredDistance = 0;
    if (pt.x < x1) squaredDistance += (x1 - pt.x) * (x1 - pt.x);
    else if (pt.x > x2) squaredDistance += (pt.x - x2) * (pt.x - x2);
    if (pt.y < y1) squaredDistance += (y1 - pt.y) * (y1 - pt.y);
    else if (pt.y > y2) squaredDistance += (pt.y - y2) * (pt.y - y2);

    if (squaredDistance > 0)
        return sqrtf(squaredDistance);
    else
        return 0;
}

float Rect::distanceSquared(const glm::vec2& pt) const {
    float squaredDistance = 0;
    if (pt.x < x1)
        squaredDistance += (x1 - pt.x) * (x1 - pt.x);
    else if (pt.x > x2)
        squaredDistance += (pt.x - x2) * (pt.x - x2);
    if (pt.y < y1)
        squaredDistance += (y1 - pt.y) * (y1 - pt.y);
    else if (pt.y > y2)
        squaredDistance += (pt.y - y2) * (pt.y - y2);

    return squaredDistance;
}

glm::vec2 Rect::closestPoint(const glm::vec2& pt) const {
    glm::vec2 result{ pt };
    if (pt.x < x1) result.x = x1;
    else if (pt.x > x2) result.x = x2;
    if (pt.y < y1) result.y = y1;
    else if (pt.y > y2) result.y = y2;
    return result;
}

void Rect::include(const glm::vec2& point) {
    if (x1 > point.x) x1 = point.x;
    if (x2 < point.x) x2 = point.x;
    if (y1 > point.y) y1 = point.y;
    if (y2 < point.y) y2 = point.y;
}

void Rect::include(std::span<const glm::vec2> points) {
    for (const auto& point : points)
        include(point);
}

void Rect::include(const Rect& rect) {
    include(glm::vec2{rect.x1, rect.y1});
    include(glm::vec2{rect.x2, rect.y2});
}

Rect Rect::getCenteredFit(const Rect& other, bool expand) const {
    Rect result{*this};
    result.offset(other.getCenter() - result.getCenter());

    bool isInside = ((result.getWidth() < other.getWidth()) && (result.getHeight() < other.getHeight()));
    if (expand || (!isInside)) { // need to do some scaling
        float aspectAspect = result.getAspectRatio() / other.getAspectRatio();
        if (aspectAspect >= 1.0f) { // result is proportionally wider so we need to fit its x-axis
            float scaleBy = other.getWidth() / result.getWidth();
            result.scaleCentered(scaleBy);
        } else { // result is proportionally wider so we need to fit its y-axis
            float scaleBy = other.getHeight() / result.getHeight();
            result.scaleCentered(scaleBy);
        }
    }

    return result;
}

Rect Rect::getCenteredFill(const Rect& other, bool contract) const {
    Rect result{*this};
    result.offset(other.getCenter() - result.getCenter());

    bool otherIsInside = ((result.getWidth() > other.getWidth()) && (result.getHeight() > other.getHeight()));
    if (contract || (!otherIsInside)) { // need to do some scaling
        float aspectAspect = result.getAspectRatio() / other.getAspectRatio();
        if (aspectAspect <= 1.0f) { // result is proportionally wider so we need to fit its x-axis
            float scaleBy = other.getWidth() / result.getWidth();
            result.scaleCentered(scaleBy);
        } else { // result is proportionally wider so we need to fit its y-axis
            float scaleBy = other.getHeight() / result.getHeight();
            result.scaleCentered(scaleBy);
        }
    }

    return result;
}