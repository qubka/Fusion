#pragma once

namespace fe {
    class Vector {
    public:
        Vector() = delete;

        template<typename T>
        static void Append(std::vector<std::byte>& outputBuffer, const std::vector<T>& t) {
            auto offset = outputBuffer.size();
            auto copySize = t.size() * sizeof(T);
            outputBuffer.resize(offset + copySize);
            std::memcpy(outputBuffer.data() + offset, t.data(), copySize);
        }

        template<typename T>
        static void Append(std::vector<std::byte>& outputBuffer, const T& t) {
            auto offset = outputBuffer.size();
            auto copySize = sizeof(T);
            outputBuffer.resize(offset + copySize);
            std::memcpy(outputBuffer.data() + offset, &t, copySize);
        }
    };
}