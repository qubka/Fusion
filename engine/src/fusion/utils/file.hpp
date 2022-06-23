#pragma once

namespace fe {
    class File {
        using SimpleHandler = std::function<void(size_t, const void*)>;

    public:
        File() = delete;

        static void WithBinaryFileContents(const std::filesystem::path& filename, const SimpleHandler& handler);

        static std::vector<uint8_t> ReadBinaryFile(const std::filesystem::path& filename);
        static std::string ReadTextFile(const std::filesystem::path& filename);

        static std::string ExtensionIcon(const std::filesystem::path& filename);

    private:
        static std::map<std::string, std::string> Extensions;
    };
}