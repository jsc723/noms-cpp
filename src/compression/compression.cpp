#include "compression.h"
#include <lz4.h>
#include <vector>
#include <exception>

namespace nomp {
	ByteSlice LZ4Compresser::compress(const ByteSlice& src) {
		if (src.size() == 0) {
			return ByteSlice();
		}
		int maxCompressedSize = LZ4_compressBound((int)src.size());
		auto compressedData = std::make_shared<std::byte[]>(maxCompressedSize);
		int compressedSize = LZ4_compress_default(
			(const char*)src.span().data(),
			(char*)compressedData.get(),
			(int)src.size(),
			maxCompressedSize
		);
		if (compressedSize <= 0) {
			throw std::runtime_error("LZ4 compression failed");
		}
		return ByteSlice(compressedData, compressedSize);
	}

	size_t LZ4Compresser::compressInplace(const ByteSlice& src, std::span<std::byte> dest) {
		if (src.size() == 0) {
			return 0;
		}
		int maxCompressedSize = LZ4_compressBound((int)src.size());
		if (dest.size() < (size_t)maxCompressedSize) {
			throw std::runtime_error("Destination buffer too small for LZ4 compression");
		}
		int compressedSize = LZ4_compress_default(
			(const char*)src.span().data(),
			(char*)dest.data(),
			(int)src.size(),
			(int)dest.size()
		);
		if (compressedSize <= 0) {
			throw std::runtime_error("LZ4 compression failed");
		}
		return compressedSize;
	}

	ByteSlice LZ4Decompresser::decompress(const ByteSlice& src, size_t originalSize) {
		if (src.size() == 0) {
			return ByteSlice();
		}
		auto decompressedData = std::make_shared<std::byte[]>(originalSize);
		int decompressedSize = LZ4_decompress_safe(
			(const char*)src.span().data(),
			(char*)decompressedData.get(),
			(int)src.size(),
			(int)originalSize
		);
		if (decompressedSize < 0) {
			throw std::runtime_error("LZ4 decompression failed");
		}
		return ByteSlice(decompressedData, decompressedSize);
	}

	size_t LZ4Decompresser::decompressInplace(const ByteSlice& src, std::span<std::byte> dest) {
		if (src.size() == 0) {
			return 0;
		}
		int decompressedSize = LZ4_decompress_safe(
			(const char*)src.span().data(),
			(char*)dest.data(),
			(int)src.size(),
			(int)dest.size()
		);
		if (decompressedSize < 0) {
			throw std::runtime_error("LZ4 decompression failed");
		}
		return decompressedSize;
	}
}

