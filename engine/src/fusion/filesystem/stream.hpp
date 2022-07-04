#pragma once

struct PHYSFS_File;

namespace fe {
    enum class FileMode {
        Read, Write, Append
    };

    class BaseFStream {
    public:
        explicit BaseFStream(PHYSFS_File *file);
        virtual ~BaseFStream();

        size_t length();

    protected:
        PHYSFS_File* file;
    };

    class IFStream : public BaseFStream, public std::istream {
    public:
        explicit IFStream(const std::filesystem::path& filename);
        ~IFStream() override;
    };

    class OFStream : public BaseFStream, public std::ostream {
    public:
        explicit OFStream(const std::filesystem::path& filename, FileMode writeMode = FileMode::Write);
        ~OFStream() override;
    };

    class FStream : public BaseFStream, public std::iostream {
    public:
        explicit FStream(const std::filesystem::path& filename, FileMode openMode = FileMode::Read);
        ~FStream() override;
    };
}
