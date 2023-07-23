#pragma once

#define FUSION_MAKE_VERSION(variant, major, minor, patch) ((((uint32_t)(variant)) << 29) | (((uint32_t)(major)) << 22) | (((uint32_t)(minor)) << 12) | ((uint32_t)(patch)))
#define FUSION_MAKE_VERSION_VARIANT(version) ((uint32_t)(version) >> 29)
#define FUSION_MAKE_VERSION_MAJOR(version) (((uint32_t)(version) >> 22) & 0x7FU)
#define FUSION_MAKE_VERSION_MINOR(version) (((uint32_t)(version) >> 12) & 0x3FFU)
#define FUSION_MAKE_VERSION_PATCH(version) ((uint32_t)(version) & 0xFFFU)

namespace fe {
    class Version {
    public:
        Version(uint8_t variant, uint8_t major, uint8_t minor, uint8_t patch)
            : variant{variant}
            , major{major}
            , minor{minor}
            , patch{patch} {
        }
        Version(uint32_t version) {
            *this = version;
        }

        Version& operator=(uint32_t version) {
            variant = FUSION_MAKE_VERSION_VARIANT(version);
            major = FUSION_MAKE_VERSION_MAJOR(version);
            minor = FUSION_MAKE_VERSION_MINOR(version);
            patch = FUSION_MAKE_VERSION_PATCH(version);
            return *this;
        }

        operator uint32_t() const { return FUSION_MAKE_VERSION(variant, major, minor, patch); }

        bool operator==(const Version& rhs) const { return (operator uint32_t()) == (rhs.operator uint32_t()); }
        bool operator!=(const Version& rhs) const { return (operator uint32_t()) != (rhs.operator uint32_t()); }
        bool operator<=(const Version& rhs) const { return (operator uint32_t()) <= (rhs.operator uint32_t()); }
        bool operator>=(const Version& rhs) const { return (operator uint32_t()) >= (rhs.operator uint32_t()); }
        bool operator>(const Version& rhs)  const { return (operator uint32_t()) > (rhs.operator uint32_t()); }
        bool operator<(const Version& rhs)  const { return (operator uint32_t()) < (rhs.operator uint32_t()); }

        std::string toString() const {
            std::stringstream ss;
            ss << static_cast<long>(variant) << "." << static_cast<long>(major) << "." << static_cast<long>(minor) << "." << static_cast<long>(patch);
            return ss.str();
        }

    private:
        uint8_t variant;
        uint8_t major;
        uint8_t minor;
        uint8_t patch;
    };
}
