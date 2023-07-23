#pragma once

namespace fe {
    class Rect {
    public:
        //! Initializes the rectangle to be the bounding box of \a points
        Rect(float x1, float y1, float x2, float y2);
        explicit Rect(std::span<const glm::vec2> points);
        explicit Rect(const glm::vec2& v1, const glm::vec2& v2);

        void set(float x1, float y1, float x2, float y2);

        float getWidth() const { return x2 - x1; }
        float getHeight() const { return y2 - y1; }
        float getAspectRatio() const { return getWidth() / getHeight(); }
        float calcArea() const { return getWidth() * getHeight(); }

        void canonicalize(); // return rect w/ properly ordered coordinates
        Rect canonicalized() const; // return rect w/ properly ordered coordinates

        void clipBy(const Rect& clip);
        Rect getClipBy(const Rect& clip) const;

        //! translates the Rect by \a off
        void offset(const glm::vec2& offset);
        //! Returns a copy of the Rect translated by \a off
        Rect getOffset(const glm::vec2& off) const;
        //! translates the Rect so that its upper-left corner is \a newUL
        void moveULTo(const glm::vec2& newUL);
        //! Returns a copy of the Rect translated so that its upper-left corner is \a newUL
        Rect getMoveULTo(const glm::vec2& newUL) const;
        void inflate(const glm::vec2& amount);
        Rect inflated(const glm::vec2& amount) const;
        //! translates the rectangle so that its center is at \a center
        void offsetCenterfloato(const glm::vec2& center) { offset( center - getCenter()); }
        void scaleCentered(const glm::vec2& scale);
        Rect scaledCentered(const glm::vec2& scale) const;
        void scaleCentered(float scale);
        Rect scaledCentered(float scale) const;
        void scale(float scale);
        void scale(const glm::vec2& scale);
        Rect scaled(float scale) const;
        Rect scaled(const glm::vec2& scale) const;

        //! transforms the Rect by \a matrix. Represents the bounding box of the transformed Rect when \a matrix expresses non-scale/translate operations.
        void transform(const glm::mat3& transform);
        //! Returns a copy of the Rect transformed by \a matrix. Represents the bounding box of the transformed Rect when \a matrix expresses non-scale/translate operations.
        Rect transformed(const glm::mat3& transform) const;

        //! Is a point \a pt inside the rectangle
        bool contains(const glm::vec2& pt) const { return (pt.x >= x1) && (pt.x <= x2) && (pt.y >= y1) && (pt.y <= y2); }
        //! Returns whether \a rect intersects with this
        bool intersects(const Rect& rect) const;

        //! Returns the distance between the point \a pt and the rectangle. Points inside the rectangle return \c 0.
        float distance(const glm::vec2& pt) const;
        //! Returns the squared distance between the point \a pt and the rectangle. Points inside the rectangle return \c 0.
        float distanceSquared(const glm::vec2& pt) const;

        //! Returns the nearest point on the Rect \a rect. Points inside the rectangle return \a pt.
        glm::vec2 closestPoint(const glm::vec2& pt) const;

        float getX1() const { return x1; }
        float getY1() const { return y1; }
        float getX2() const { return x2; }
        float getY2() const { return y2; }

        glm::vec2 getUpperLeft() const { return { x1, y1 }; };
        glm::vec2 getUpperRight() const	{ return { x2, y1 }; };
        glm::vec2 getLowerRight() const	{ return { x2, y2 }; };
        glm::vec2 getLowerLeft() const { return { x1, y2 }; };
        glm::vec2 getCenter() const { return { (x1 + x2) / 2, (y1 + y2) / 2}; }
        glm::vec2 getSize() const { return { x2 - x1, y2 - y1 }; }

        /** \return Scaled copy with the same aspect ratio centered relative to and scaled to fit inside \a other. If \a expand then the rectangle is expanded if it is smaller than \a other */
        Rect getCenteredFit(const Rect& other, bool expand) const;
        /** \return Scaled copy with the same aspect ratio centered relative to and scaled so it completely fills \a other. If \a contract then the rectangle is contracted if it is larger than \a other */
        Rect getCenteredFill(const Rect& other, bool contract) const;

        /** Expands the Rect to include \a point in its interior **/
        void include(const glm::vec2& point);
        /** Expands the Rect to include all points in \a points in its interior **/
        void include(std::span<const glm::vec2> points);
        /** Expands the Rect to include \a rect in its interior **/
        void include(const Rect& rect);

        const Rect operator+(const glm::vec2& o) const { return getOffset(o); }
        const Rect operator-(const glm::vec2& o) const { return getOffset(-o); }
        const Rect operator*(float s) const { return scaled(s); }
        const Rect operator/(float s) const { return scaled(1.0f / s); }

        const Rect operator+( const Rect& rhs) const { return { x1 + rhs.x1, y1 + rhs.y1, x2 + rhs.x2, y2 + rhs.y2 }; }
        const Rect operator-( const Rect& rhs) const { return { x1 - rhs.x1, y1 - rhs.y1, x2 - rhs.x2, y2 - rhs.y2 }; }

        Rect& operator+=(const glm::vec2& o) { offset(o); return *this; }
        Rect& operator-=(const glm::vec2& o) { offset(-o); return *this; }
        Rect& operator*=(float s) { scale(s); return *this; }
        Rect& operator/=(float s) { scale(1.0f / s); return *this; }

    private:
        float x1, y1, x2, y2;
    };
}