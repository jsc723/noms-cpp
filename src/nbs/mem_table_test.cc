#include <gtest/gtest.h>

#include "mem_table.h"
#include "table_writer.h"
#include <memory>

using namespace nomp;

static void assertChunksInReader(const std::vector<nomp::Chunk>& chunks, nomp::interface::IRawChunkReader reader) {
	for (const auto& chunk : chunks) {
		EXPECT_TRUE(reader->has(chunk.hash()));
	}
}

static auto buildTable(std::span<const nomp::Chunk> chunks) {
	auto totalData = uint64_t(0);
	for (const auto& chunk : chunks) {
		totalData += chunk.size();
	}
	TableWriter tw(chunks.size(), totalData);
	for (const auto& chunk : chunks) {
		tw.addChunk(chunk.hash(), chunk.data());
	}
	return tw.finish();
}

static std::span<Chunk> sliceChunks(std::vector<Chunk>& chunks, size_t start, size_t end) {
	if (start >= chunks.size() || end > chunks.size() || start >= end) {
		throw std::out_of_range("Invalid slice indices");
	}
	return std::span<Chunk>(chunks.data() + start, end - start);
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

TEST(MemTableTest, TestMemTableWrite) {
	MemTable mt(1024);
	std::vector<std::string> datas {
		"hello2",
		"goodbye2",
		"badbye2"
	};
	std::vector<nomp::Chunk> chunks;
	for (const auto& data : datas) {
		chunks.emplace_back(nomp::Chunk::FromStringView(data));
	}
	for (const auto& chunk : chunks) {
		EXPECT_TRUE(mt.addChunk(chunk.hash(), chunk.data()));
	}

	auto [hash1, td1] = buildTable(sliceChunks(chunks, 1, 2));
	auto [hash2, td2] = buildTable(sliceChunks(chunks, 2, chunks.size()));
	EXPECT_NE(hash1, hash2);
}

