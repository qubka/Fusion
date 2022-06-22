#pragma once

namespace fe {
    class File {
        using SimpleHandler = std::function<void(size_t, const void*)>;
        using NamedHandler = std::function<void(const char*, size_t, const void*)>;

    public:
        static void WithBinaryFileContents(const std::filesystem::path& filename, const SimpleHandler& handler);
        static void WithBinaryFileContents(const std::filesystem::path& filename, const NamedHandler& handler);

        static std::vector<uint8_t> ReadBinaryFile(const std::filesystem::path& filename);
        static std::string ReadTextFile(const std::filesystem::path& filename);

        static std::string ExtensionIcon(const std::filesystem::path& filename);

    private:
        static std::map<std::string, std::string> Extensions;
    };
}