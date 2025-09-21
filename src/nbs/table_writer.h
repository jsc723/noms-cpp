#pragma once 

#include "common.h"
#include "table.h"
#include "compression/compression.h"
#include <array>
#include <utility>

namespace nomp {
	namespace interface {
	}


	struct PrefixIndexRec {
		uint64_t prefix;
		std::array<std::byte, SuffixSize> suffix;
		uint32_t order;
		uint32_t size;
		PrefixIndexRec(const Hash& h, uint32_t order, uint32_t size) :
			prefix(h.prefix()), order(order), size(size) {
			std::span<const byte> sp = h;
			std::copy(sp.begin() + PrefixSize, sp.end(), suffix.begin());
		}
	};
	
	uint64_t maxTableSize(uint64_t numChunks, uint64_t totalData);
	class TableWriter {
		ByteSlice buff;
		uint64_t pos;
		uint64_t totalCompressed;
		uint64_t totalUncompressed;
		std::vector<std::shared_ptr<PrefixIndexRec>> prefixes;
		Hasher blockHasher;

		interface::ICompresser compressor;
		
		void writeIndex();
		void writeFooter();
	public:
		TableWriter(uint64_t numChunks, uint64_t totalData, interface::ICompresser comp):
			compressor(comp),
			buff(maxTableSize(numChunks, totalData)),
			pos(0),
			totalCompressed(0),
			totalUncompressed(0)
		{}

		TableWriter(uint64_t numChunks, uint64_t totalData) :
			TableWriter(numChunks, totalData, 
				interface::ICompresser(pro::make_proxy<interface::Compresser, LZ4Compresser>()))
		{
		}

		void addChunk(const Hash& h, const ByteSlice& data);
		std::pair<Hash, ByteSlice> finish();
	};
	
}