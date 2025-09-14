#include "mem_table.h"
#include "hash/all.h"
#include <exception>

namespace nomp {

	static void checkMemTable() {
		auto p = pro::make_proxy<interface::RawChunkReader, MemTable>();
	}
	bool MemTable::addChunk(const Hash& h, const ByteSlice &data) {
		if (data.size() == 0) {
			throw std::runtime_error("NBS blocks cannont be zero length");
		}

		if (has(h)) {
			return true;
		}

		if (totalData + data.size() > maxData) {
			return false;
		}

		totalData += data.size();
		table[h] = data;
		order.emplace_back(hasRecord{
			h,
			h.prefix(),
			(int)order.size(),
			false
		});
		return true;

	}
}