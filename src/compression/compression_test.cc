#include <gtest/gtest.h>

#include <lz4.h>
#include "compression/compression.h"

using namespace nomp;

TEST(CompresserTest, TestLZ4Compress) {
	nomp::LZ4Compresser compresser;
	std::string originalData = "This is a sample string that we will compress using LZ4. It's designed for speed! aaaaaaaaaaaaaabbbbbbbbbbccccccccbbbbbbbbcccc";
	std::span<byte> originalSpan{ (std::byte*)originalData.data(), originalData.size() };
	auto compressed = compresser.compress(originalSpan);
	EXPECT_FALSE(compressed.empty());
	EXPECT_LT(compressed.size(), originalSpan.size());

	auto decompressed = nomp::LZ4Decompresser().decompress(compressed, originalSpan.size());
	EXPECT_EQ(decompressed.size(), originalSpan.size());
	EXPECT_EQ(decompressed, ByteSlice(originalSpan));
}

TEST(CompresserTest, TestLZ4CompressInplace) {
	nomp::LZ4Compresser compresser;
	std::string originalData = "This is a sample string that we will compress using LZ4. It's designed for speed! aaaaaaaaaaaaaabbbbbbbbbbccccccccbbbbbbbbcccc";
	std::span<byte> originalSpan{ (std::byte*)originalData.data(), originalData.size() };
	int maxCompressedSize = LZ4_compressBound((int)originalSpan.size());
	std::vector<std::byte> compressedBuffer(maxCompressedSize);
	size_t compressedSize = compresser.compressInplace(originalSpan, compressedBuffer);
	EXPECT_GT(compressedSize, 0);
	EXPECT_LT(compressedSize, originalSpan.size());

	nomp::LZ4Decompresser decompresser;
	std::vector<byte> dst(originalSpan.size());
	auto decompressedSize = decompresser.decompressInplace(ByteSlice(std::span<byte>{compressedBuffer.data(), compressedSize}), dst);
	EXPECT_EQ(decompressedSize, originalSpan.size());
	ByteSlice decompressed(std::span{ dst.data(), decompressedSize });
	EXPECT_EQ(decompressed, ByteSlice(originalSpan));
}