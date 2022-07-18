#include "rect.hpp"

using namespace fe;

Rect::Rect() : position{0.0f}, size{0.0f}, rect{0.0f}, center{0.0f}, radius{0.0f} {
}

Rect::Rect(const glm::vec2& position, const glm::vec2& size) {
    set(position, size);
}

Rect::Rect(float x, float y, float width, float height) {
    set(x, y, width, height);
}

Rect::Rect(const glm::vec4& rect) {
   set(rect);
}

void Rect::set(const glm::vec2& p, const glm::vec2& s) {
    position = p;
    size = s;
    rect.x = position.x;
    rect.y = position.y;
    rect.z = size.x;
    rect.w = size.y;
    center = position + size * 0.5f;
    radius = size.x * 0.5f;
}

void Rect::set(float x, float y, float width, float height) {
    position = {x, y};
    size = {width, height};
    rect.x = x;
    rect.y = y;
    rect.z = width;
    rect.w = height;
    center = position + size * 0.5f;
    radius = size.x * 0.5f;
}

void Rect::set(const glm::vec4& abcd) {
    rect = abcd;
    position = {rect.x, rect.y};
    size = {rect.y, rect.z};
    center = position + size * 0.5f;
    radius = size.x * 0.5f;
}

void Rect::setPosition(const glm::vec2& p) {
    position = p;
    rect.x = position.x;
    rect.y = position.y;
    center = position + size * 0.5f;
}

void Rect::setSize(const glm::vec2& s) {
    size = s;
    rect.z = size.x;
    rect.w = size.y;
    center = position + size * 0.5f;
    radius = size.x * 0.5f;
}

void Rect::setCenter(const glm::vec2& c) {
    center = c;
    position = center - size * 0.5f;
    rect.x = center.x - size.x * 0.5f;
    rect.y = center.y - size.y * 0.5f;
}

void Rect::transform(const glm::mat4& transform) {
    rect = transform * rect;
    position = {rect.x, rect.y};
    size = {rect.z, rect.w};
    center = position + size * 0.5f;
    radius = size.x * 0.5f;
}

bool Rect::contains(const glm::vec2& point) const {
    if (point.x < position.x || point.x > position.x + size.x) {
        return false;
    }

    if (point.y < position.y || point.y > position.y + size.y) {
        return false;
    }

    return true;
}

bool Rect::contains(const Rect& rect) const {
    if (rect.position.x > position.x + size.x || rect.position.x + rect.size.x < position.x) {
        return false;
    }

    if (rect.position.y > position.y + size.y || rect.position.y + rect.size.y < position.y) {
        return false;
    }

    return true;
}

bool Rect::intersects(const Rect& rect) const {
    return contains(rect);
}

std::ostream& operator<<(std::ostream& o, const Rect& r) {
    return o << "(" << glm::to_string(r.getPosition()) << ", " << glm::to_string(r.getSize())<< ", " << glm::to_string(r.getCenter()) << ")";
}