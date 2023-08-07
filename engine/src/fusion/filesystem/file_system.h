#pragma once

#define PHYSFS 0

namespace fe {
    inline fs::path operator""_p(const char* str, size_t len) { return fs::path{std::string{str, len}}; }

#if PHYSFS
    enum class FileType : unsigned char {
        Regular,      /**< a normal file */
        Directory,    /**< a directory */
        Symlink,      /**< a symlink */
        Other         /**< something completely different like a device */
    };

    struct FileStats {
        size_t filesize;     /**< size in bytes, -1 for non-files and unknown */
        DateTime modtime;    /**< last modification time */
        DateTime createtime; /**< like modtime, but for file creation time */
        DateTime accesstime; /**< like modtime, but for file access time */
        FileType filetype;   /**< File? Directory? Symlink? */
        bool readonly;       /**< non-zero if read only, zero if writable. */
    };

    enum class FileAttributes : unsigned char {
        ReadOnly = 1,
        Regular = 2,
        Directory = 4,
        Symlink = 8,
    };
    BITMASK_DEFINE_MAX_ELEMENT(FileAttributes, Symlink);
#endif

    class FUSION_API FileSystem : public Module::Registrar<FileSystem> {
    public:
        FileSystem();
        ~FileSystem() override;

        /**
         * Reads a file found by real or partial path with a lambda.
         * @param filepath The path to read.
         * @param handler The lambda with data read from the file.
         */
        static void ReadBytes(const fs::path& filepath, const std::function<void(gsl::span<const std::byte>)>& handler);

        /**
         * Opens a text file, reads all the text in the file into a string, and then closes the file.
         * @param filepath The path to read.
         * @return The data read from the file.
         */
        static std::string ReadText(const fs::path& filepath);

        /**
         * Opens a file, write the binary data into the file, and then closes the file.
         * @param filepath The path to write.
         * @param buffer The buffer data.
         * @return True on the success, false otherwise.
         */
        static bool WriteBytes(const fs::path& filepath, gsl::span<const std::byte> buffer);

        /**
         * Opens a file, write the text string into the file, and then closes the file.
         * @param filepath The path to write.
         * @param text The text string.
         * @return True on the success, false otherwise.
         */
        static bool WriteText(const fs::path& filepath, std::string_view text);

        /**
         * Gets the file extention in the lowercase format.
         * @param filepath The path to the file.
         * @return The string extension.
         */
        static std::string GetExtension(const fs::path& filepath);

        /**
         * Return the paths of all files that have the specified extension
         * in the specified directory and all subdirectories.
         * @param root The path to the root folder.
         * @param ext The extension string.
         * @return The file array.
         */
        static std::vector<fs::path> GetFilesInPath(const fs::path& root, const std::string& ext);

        static bool IsExists(const fs::path& filepath);
    };
}