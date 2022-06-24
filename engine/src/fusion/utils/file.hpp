#pragma once

namespace fe {
    class File {
        using SimpleHandler = std::function<void(size_t, const void*)>;
    public:
        File() = delete;

        /**
         * Reads a file found by real or partial path with a lambda.
         * @param filename The path to read.
         * @param handler The lambda with data read from the file.
         */
        static void Read(const std::filesystem::path& filename, const SimpleHandler& handler);

        /**
         * Opens a binary file, reads the contents of the file into a byte array, and then closes the file.
         * @param filename The path to read.
         * @return The data read from the file.
         */
        static std::vector<uint8_t> ReadAllBytes(const std::filesystem::path& filename);

        /**
         * Opens a text file, reads all the text in the file into a string, and then closes the file.
         * @param filename The path to read.
         * @return The data read from the file.
         */
        static std::string ReadAllText(const std::filesystem::path& filename);

        /**
         *
         * @param filename
         * @return
         */
        static std::string ExtensionIcon(const std::filesystem::path& filename);

    private:
        static std::map<std::string, std::string> Extensions;
    };
}