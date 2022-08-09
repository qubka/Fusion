#pragma once

namespace fe {
    class SpecConstant {
    public:
        SpecConstant() : value{0.0f} {}
        template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
        SpecConstant(const T& value) : value{static_cast<float>(value)} {}

    private:
        float value;
    };
}
