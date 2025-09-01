#pragma once
#include "common.h"
#include "chunk.h"
#include <optional>
#include <vector>

namespace nomp {
	namespace interface {
		PRO_DEF_MEM_DISPATCH(MemHas, has);
		PRO_DEF_MEM_DISPATCH(MemAbsent, absent);
		PRO_DEF_MEM_DISPATCH(MemGet, get);
		PRO_DEF_MEM_DISPATCH(MemGetMany, getMany);
		PRO_DEF_MEM_DISPATCH(MemPut, put);
		PRO_DEF_MEM_DISPATCH(MemVersion, version);
		PRO_DEF_MEM_DISPATCH(MemRebase, rebase);
		PRO_DEF_MEM_DISPATCH(MemRoot, root);
		PRO_DEF_MEM_DISPATCH(MemCommit, commit);
		PRO_DEF_MEM_DISPATCH(MemStats, stats);
		PRO_DEF_MEM_DISPATCH(MemStatsSummary, statsSummary);

		// ChunkStore is the core storage abstraction in noms. We can put data
		// anyplace we have a ChunkStore implementation for.
		struct ChunkStore : pro::facade_builder
			// Returns true iff the value at the address |h| is contained in the store.
			
			::add_convention<MemHas, bool(const Hash& hash)>
			// Returns the subset of |hashes| that are not contained in the store.
			::add_convention<MemAbsent, std::unique_ptr<HashSet>(const HashSet& hashes)>
			
			// Returns the value at the address |h| if it is contained in the store.
			::add_convention<MemGet, std::optional<Chunk>(const Hash& hash)>
			
			// Returns the values at the addresses in |hashes| that are contained in the store.
			::add_convention<MemGetMany, std::vector<Chunk>(const HashSet& hashes)>
			
			// Stores the value |chunk| at the address |chunk.hash()|.
			::add_convention<MemPut, void(const Chunk& chunk)>
			
			// Returns the NompVersion with which this ChunkSource is compatible.
			::add_convention<MemVersion, std::string_view()>
			
			// Rebase brings this ChunkStore into sync with the persistent storage's current root.
			::add_convention<MemRebase, void()>
			
			// Root returns the root of the database as of the time the ChunkStore was opened or the most recent call to Rebase.
			::add_convention<MemRoot, Hash()>
			
			// Commit atomically attempts to persist all novel Chunks and update the
			// persisted root hash from last to current (or keeps it the same).
			// If last doesn't match the root in persistent storage, returns false.
			::add_convention<MemCommit, bool(const Hash& newRoot, const Hash& last)>
			
			// Stats may return some kind of struct that reports statistics about the
			// ChunkStore instance. The type is implementation-dependent, and impls
			// may return nil
			//::add_convention<MemStats, std::string()>
			
			// StatsSummary may return a string containing summarized statistics for
			// this ChunkStore. It must return "Unsupported" if this operation is not
			// supported.
			//::add_convention<MemStatsSummary, std::string()>
			
			// Close tears down any resources in use by the implementation. After
			// Close(), the ChunkStore may not be used again. It is NOT SAFE to call
			// Close() concurrently with any other ChunkStore method; behavior is
			// undefined and probably crashy.
			::add_convention<MemClose, void()>
			::build {
		};
		using IChunkStore = pro::proxy<ChunkStore>;

		PRO_DEF_MEM_DISPATCH(MemCreateStore, createStore);
		PRO_DEF_MEM_DISPATCH(MemShutdown, shutdown);

		struct ChunkStoreFactory : pro::facade_builder
			::add_convention<MemCreateStore, IChunkStore(const std::string& path)>
			::add_convention<MemShutdown, void()>
			::build {
		};
		using IChunkStoreFactory = pro::proxy<ChunkStoreFactory>;
		
	}
}
