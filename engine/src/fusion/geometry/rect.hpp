#pragma once

namespace fe {
    class Rect {
    public:
        Rect();
        Rect(const glm::vec2& position, const glm::vec2& size);
        Rect(float x, float y, float width, float height);
        Rect(const glm::vec4& rect);

        const glm::vec4& get() const { return rect; }
        void set(const glm::vec2& position, const glm::vec2& size);
        void set(float x, float y, float width, float height);
        void set(const glm::vec4& rect);

        const glm::vec2& getPosition() const { return position; }
        void setPosition(const glm::vec2& position);

        const glm::vec2& getSize() const { return size; }
        void setSize(const glm::vec2& size);

        const glm::vec2& getCenter() const { return center; }
        void setCenter(const glm::vec2& center);

        bool contains(const glm::vec2& point) const;
        bool contains(const Rect& rect) const;

        bool intersects(const Rect& rect) const;
        void transform(const glm::mat4& transform);

    private:
        glm::vec4 rect;
        glm::vec2 position;
        glm::vec2 size;
        glm::vec2 center;
        float radius;
    };
}

std::ostream& operator<<(std::ostream& o, const fe::Rect& r);