#include "stream.hpp"

#include "physfs.h"

namespace fe {
    using std::streambuf;
    using std::ios_base;

    class FBuffer : public streambuf {
    public:
        explicit FBuffer(PHYSFS_File *file, std::size_t bufferSize = 2048)
            : buffer(bufferSize)
            , file{file} {
            auto end = buffer.data() + bufferSize;
            setg(end, end, end);
            setp(buffer.data(), end);
        }

        ~FBuffer() override {
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

    protected:
        PHYSFS_File* file;
        std::vector<char> buffer;
    };
}

using namespace fe;

PHYSFS_File* OpenWithMode(const std::filesystem::path& filename, FileMode openMode) {
    PHYSFS_File *file = nullptr;

    auto pathStr = filename.string();
    std::replace(pathStr.begin(), pathStr.end(), '\\', '/');

    switch (openMode) {
        case FileMode::Write:
            file = PHYSFS_openWrite(pathStr.c_str());
            break;
        case FileMode::Append:
            file = PHYSFS_openAppend(pathStr.c_str());
            break;
        case FileMode::Read:
            file = PHYSFS_openRead(pathStr.c_str());
    }

    if (file == nullptr)
        throw std::invalid_argument("File could not be found");

    return file;
}

BaseFStream::BaseFStream(PHYSFS_File* file) : file{file} {
    if (file == nullptr) {
        throw std::invalid_argument("Attempted to construct fstream with a NULL file");
    }
}

BaseFStream::~BaseFStream() {
    PHYSFS_close(file);
}

size_t BaseFStream::length() {
    return PHYSFS_fileLength(file);
}

IFStream::IFStream(const std::filesystem::path &filename) 
    : BaseFStream{OpenWithMode(filename, FileMode::Read)}
    , std::istream{new FBuffer{file}} {
}

IFStream::~IFStream() {
    delete rdbuf();
}

OFStream::OFStream(const std::filesystem::path &filename, FileMode writeMode) 
    : BaseFStream{OpenWithMode(filename, writeMode)}
    , std::ostream{new FBuffer{file}} {
}

OFStream::~OFStream() {
    delete rdbuf();
}

FStream::FStream(const std::filesystem::path &filename, FileMode openMode) 
    : BaseFStream{OpenWithMode(filename, openMode)}
    , std::iostream{new FBuffer{file}} {
}

FStream::~FStream() {
    delete rdbuf();
}
