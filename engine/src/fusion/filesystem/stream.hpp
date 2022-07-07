#pragma once
/*
struct PHYSFS_File;

namespace fe {
    enum class FileMode {
        Read, Write, Append
    };

    class BaseFileStream {
    public:
        explicit BaseFileStream(PHYSFS_File *file);
        virtual ~BaseFileStream();

        size_t length();

    protected:
        PHYSFS_File* file;
    };

    class IFileStream final : public BaseFileStream, public std::istream {
    public:
        explicit IFileStream(const std::filesystem::path& filename);
        ~IFileStream() override;
    };

    class OFileStream final : public BaseFileStream, public std::ostream {
    public:
        explicit OFileStream(const std::filesystem::path& filename, FileMode writeMode = FileMode::Write);
        ~OFileStream() override;
    };

    class FileStream final : public BaseFileStream, public std::iostream {
    public:
        explicit FileStream(const std::filesystem::path& filename, FileMode openMode = FileMode::Read);
        ~FileStream() override;
    };
}*/
