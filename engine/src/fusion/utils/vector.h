#pragma once

namespace fe {
    class FUSION_API Vector {
    public:
        Vector() = delete;

        template<typename T>
        static void Append(std::vector<uint8_t>& outputBuffer, const std::vector<T>& t) { //! not use span here
            auto offset = outputBuffer.size();
            auto copySize = t.size() * sizeof(T);
            outputBuffer.resize(offset + copySize);
            std::memcpy(outputBuffer.data() + offset, t.data(), copySize);
        }

        template<typename T>
        static void Append(std::vector<uint8_t>& outputBuffer, const T& t) {
            auto offset = outputBuffer.size();
            auto copySize = sizeof(T);
            outputBuffer.resize(offset + copySize);
            std::memcpy(outputBuffer.data() + offset, &t, copySize);
        }
    };
}