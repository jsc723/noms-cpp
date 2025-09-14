#include "compression.h"
#include <lz4.h>
#include <vector>
#include <exception>
nomp::ByteSlice nomp::LZ4Compressor::encode(ByteSlice data)
{
    auto sp = data.span();
    const int bufSz = LZ4_COMPRESSBOUND(sp.size());
    std::shared_ptr buf = std::make_shared<std::byte[]>(bufSz);
    int compressedSize = LZ4_compress_default(
        (const char *)sp.data(),
        (char *)buf.get(),
        sp.size(),
        bufSz
    );
    if (compressedSize < 0) {
        throw std::runtime_error("LZ4 compression failed");
    }
    return ByteSlice(buf, bufSz);
}

nomp::ByteSlice nomp::LZ4Compressor::decode(ByteSlice data)
{
    auto sp = data.span();
    const int bufSz = sp.size();
    std::shared_ptr buf = std::make_shared<std::byte[]>(bufSz);
    int decompressedSize = LZ4_decompress_safe(
        (const char *)sp.data(),
        (char*)buf.get(),
        sp.size(),
        bufSz
    );
    if (decompressedSize < 0) {
        throw std::runtime_error("LZ4 decompression failed");
    }
    return ByteSlice(buf, bufSz);
}

