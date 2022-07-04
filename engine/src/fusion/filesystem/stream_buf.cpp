#include "stream_buf.hpp"

#include <physfs.h>

using namespace fe;

BaseFileStreambuf::BaseFileStreambuf(PHYSFS_File* file) : file{file}, buf{} {
}

BaseFileStreambuf::~BaseFileStreambuf() {
    PHYSFS_close(file);
}

IFileStreambuf::IFileStreambuf(PHYSFS_File* file) : BaseFileStreambuf{file} {
}

IFileStreambuf::~IFileStreambuf() {
}

int IFileStreambuf::underflow() {
    if (PHYSFS_eof(file)) {
        return traits_type::eof();
    }
    PHYSFS_sint64 bytesread = PHYSFS_readBytes(file, buf, sizeof(buf));
    if (bytesread <= 0) {
        return traits_type::eof();
    }
    setg(buf, buf, buf + bytesread);
    return buf[0];
}

IFileStreambuf::pos_type IFileStreambuf::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode mode) {
    off_type pos = off;
    PHYSFS_sint64 ptell = PHYSFS_tell(file);
    switch (dir) {
        case std::ios_base::beg:
            break;
        case std::ios_base::cur:
            if (off == 0)
                return static_cast<pos_type> (ptell) - static_cast<pos_type> (egptr() - gptr());
            pos += static_cast<off_type>(ptell) - static_cast<off_type> (egptr() - gptr());
            break;
        case std::ios_base::end:
            pos += static_cast<off_type>(PHYSFS_fileLength(file));
            break;
        default:
            assert(false);
            return {off_type(-1)};
    }
    return seekpos(static_cast<pos_type>(pos), mode);
}

IFileStreambuf::pos_type IFileStreambuf::seekpos(std::fpos<mbstate_t> pos, std::ios_base::openmode mode) {
    if (PHYSFS_seek(file, static_cast<PHYSFS_uint64>(pos)) == 0) {
        return {off_type(-1)};
    }
    // the seek invalidated the buffer
    setg(buf, buf, buf);
    return pos;
}

OFileStreambuf::OFileStreambuf(PHYSFS_File* file) : BaseFileStreambuf{file} {
    setp(buf, buf + sizeof(buf));
}

OFileStreambuf::~OFileStreambuf() {
    sync();
}

int OFileStreambuf::overflow(int c) {
    char c2 = static_cast<char>(c);
    if (pbase() == pptr())
        return 0;
    size_t size = pptr() - pbase();
    PHYSFS_sint64 res = PHYSFS_writeBytes(file, pbase(), size);
    if (res <= 0)
        return traits_type::eof();
    if (c != traits_type::eof()) {
        PHYSFS_sint64 res_ = PHYSFS_writeBytes(file, &c2, 1);
        if (res_ <= 0)
            return traits_type::eof();
    }
    setp(buf, buf + res);
    return 0;
}

int OFileStreambuf::sync() {
    return overflow(traits_type::eof());
}
