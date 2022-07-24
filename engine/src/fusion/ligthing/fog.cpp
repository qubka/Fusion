#include "fog.hpp"

using namespace fe;

Fog::Fog(const glm::vec4& color, float density, float gradient, float lowerLimit, float upperLimit)
        : color{color}
        , density{density}
        , gradient{gradient}
        , lowerLimit{lowerLimit}
        , upperLimit{upperLimit}
{

}
