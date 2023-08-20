#pragma once

#include "fusion/filesystem/file_system.h"

namespace fe {
    class FUSION_API VirtualFileSystem {
    public:
        VirtualFileSystem() = default;
        virtual ~VirtualFileSystem() = default;
        NONCOPYABLE(VirtualFileSystem);

        /**
         * Add an archive or directory to the search path.
         * @param path The path to the directory or archive.
         */
        virtual void mount(const fs::path& path, const fs::path& mount) = 0;

        /**
         * Add an archive, contained in a PHYSFE_File handle, to the search path.
         * @param path The path to the directory or archive.
         */
        virtual void unmount(const fs::path& path) = 0;

        /**
         * Reads a file found by real or partial path with a lambda.
         * @param filepath The path to read.
         * @param handler The lambda with data read from the file.
         */
        virtual void readBytes(const fs::path& filepath, const std::function<void(gsl::span<const uint8_t>)>& handler) const = 0;

        /**
         * Opens a text file, reads all the text in the file into a string, and then closes the file.
         * @param filepath The path to read.
         * @return The data read from the file.
         */
        virtual std::string readText(const fs::path& filepath) const = 0;

        /**
         * Opens a file, write the binary data into the file, and then closes the file.
         * @param filepath The path to write.
         * @param buffer The buffer data.
         * @return True on the success, false otherwise.
         */
        virtual bool writeBytes(const fs::path& filepath, gsl::span<const uint8_t> buffer) const = 0;

        /**
         * Opens a file, write the text string into the file, and then closes the file.
         * @param filepath The path to write.
         * @param text The text string.
         * @return True on the success, false otherwise.
         */
        virtual bool writeText(const fs::path& filepath, std::string_view text) const = 0;

        /**
         * Gets if the path is found in one of the search paths.
         * @param filepath The path to look for.
         * @return If the path is found in one of the searches.
         */
        virtual bool isExists(const fs::path& filepath) const = 0;

        /**
         * Checks that file is a directory.
         * @param filepath The path to the file.
         * @return True if path has a directory.
         */
        virtual bool isDirectory(const fs::path& filepath) const = 0;

        /**
         * Finds all the files in a path.
         * @param filepath The path to search.
         * @param recursive If paths will be recursively searched.
         * @param ext The extension string.
         * @return The files found.
         */
        virtual std::vector<fs::path> getFiles(const fs::path& filepath, bool recursive = false, std::string_view ext = "") const = 0;
    };
}