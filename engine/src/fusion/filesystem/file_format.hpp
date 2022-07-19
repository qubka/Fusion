#pragma once

namespace fe {
    class FileFormat {
    public:
        FileFormat() = delete;

        /**
         * Validate file format.
         * @param filepath The path to the file.
         * @return True or false.
         */
        static bool IsTextFile(const fs::path& filepath);
        static bool IsAudioFile(const fs::path& filepath);
        static bool IsSceneFile(const fs::path& filepath);
        static bool IsModelFile(const fs::path& filepath);
        static bool IsTextureFile(const fs::path& filepath);
        static bool IsShaderFile(const fs::path& filepath);
        static bool IsTextureStorageFile(const fs::path& filepath);

        /**
         * Gets the icon string for the specific file format.
         * @param filepath The path to the file.
         * @return The string with font awesome icon.
         */
        static const char* GetIcon(const fs::path& filepath);

        /**
         * Checks if the file name is available, if file with the same name exist, generates a new name from the given one.
         * @param filepath The path to the file.
         * @return The unique path.
         */
        static fs::path GetNextFileName(fs::path filepath);
    };
}
