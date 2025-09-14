#include "table_writer.h"
#include <string>
#include <cstring>
#include <lz4.h>
#include <lz4frame.h>

namespace nomp
{
	static void checkCompress() {
		std::string originalData = "This is a sample string that we will compress using LZ4. It's designed for speed!";
		std::vector<char> compressedData(LZ4_COMPRESSBOUND(originalData.length()));

		// --- Basic Compression ---
		int compressedSize = LZ4_compress_default(
			originalData.c_str(),
			compressedData.data(),
			originalData.length(),
			compressedData.size()
		);

		// --- Basic Decompression ---
		std::vector<char> decompressedData(originalData.length()); // Allocate space for decompressed data
		int decompressedSize = LZ4_decompress_safe(
			compressedData.data(),
			decompressedData.data(),
			compressedSize,
			decompressedData.size()
		);
		std::string decompressedString(decompressedData.data(), decompressedSize);
	}
}