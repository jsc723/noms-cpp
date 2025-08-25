#pragma once
#include "table.h"
/*
type chunkReader interface {
	has(h addr) bool
	hasMany(addrs []hasRecord) bool
	get(h addr, stats *Stats) []byte
	getMany(reqs []getRecord, foundChunks chan *chunks.Chunk, wg *sync.WaitGroup, stats *Stats) bool
	count() uint32
	uncompressedLen() uint64
	extract(chunks chan<- extractRecord)
}
*/
namespace noms {
	class memTable {
	public:
		bool has(const addr& h);
		bool hasMany(std::span<hasRecord> records);
		std::shared_ptr<std::byte[]> get(const addr& h);
		bool getMany(std::span<getRecord> records);
		uint32_t count() const;
		uint64_t uncompressedLen() const;
	};
}

