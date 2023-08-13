#pragma once

namespace fe {
    enum PowerState : unsigned char {
        Unknown,
        OnBattery,
        NoBattery,
        Charging,
        Charged
    };

    class FUSION_API OS {
    public:
        OS() = default;
        virtual ~OS() = default;

        virtual void run() = 0;

        static std::unique_ptr<OS> Init();

        static OS* Get() { return Instance; }

        virtual fs::path getExecutablePath() const = 0;
        virtual fs::path getCurrentWorkingDirectory() const { return ""; };
        virtual fs::path getAssetPath() const { return "" };

        virtual void vibrate() const {};
        virtual void setTitleBarColour(const glm::vec4& colour, bool dark = true) {};

        virtual void showKeyboard() {};
        virtual void hideKeyboard() {};
        virtual void delay(uint32_t usec) {};

        virtual void openFileLocation(const std::filesystem::path& path) { }
        virtual void openFileExternal(const std::filesystem::path& path) { }
        virtual void openURL(std::string_view url) { }

    protected:
        static OS* Instance;
    };
}