#pragma once
#include "proxy.h"
#include <string>
#include <span>
#include "hash/all.h"
#include "chunks/all.h"

namespace nomp {

	constexpr size_t AddrSize = 20; // bytes
	constexpr size_t PrefixSize = 8; // bytes
	constexpr size_t SuffixSize = AddrSize - PrefixSize; // bytes
	constexpr size_t Uint64Size = 8; // bytes
	constexpr size_t Uint32Size = 4; // bytes
	constexpr size_t OrdinalSize = 4; // bytes
	constexpr size_t LengthSize = Uint32Size; // bytes
	constexpr size_t OffsetSize = Uint32Size; // bytes
	constexpr uint64_t MagicNumber = 0xFFB5D8C22463EE50;
	constexpr size_t MagicNumberSize = 8; // bytes
	constexpr size_t FooterSize = Uint32Size + Uint64Size + MagicNumberSize;
	constexpr size_t PrefixTupleSize = PrefixSize + OrdinalSize;
	constexpr size_t CheckSumSize = Uint32Size;
	constexpr size_t MaxChunkSize = 0xffffffff;


	struct hasRecord {
		Hash addr;
		uint64_t prefix;
		int      order;
		bool     has;
	};

	struct getRecord {
		Hash addr;
		ByteSlice data;
		uint64_t prefix;
		bool     found;
	};

	struct extractRecord {
		Hash addr;
		ByteSlice data;
		int err; // non-zero if error
	};

	namespace interface {
		PRO_DEF_MEM_DISPATCH(MemHasMany, hasMany);
		PRO_DEF_MEM_DISPATCH(MemCount, count);
		PRO_DEF_MEM_DISPATCH(MemUncompressedLen, uncompressedLen);
		PRO_DEF_MEM_DISPATCH(MemExtract, extract);

		struct RawChunkReader : pro::facade_builder
			// Returns true iff the value at the address |hash| is contained in the reader.
			::add_convention<MemHas, bool(const Hash& hash)>

			// Returns true if any is absent in the reader.
			::add_convention<MemHasMany, bool(std::span<hasRecord> &records)>

			// Returns the value at the address |hash| if it is contained, return true if hash is found
			::add_convention<MemGet, bool(const Hash& hash, ByteSlice &data)>

			// Returns true if any is absent in the reader.
			::add_convention<MemGetMany, bool(std::span<getRecord> &hashes)>

			// Returns the number of chunks in the reader.
			::add_convention<MemCount, uint32_t()>

			// Returns the total uncompressed length of all chunks in the reader.
			::add_convention<MemUncompressedLen, uint64_t()>

			// Returns the chunks in insertion order
			::add_convention<MemExtract, void (std::vector<extractRecord> &)>

		::build{
		};
		using IRawChunkReader = pro::proxy<RawChunkReader>;
	}
	
}




