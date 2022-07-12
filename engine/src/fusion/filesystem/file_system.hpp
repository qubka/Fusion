#pragma once

#include "fusion/core/module.hpp"
#include "fusion/utils/date_time.hpp"

namespace fe {
    //inline fs::path operator""_p(const char* str, size_t len) { return fs::path{std::string{str, len}}; }

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
        static bool Exists(const fs::path& path);

        /**
         * Create a directory.
         * @param path The path to use. All missing parent directories are also created if they don't exist.
         * @return True on the success, false otherwise.
         */
        static bool CreateDirectory(const fs::path& path);

        /**
         * Tell PhysicsFS where it may write files.
         * Set a new write dir. This will override the previous setting.
         * @param path The path to directory.
         * @return True on the success, false otherwise.
         */
        static bool SetWriteDirectory(const fs::path& path);

        /**
         * Reads a file found by real or partial path with a lambda.
         * @param filepath The path to read.
         * @param handler The lambda with data read from the file.
         */
        static void Read(const fs::path& filepath, const SimpleHandler& handler);

        /**
         * Opens a binary file, reads the contents of the file into a byte array, and then closes the file.
         * @param filepath The path to read.
         * @return The data read from the file.
         */
        static std::vector<uint8_t> ReadBytes(const fs::path& filepath);

        /**
         * Opens a text file, reads all the text in the file into a string, and then closes the file.
         * @param filepath The path to read.
         * @return The data read from the file.
         */
        static std::string ReadText(const fs::path& filepath);

        /**
         * Opens a file, write all data into the file, and then closes the file.
         * @param filepath The path to write.
         * @param buffer The buffer data.
         * @param size The size of the buffer.
         * @return True on the success, false otherwise.
         */
        static bool Write(const fs::path& filepath, const void* buffer, size_t size);

        /**
         * Finds all the files in a path.
         * @param path The path to search.
         * @param recursive If paths will be recursively searched.
         * @return The files found.
         */
        static std::vector<fs::path> GetFiles(const fs::path& path, bool recursive = false);

        /**
         * Gets the FileStats of the file on the path.
         * @param path The path to the file.
         * @return The FileStats of the file on the path.
         */
        static FileStats GetStats(const fs::path& path);

        /**
         * Gets the FileAttributes of the file on the path.
         * @param path The path to the file.
         * @return The FileAttributes of the file on the path.
         */
        static bitmask::bitmask<FileAttributes> GetAttributes(const fs::path& path);

        /**
         * Checks that file is a directory.
         * @param path The path to the file.
         * @return True if path has a directory.
         */
        static bool IsDirectory(const fs::path& path);

        /**
         * Gets the file extention in the lowercase format.
         * @param path The path to the file.
         * @return The string extension.
         */
        static std::string GetExtension(const fs::path& path);

        /**
         * Add an archive or directory to the search path.
         * @param path The path to the directory or archive.
         * @param mount Location in the interpolated tree that this archive will be "mounted", in platform-independent notation. NULL or "" is equivalent to "/".
         * @return True on the success, false otherwise.
         */
        static bool Mount(const fs::path& path, const fs::path& mount);

        /**
         * Remove a directory or archive from the search path.
         * @param path The path to the directory or archive.
         * @return True on the success, false otherwise.
         */
        static bool Unmount(const fs::path& path);

        /**
         * Get the current search path.
         * @return Array of null-terminated paths.
         */
        static std::vector<fs::path> getMounted();

    private:
    };
}