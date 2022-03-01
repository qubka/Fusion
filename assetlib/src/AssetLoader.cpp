#include "AssetLoader.hpp"

using namespace fe;

bool Asset::SaveBinaryFile(const std::string& path, const AssetFile& file) {
    std::ofstream outfile;
    outfile.open(path, std::ios::binary | std::ios::out);

    outfile.write(file.type, 4);
    uint32_t version = file.version;

    //version
    outfile.write(reinterpret_cast<const char*>(&version), sizeof(uint32_t));

    //json length
    uint32_t length = file.serialized.size();
    outfile.write(reinterpret_cast<const char*>(&length), sizeof(uint32_t));

    //blob length
    uint32_t bloblength = file.binaryBlob.size();
    outfile.write(reinterpret_cast<const char*>(&bloblength), sizeof(uint32_t));

    //json stream
    outfile.write(file.serialized.data(), length);

    //blob data
    outfile.write(file.binaryBlob.data(), file.binaryBlob.size());

    outfile.close();

    return true;
}

bool Asset::LoadBinaryFile(const std::string& path, AssetFile& file) {
    std::ifstream infile;
    infile.open(path, std::ios::binary);

    if (!infile.is_open()) {
        LOG_ERROR << "Error when trying to write file: " << path << std::endl;
        return false;
    }

    //move file cursor to beginning
    infile.seekg(0);

    infile.read(file.type, 4);
    infile.read(reinterpret_cast<char*>(&file.version), sizeof(uint32_t));

    uint32_t serialLen;
    infile.read(reinterpret_cast<char*>(&serialLen), sizeof(uint32_t));

    uint32_t blobLen;
    infile.read(reinterpret_cast<char*>(&blobLen), sizeof(uint32_t));

    file.serialized.resize(serialLen);
    infile.read(file.serialized.data(), serialLen);

    file.binaryBlob.resize(blobLen);
    infile.read(file.binaryBlob.data(), blobLen);

    return true;
}

CompressionMode Asset::parseCompression(const char* f) {
    if (strcmp(f, "LZ4") == 0) {
        return CompressionMode::LZ4;
    } else {
        return  CompressionMode::None;
    }
}
