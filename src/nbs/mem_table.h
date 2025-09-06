#pragma once
#include "table.h"
#include <unordered_map>
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
namespace nomp {
	class MemTable {
		std::unordered_map<Hash, ByteSlice, Hash::Hasher> table;
		std::vector<hasRecord> order; //insertion order

		uint64_t maxData;
		uint64_t totalData;
	public:
		MemTable(int maxData = 1LL << 20) : maxData(maxData), totalData(0) {
		}
		bool addChunk(const Hash& h, const ByteSlice &data);
		bool has(const Hash& hash) {
			return table.count(hash) > 0;
		}
		bool hasMany(std::span<hasRecord> records) {
			bool remaining = false;
			for (auto& rec : records) {
				if (rec.has) {
					continue;
				}
				if (has(rec.addr)) {
					rec.has = true;
				}
				else {
					remaining = true;
				}
			}
			return remaining;
		}
		bool get(const Hash& h, ByteSlice &data) {
			auto it = table.find(h);
			if (it != table.end()) {
				data = it->second;
				return true;
			}
			return false;
		}
		bool getMany(std::span<getRecord> &records) {
			bool remaining = false;
			for (auto& rec : records) {
				if (rec.found) {
					continue;
				}
				rec.found = get(rec.addr, rec.data);
				if (!rec.found) {
					remaining = true;
				}
			}
			return remaining;
		}
		uint32_t count() const {
			return uint32_t(order.size());
		}
		uint64_t uncompressedLen() const {
			return totalData;
		}
		void extract(std::vector<extractRecord>& out) {
			for (auto& rec : order) {
				auto it = table.find(rec.addr);
				if (it == table.end()) {
					throw std::runtime_error("Inconsistent MemTable state, [" + rec.addr.toString() + "] not found");
				}
				out.emplace_back(extractRecord{
					rec.addr,
					it->second,
					0
				});
			}
		}


	};
}

