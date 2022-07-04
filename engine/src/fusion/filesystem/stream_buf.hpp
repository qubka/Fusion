#pragma once

struct PHYSFS_File;

namespace fe {
    using std::streambuf;
    using std::ios_base;

    class BaseFileStreambuf {
    public:
        explicit BaseFileStreambuf(PHYSFS_File* file);
        virtual ~BaseFileStreambuf();

    protected:
        PHYSFS_File* file;
        char buf[1024];
    };

    class IFileStreambuf final : public BaseFileStreambuf, public std::streambuf {
    public:
        explicit IFileStreambuf(PHYSFS_File* file);
        ~IFileStreambuf() override;

    protected:
        int underflow() override;
        pos_type seekoff(off_type pos, std::ios_base::seekdir, std::ios_base::openmode) override;
        pos_type seekpos(pos_type pos, std::ios_base::openmode) override;
    };

    class OFileStreambuf final : public BaseFileStreambuf, public std::streambuf {
    public:
        explicit OFileStreambuf(PHYSFS_File* file);
        ~OFileStreambuf() override;

    protected:
        int overflow(int c) override;
        int sync() override;
    };
}
