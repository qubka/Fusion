#pragma once

namespace fe {
    /**
     * @brief Component that represents a 3d fog.
     */
    class Fog {
    public:
        /**
         * Creates a new hazy fog.
         * @param color The color of the fog.
         * @param density How dense the fog will be.
         * @param gradient The gradient of the fog.
         * @param lowerLimit At what height will the skybox fog begin to appear.
         * @param upperLimit At what height will there be skybox no fog.
         */
        explicit Fog(const glm::vec4& color = {0.0f, 0.0f, 0.0f, 1.0f}, float density = 0.0f, float gradient = -1.0f, float lowerLimit = 0.0f, float upperLimit = 0.0f);

        const glm::vec4& getColor() const { return color; }
        void setColor(const glm::vec4& value) { color = value; }

        float getDensity() const { return density; }
        void setDensity(float value) { density = value; }

        float getGradient() const { return gradient; }
        void setGradient(float value) { gradient = value; }

        float getLowerLimit() const { return lowerLimit; }
        void setLowerLimit(float value) { lowerLimit = value; }

        float getUpperLimit() const { return upperLimit; }
        void setUpperLimit(float value) { upperLimit = value; }

    private:
        glm::vec4 color;
        float density;
        float gradient;
        float lowerLimit;
        float upperLimit;
    };
}
