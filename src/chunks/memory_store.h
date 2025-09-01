#include "chunk_store.h"
#include <unordered_map>
#include "hash/all.h"
#include <mutex>
#include <optional>

namespace nomp {
	class MemoryStoreView;
	// MemoryStorage provides a "persistent" storage layer to back multiple
	// MemoryStoreViews. A MemoryStorage instance holds the ground truth for the
	// root and set of chunks that are visible to all MemoryStoreViews vended by
	// NewView(), allowing them to implement the transaction-style semantics that
	// ChunkStore requires.
	class MemoryStore {
		std::unordered_map<Hash, Chunk, Hash::Hasher> store;
		Hash rootHash;
		mutable std::mutex mtx;

	public:
		interface::IChunkStore newView();

		// Get retrieves the Chunk with the Hash h, returning EmptyChunk if it's not
		// present.
		std::optional<Chunk> get(const Hash &h) {
			std::lock_guard lock(mtx);
			auto it = store.find(h);
			if (it != store.end()) {
				return it->second;
			}
			return std::nullopt;
		}

		// Has returns true if the Chunk with the Hash h is present in ms.data, false
		// if not.
		bool has(const Hash& hash) {
			std::lock_guard lock(mtx);
			return store.contains(hash);
		}

		// Root returns the currently "persisted" root hash of this in-memory store.
		Hash root() {
			std::lock_guard lock(mtx);
			return rootHash;
		}

		// Update checks the "persisted" root against last and, iff it matches,
		// updates the root to newRoot, adds all of newChunks to ms.data, and returns
		// true. Otherwise returns false.
		bool update(const Hash& last, const Hash& newRoot, const std::unordered_map<Hash, Chunk, Hash::Hasher>& newChunks) {
			std::lock_guard lock(mtx);
			if (last != rootHash) {
				return false;
			}
			for (const auto& [hash, chunk] : newChunks) {
				store[chunk.hash()] = chunk;
			}
			rootHash = newRoot;
			return true;
		}
	};

	 //MemoryStoreView is an in-memory implementation of store.ChunkStore. Useful
	 //mainly for tests.
	 //The proper way to get one:
	 //MemoryStore storage;
	 //auto ms = storage.NewView()
	class MemoryStoreView {
		MemoryStore& storage;
		Hash currentRoot;
		std::unordered_map<Hash, Chunk, Hash::Hasher> pending; // chunks to be committed
		mutable std::mutex mtx;

	public:
		explicit MemoryStoreView(MemoryStore& storage) : storage(storage), currentRoot(storage.root()), pending() {}
		
		~MemoryStoreView() = default;
		bool has(const Hash& hash) {
			std::lock_guard lock(mtx);
			return pending.contains(hash) || storage.has(hash);
		}
		std::unique_ptr<HashSet> absent(const HashSet& hashes) {
			std::lock_guard lock(mtx);
			auto absent = std::make_unique<HashSet>();
			for (const auto& h : hashes) {
				if (!has(h)) {
					absent->insert(h);
				}
			}
			return absent;
		}
		std::optional<Chunk> get(const Hash& hash) {
			std::lock_guard lock(mtx);
			auto it = pending.find(hash);
			if (it != pending.end()) {
				return it->second;
			}
			return storage.get(hash);
		}
		std::vector<Chunk> getMany(const HashSet& hashes) {
			std::vector<Chunk> chunks;
			for (const auto& h : hashes) {
				auto chunk = get(h);
				if (chunk.has_value()) {
					chunks.push_back(chunk.value());
				}
			}
			return chunks;
		}
		void put(const Chunk& chunk) {
			std::lock_guard lock(mtx);
			pending[chunk.hash()] = chunk;
		}
		std::string_view version() {
			return nomp::NOMP_VERSION;
		}
		void rebase() {
			std::lock_guard lock(mtx);
			currentRoot = storage.root();
		}
		Hash root() {
			std::lock_guard lock(mtx);
			return currentRoot;
		}
		bool commit(const Hash& newRoot, const Hash& last) {
			std::lock_guard lock(mtx);
			if (last != currentRoot) {
				return false;
			}
			auto success = storage.update(currentRoot, newRoot, pending);
			if (success) {
				pending.clear();
			}
			currentRoot = newRoot;
			return success;
		}
		void close() {
		}
	};

	struct MemoryStoreFactory {
		std::unordered_map<std::string, std::shared_ptr<MemoryStore>> stores;
		std::mutex mtx;
		interface::IChunkStore createStore(const std::string& path) {
			std::lock_guard lock(mtx);
			auto it = stores.find(path);
			if (it == stores.end()) {
				auto store = std::make_shared<MemoryStore>();
				stores[path] = store;
				return store->newView();
			}
			return it->second->newView();
		}
		void shutdown() {
		}
	};
}