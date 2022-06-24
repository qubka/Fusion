#pragma once

namespace fe {
    class Version {
    public:
        Version(uint8_t major = 0, uint8_t minor = 0, uint8_t patch = 0)
            : major{major}
            , minor{minor}
            , patch{patch} {
        }
        Version(uint32_t version) {
            *this = version;
        }

        Version& operator=(uint32_t version) {
            major = ((uint32_t)(version) >> 22);
            minor = (((uint32_t)(version) >> 12) & 0x3FFU);
            patch = ((uint32_t)(version) & 0xFFFU);
            return *this;
        }

        operator uint32_t() const {
            return ((((uint32_t)(major)) << 22) | (((uint32_t)(minor)) << 12) | ((uint32_t)(patch)));
        }

        bool operator >=(const Version& other) const {
            return (operator uint32_t()) >= (other.operator uint32_t());
        }

        std::string to_string() const {
            std::stringstream buffer;
            buffer << std::to_string(major) << "." << std::to_string(minor) << "." << std::to_string(patch);
            return buffer.str();
        }

    private:
        uint8_t major, minor, patch;
    };
}
