#pragma once

#include "fusion/core/module.hpp"

namespace fe {
    class FileSystem : public Module::Registrar<FileSystem> {
        using SimpleHandler = std::function<void(const uint8_t*, size_t)>;
    public:
        FileSystem();
        ~FileSystem() override;

        void update() override;

        /**
         * Gets if the path is found in one of the search paths.
         * @param path The path to look for.
         * @return If the path is found in one of the searches.
         */
        static bool Exists(const std::filesystem::path& path);

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
        static std::vector<uint8_t> ReadBytes(const std::filesystem::path& filename);

        /**
         * Opens a text file, reads all the text in the file into a string, and then closes the file.
         * @param filename The path to read.
         * @return The data read from the file.
         */
        static std::string ReadText(const std::filesystem::path& filename);

        /**
         * Finds all the files in a path.
         * @param path The path to search.
         * @param recursive If paths will be recursively searched.
         * @return The files found.
         */
        static std::vector<std::string> GetFiles(const std::filesystem::path& path, bool recursive = true);

        /**
         * Gets the next line from a stream.
         * @param is The input stream.
         * @param t The next string.
         * @return The input stream.
         */
        static std::istream& SafeGetLine(std::istream& is, std::string& t);

        static bool WriteFile(const std::filesystem::path& filename, uint8_t* buffer, size_t size);

        static bool WriteTextFile(const std::filesystem::path& filename, const std::string& text);

        static bool IsDirectory(const std::filesystem::path& path);

        static bool HasDirectories(const std::filesystem::path& path);

        /**
         *
         * @param filename
         * @return
         */
        static std::string GetExtension(const std::filesystem::path& filename);

        /**
         *
         * @param filename
         * @return
         */
        static std::string GetIcon(const std::filesystem::path& filename);

    private:
        static std::unordered_map<std::string, std::string> Extensions;
    };
}