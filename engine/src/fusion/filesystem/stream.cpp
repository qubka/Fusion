#include "stream.hpp"

/*#include <physfs.h>

namespace fe {
    using std::streambuf;
    using std::ios_base;

    class FBuffer : public streambuf {
    public:
        explicit FBuffer(PHYSFS_File *file, size_t bufferSize = 2048) : file{file}, buffer(bufferSize) {
            auto end = buffer.data() + buffer.size();
            setg(end, end, end);
            setp(buffer.data(), end);
        }

        ~FBuffer() {
            sync();
        }

        NONCOPYABLE(FBuffer);

    private:
        int_type underflow() override {
            if (PHYSFS_eof(file)) {
                return traits_type::eof();
            }

            auto bytesRead = PHYSFS_readBytes(file, buffer.data(), static_cast<PHYSFS_uint32>(buffer.size()));
            if (bytesRead < 1)
                return traits_type::eof();

            setg(buffer.data(), buffer.data(), buffer.data() + static_cast<size_t>(bytesRead));
            return static_cast<int_type>(*gptr());
        }

        pos_type seekoff(off_type pos, ios_base::seekdir dir, ios_base::openmode mode) override {
            switch (dir) {
                case std::ios_base::beg:
                    PHYSFS_seek(file, pos);
                    break;
                case std::ios_base::cur:
                    // Subtract characters currently in buffer from seek position.
                    PHYSFS_seek(file, (PHYSFS_tell(file) + pos) - (egptr() - gptr()));
                    break;
                case std::ios_base::end:
                    PHYSFS_seek(file, PHYSFS_fileLength(file) + pos);
                    break;
            }

            if (mode & std::ios_base::in)
                setg(egptr(), egptr(), egptr());
            if (mode & std::ios_base::out)
                setp(buffer.data(), buffer.data());

            return PHYSFS_tell(file);
        }

        pos_type seekpos(pos_type pos, std::ios_base::openmode mode) override {
            PHYSFS_seek(file, pos);

            if (mode & std::ios_base::in)
                setg(egptr(), egptr(), egptr());
            if (mode & std::ios_base::out)
                setp(buffer.data(), buffer.data());

            return PHYSFS_tell(file);
        }

        int_type overflow(int_type c = traits_type::eof()) override {
            if (pptr() == pbase() && c == traits_type::eof())
                return 0; // no-op

            if (PHYSFS_writeBytes(file, pbase(), static_cast<PHYSFS_uint32>(pptr() - pbase())) < 1)
                return traits_type::eof();

            if (c != traits_type::eof()) {
                if (PHYSFS_writeBytes(file, &c, 1) < 1)
                    return traits_type::eof();
            }

            return 0;
        }

        int sync() override {
            return overflow();
        }

        std::vector<char> buffer;

    protected:
        PHYSFS_File* file;
    };
}

using namespace fe;

PHYSFS_File* OpenWithMode(const fs::path& filepath, FileMode openMode) {
    PHYSFS_File* file = nullptr;

    switch (openMode) {
        case FileMode::Write:
            file = PHYSFS_openWrite(filepath.string().c_str());
            break;
        case FileMode::Append:
            file = PHYSFS_openAppend(filepath.string().c_str());
            break;
        case FileMode::Read:
            file = PHYSFS_openRead(filepath.string().c_str());
    }

    if (file == nullptr)
        throw std::invalid_argument("File could not be found");

    return file;
}

BaseFileStream::BaseFileStream(PHYSFS_File* file) : file{file} {
    if (file == nullptr) {
        throw std::invalid_argument("Attempted to construct fstream with a NULL file");
    }
}

BaseFileStream::~BaseFileStream() {
    PHYSFS_close(file);
}

size_t BaseFileStream::length() {
    return PHYSFS_fileLength(file);
}

IFileStream::IFileStream(const fs::path& filepath)
    : BaseFileStream{OpenWithMode(filepath, FileMode::Read)}
    , std::istream{new FBuffer{file}} {
}

IFileStream::~IFileStream() {
    delete rdbuf();
}

OFileStream::OFileStream(const fs::path& filepath, FileMode writeMode)
    : BaseFileStream{OpenWithMode(filepath, writeMode)}
    , std::ostream{new FBuffer{file}} {
}

OFileStream::~OFileStream() {
    delete rdbuf();
}

FileStream::FileStream(const fs::path &filepath, FileMode openMode)
    : BaseFileStream{OpenWithMode(filepath, openMode)}
    , std::iostream{new FBuffer{file}} {
}

FileStream::~FileStream() {
    delete rdbuf();
}*/