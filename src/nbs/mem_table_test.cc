#include <gtest/gtest.h>

#include "mem_table.h"
#include <memory>

static void assertChunksInReader(const std::vector<nomp::Chunk>& chunks, nomp::interface::IRawChunkReader reader) {
	for (const auto& chunk : chunks) {
		EXPECT_TRUE(reader->has(chunk.hash()));
	}
}

TEST(MemTableTest, TestMemTableAddHasGetChunk) {
	nomp::MemTable table(1024);

	std::vector<std::string> datas {
		"chunk1",
		"chunk2",
		"chunk3"
	};
	std::vector<nomp::Chunk> chunks;
	for (const auto& data : datas) {
		chunks.emplace_back(nomp::Chunk::FromStringView(data));
	}

	for (const auto& chunk : chunks) {
		EXPECT_TRUE(table.addChunk(chunk.hash(), chunk.data()));
	}

	assertChunksInReader(chunks, pro::make_proxy<nomp::interface::RawChunkReader>(table));

	auto notPresent = nomp::Chunk::FromString(std::string("notpresent"));
	EXPECT_FALSE(table.has(notPresent.hash()));
	nomp::ByteSlice out;
	EXPECT_FALSE(table.get(notPresent.hash(), out));
	EXPECT_TRUE(out.empty());


	EXPECT_EQ(table.count(), datas.size());
}

TEST(MemTableTest, TestMemTableAddOverflowChunk) {
		nomp::MemTable table(12); // small maxData to trigger overflow
	std::vector<std::string> datas {
		"chunk1",
		"chunk02",
		"chunk3",
		"c"
	};
	std::vector<nomp::Chunk> chunks;
	for (const auto& data : datas) {
		chunks.emplace_back(nomp::Chunk::FromStringView(data));
	}
	EXPECT_TRUE(table.addChunk(chunks[0].hash(), chunks[0].data()));
	EXPECT_TRUE(table.has(chunks[0].hash()));
	// This chunk should cause overflow and be rejected
	EXPECT_FALSE(table.addChunk(chunks[1].hash(), chunks[1].data()));
	EXPECT_FALSE(table.has(chunks[1].hash()));
	// Adding a smaller chunk should succeed
	EXPECT_TRUE(table.addChunk(chunks[2].hash(), chunks[2].data()));
	EXPECT_TRUE(table.has(chunks[2].hash()));
	// Adding another small chunk should fail
	EXPECT_FALSE(table.addChunk(chunks[3].hash(), chunks[3].data()));
	EXPECT_FALSE(table.has(chunks[3].hash()));
	EXPECT_EQ(table.count(), 2); // Only two chunks should be present
}

