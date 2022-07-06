#pragma once

#include "fusion/core/module.hpp"
#include "fusion/utils/date_time.hpp"

namespace fe {
    enum class FileType {
        Regular,      /**< a normal file */
        Directory,    /**< a directory */
        Symlink,      /**< a symlink */
        Other         /**< something completely different like a device */
    };

    struct FileStats {
        size_t filesize; /**< size in bytes, -1 for non-files and unknown */
        DateTime modtime;  /**< last modification time */
        DateTime createtime; /**< like modtime, but for file creation time */
        DateTime accesstime; /**< like modtime, but for file access time */
        FileType filetype; /**< File? Directory? Symlink? */
        bool readonly; /**< non-zero if read only, zero if writable. */
    };

    enum class FileAttributes : uint8_t {
        ReadOnly = 1,
        Regular = 2,
        Directory = 4,
        Symlink = 8,
        // TODO: Expand
    };
    BITMASK_DEFINE_MAX_ELEMENT(FileAttributes, Symlink);

    class FileSystem : public Module::Registrar<FileSystem> {
        using SimpleHandler = std::function<void(const uint8_t*, size_t)>;
    public:
        FileSystem();
        ~FileSystem() override;

        void onUpdate() override;

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
         * Opens a file, write all data into the file, and then closes the file.
         * @param filename The path to write.
         * @param buffer The buffer data.
         * @param size The size of the buffer.
         * @return True on the success, false otherwise.
         */
        static bool WriteFile(const std::filesystem::path& filename, uint8_t* buffer, size_t size);

        /**
         * Finds all the files in a path.
         * @param path The path to search.
         * @param recursive If paths will be recursively searched.
         * @return The files found.
         */
        static std::vector<std::filesystem::path> GetFiles(const std::filesystem::path& path, bool recursive = true);

        /**
         * Finds all the files in a path.
         * @param path The path to search.
         * @param recursive If paths will be recursively searched.
         * @param filter File name pattern.
         * @param formats File formats to search.
         * @return The files found.
         */
        static std::vector<std::filesystem::path> GetFilesWithFilter(const std::filesystem::path& path, bool recursive = true, const std::string& filter = "", const std::vector<std::string>& formats = {});

        /**
         * Gets the FileStats of the file on the path.
         * @param path The path to the file.
         * @return The FileStats of the file on the path.
         */
        static FileStats GetStats(const std::filesystem::path& path);

        /**
         * Gets the FileAttributes of the file on the path.
         * @param path The path to the file.
         * @return The FileAttributes of the file on the path.
         */
        static bitmask::bitmask<FileAttributes> GetAttributes(const std::filesystem::path& path);

        /**
         * Checks that file is a directory and has other directories inside.
         * @param path The path to the file.
         * @returnTrue if path is a directory and has directories inside.
         */
        static bool HasDirectories(const std::filesystem::path& path);

        /**
         * Checks that file is a directory.
         * @param path The path to the file.
         * @return True if path has a directory.
         */
        static bool IsDirectory(const std::filesystem::path& path);

        /**
         * Gets the file extention in the lowercase format.
         * @param path The path to the file.
         * @return The string extension.
         */
        static std::string GetExtension(const std::filesystem::path& path);

        /**
         * Gets the icon string for the specific file format.
         * @param path The path to the file.
         * @return The string with font awesome icon.
         */
        //static std::string GetIcon(const std::filesystem::path& path);

    private:
        static std::unordered_map<std::string, std::string> Extensions;
    };
}