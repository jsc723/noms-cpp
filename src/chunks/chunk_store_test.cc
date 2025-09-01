#include <gtest/gtest.h>
#include "chunk_store.h"
#include "memory_store.h"
#include <string>

using nomp::Chunk;
using nomp::Hash;

// 1. Define the list of types to be tested
using ChunkStoreFactories = testing::Types<nomp::MemoryStoreFactory>;

// 2. Define the test fixture as a template class
template <class T>
class ChunkStoreTest : public testing::Test {
protected:
	nomp::interface::IChunkStoreFactory factory;
	ChunkStoreTest() : factory(pro::make_proxy<nomp::interface::ChunkStoreFactory, T>()) {}
};

// 3. Register the test fixture with the list of types
TYPED_TEST_SUITE(ChunkStoreTest, ChunkStoreFactories);

// 4. Define the tests using TYPED_TEST
TYPED_TEST(ChunkStoreTest, CreateAndShutdown) {
	auto store = this->factory->createStore("ns");
	this->factory->shutdown();
}

TYPED_TEST(ChunkStoreTest, TestPut) {
	auto store = this->factory->createStore("ns");
	std::string_view input = "abc";
	nomp::Chunk c = nomp::Chunk::FromStringView(input);
	store->put(c);
	nomp::Hash h = c.hash();

	std::optional<nomp::Chunk> chunk = store->get(h);
	EXPECT_TRUE(chunk.has_value());
	EXPECT_EQ(chunk.value(), c);
}

TYPED_TEST(ChunkStoreTest, TestRoot) {
	auto store = this->factory->createStore("ns");
	auto oldRoot = store->root();
	EXPECT_TRUE(oldRoot.isEmpty());

	auto bogusRoot = nomp::Hash::Parse("8habda5skfek1265pc5d5l1orptn5dr0");
	auto newRoot = nomp::Hash::Parse("8la6qjbh81v85r6q67lqbfrkmpds14lg");

	// Try to update root with bogus oldRoot
	bool result = store->commit(newRoot, bogusRoot);
	EXPECT_FALSE(result);

	// Now do a valid root update
	result = store->commit(newRoot, oldRoot);
	EXPECT_TRUE(result);
}

void assertInputInStore(std::string_view input, const nomp::Hash& h, nomp::interface::IChunkStore &store) {
	std::optional<nomp::Chunk> chunk = store->get(h);
	ASSERT_TRUE(chunk.has_value());
	ASSERT_EQ(chunk.value(), nomp::Chunk::FromStringView(input));
}

void assertInputNotInStore(std::string_view input, const nomp::Hash& h, nomp::interface::IChunkStore &store) {
	std::optional<nomp::Chunk> chunk = store->get(h);
	ASSERT_FALSE(chunk.has_value());
}

TYPED_TEST(ChunkStoreTest, TestCommitPut) {
	std::string name = "ns";
	auto store = this->factory->createStore(name);
	std::string_view input = "abc";
	nomp::Chunk c = nomp::Chunk::FromStringView(input);
	store->put(c);
	auto h = c.hash();

	// Reading it via the API should work...
	assertInputInStore(input, h, store);
	// ...but it shouldn't be persisted yet
	auto store2 = this->factory->createStore(name);
	assertInputNotInStore(input, h, store2);

	store->commit(h, store->root()); // Commit persists Chunks
	assertInputInStore(input, h, store);
	auto store3 = this->factory->createStore(name);
	assertInputInStore(input, h, store3);
}

TYPED_TEST(ChunkStoreTest, TestGetNonExisting) {
	auto store = this->factory->createStore("ns");
	auto h = Hash::Parse("11111111111111111111111111111111");
    auto c = store->get(h);
	EXPECT_FALSE(c.has_value());
}

TYPED_TEST(ChunkStoreTest, TestVersion) {
	auto store = this->factory->createStore("ns");
	auto h = Hash::Parse("11111111111111111111111111111111");
	EXPECT_TRUE(store->commit(h, store->root()));
	EXPECT_EQ(store->version(), std::string(nomp::NOMP_VERSION.data()));
}

TYPED_TEST(ChunkStoreTest, TestCommitUnchangedRoot) {
	auto store1 = this->factory->createStore("ns");
	auto store2 = this->factory->createStore("ns");
	std::string_view input = "abc";
	Chunk c = Chunk::FromStringView(input);
	store1->put(c);
	Hash h = c.hash();

	// Reading c from store1 via the API should work...
	assertInputInStore(input, h, store1);
	// ...but not store2.
	assertInputNotInStore(input, h, store2);

	EXPECT_TRUE(store1->commit(h, store1->root()));
	store2->rebase();
	// Now, reading c from store2 via the API should work...
	assertInputInStore(input, h, store2);
	std::string_view input2 = "def";
	assertInputNotInStore(input2, Chunk::FromStringView(input2).hash(), store2);
}




