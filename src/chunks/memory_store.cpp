#include "memory_store.h"

namespace nomp {
	interface::IChunkStore MemoryStore::newView() {
		return pro::make_proxy<interface::ChunkStore, MemoryStoreView>(*this);
	}

}