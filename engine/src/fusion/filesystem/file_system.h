#pragma once

namespace fe {
    inline fs::path operator""_p(const char* str, size_t len) { return fs::path{std::string{str, len}}; }

    class VirtualFileSystem;
    
    class FUSION_API FileSystem : public Module::Registrar<FileSystem> {
    public:
        FileSystem();
        ~FileSystem() override;

        /**
         * Reads a file found by real or partial path with a lambda.
         * @param filepath The path to read.
         * @param handler The lambda with data read from the file.
         */
        static void ReadBytes(const fs::path& filepath, const std::function<void(gsl::span<const std::byte>)>& handler, bool virtual_vs = FUSION_VIRTUAL_FS);

        /**
         * Opens a text file, reads all the text in the file into a string, and then closes the file.
         * @param filepath The path to read.
         * @return The data read from the file.
         */
        static std::string ReadText(const fs::path& filepath, bool virtual_vs = FUSION_VIRTUAL_FS);

        /**
         * Opens a file, write the binary data into the file, and then closes the file.
         * @param filepath The path to write.
         * @param buffer The buffer data.
         * @return True on the success, false otherwise.
         */
        static bool WriteBytes(const fs::path& filepath, gsl::span<const std::byte> buffer, bool virtual_vs = FUSION_VIRTUAL_FS);

        /**
         * Opens a file, write the text string into the file, and then closes the file.
         * @param filepath The path to write.
         * @param text The text string.
         * @return True on the success, false otherwise.
         */
        static bool WriteText(const fs::path& filepath, std::string_view text, bool virtual_vs = FUSION_VIRTUAL_FS);

        /**
         * Gets the file extention in the lowercase format.
         * @param filepath The path to the file.
         * @return The string extension.
         */
        static std::string GetExtension(const fs::path& filepath, bool virtual_vs = FUSION_VIRTUAL_FS);

        /**
         * Gets if the path is found in one of the search paths.
         * @param filepath The path to look for.
         * @return If the path is found in one of the searches.
         */
        static bool IsExists(const fs::path& filepath, bool virtual_vs = FUSION_VIRTUAL_FS);

        /**
        * Checks that file is a directory.
        * @param filepath The path to the file.
        * @return True if path has a directory.
        */
        static bool IsDirectory(const fs::path& filepath, bool virtual_vs = FUSION_VIRTUAL_FS);

        /**
         * Finds all the files in a path.
         * @param filepath The path to search.
         * @param recursive If paths will be recursively searched.
         * @param ext The extension string.
         * @return The files found.
         */
        static std::vector<fs::path> GetFiles(const fs::path& root, bool recursive = false, std::string_view ext = "", bool virtual_vs = FUSION_VIRTUAL_FS);

    private:
        std::unique_ptr<VirtualFileSystem> vfs;
    };
}