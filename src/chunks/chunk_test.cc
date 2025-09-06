#include <gtest/gtest.h>
#include "chunk.h"
#include "hash/hash.h"
#include <string>

using std::string;

TEST(ChunkTest, TestChunkCreation) {
	string data = "abc";
	nomp::Chunk chunk(std::span{ (std::byte*)data.c_str(), data.size()} );
	EXPECT_EQ(chunk.size(), data.size());
	EXPECT_EQ(chunk.hash().toString(), string("rmnjb8cjc5tblj21ed4qs821649eduie"));
}

TEST(ChunkTest, TestChunkCopyMove) {
	string data = "abc";
	nomp::Chunk chunk1(std::span{ (std::byte*)data.c_str(), data.size() });
	nomp::Chunk chunk2 = chunk1; // copy
	EXPECT_EQ(chunk2.size(), data.size());
	EXPECT_EQ(chunk2.hash().toString(), string("rmnjb8cjc5tblj21ed4qs821649eduie"));
	nomp::Chunk chunk3 = std::move(chunk1); // move
	EXPECT_EQ(chunk3.size(), data.size());
	EXPECT_EQ(chunk3.hash().toString(), string("rmnjb8cjc5tblj21ed4qs821649eduie"));
}

TEST(ChunkTest, TestChunkWriter) {
	nomp::ChunkWriter writer;
	string data1 = "abc";
	string data2 = "defgh";
	writer.write(std::span{ (std::byte*)data1.c_str(), data1.size() });
	writer.write(std::span{ (std::byte*)data2.c_str(), data2.size() });
	auto chunk = writer.getChunk();
	EXPECT_EQ(chunk.size(), data1.size() + data2.size());
	string allData = data1 + data2;
	EXPECT_TRUE(std::equal((std::byte*)allData.c_str(), (std::byte*)allData.c_str() + allData.size(), chunk.data().span().begin()));

	string data3 = "ijklmnop";
	writer.write(std::span{ (std::byte*)data3.c_str(), data3.size() });
	auto chunk2 = writer.getChunk();
	EXPECT_EQ(chunk2.size(), data3.size());
	EXPECT_TRUE(std::equal((std::byte*)data3.c_str(), (std::byte*)data3.c_str() + data3.size(), chunk2.data().span().begin()));
}




