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

        /**
         * Gets the icon string for the specific file format.
         * @param filepath The path to the file.
         * @return The string with font awesome icon.
         */
        static const char* GetIcon(const fs::path& filepath);
    };
}
