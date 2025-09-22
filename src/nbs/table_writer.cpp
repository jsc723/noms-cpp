#include "table_writer.h"
#include <string>
#include <cstring>
#include <lz4.h>
#include <lz4frame.h>
#include "binary/all.h"
#include <algorithm>

namespace nomp
{
	/*
	* Table: Chunks, Index, Footer
	* 
	* Chunks: Chunk 0, Chunk 1, ..., Chunk N-1
	* 
	* Chunk: [compressed data][crc32]
	* 
	* Index: PrefixTuples, Lengths, Offsets,  Suffixes
	* PrefixTuples: [prefix0][order0][prefix1][order1]...[prefixN-1][orderN-1]
	* Lengths: [len0][len1]...[lenN-1]
	* Offsets: [offset0][offset1]...[offsetN-1]
	* Suffixes: [suffix0][suffix1]...[suffixN-1]
	*/

	uint64_t maxTableSize(uint64_t numChunks, uint64_t totalData)
	{
		auto avgChunkSize = totalData / numChunks;
		if (avgChunkSize > MaxChunkSize) {
			throw std::runtime_error("Average chunk size exceeds maximum chunk size");
		}
		auto maxLZ4Size = LZ4_compressBound((int)avgChunkSize);
		return numChunks * (PrefixTupleSize + LengthSize + OffsetSize + SuffixSize + CheckSumSize + maxLZ4Size) + FooterSize;
	}

	
	// append [compressed data][crc32] and update prefixes
	void TableWriter::addChunk(const Hash& h, const ByteSlice& data)
	{
		if (data.size() == 0) {
			throw std::runtime_error("NBS blocks cannont be zero length");
		}

		const auto compressedSize = compressor->compressInplace(data, buff.subSpan(pos, buff.size() - pos));
		const std::span<std::byte> compressedSpan = buff.subSpan(pos, compressedSize);
		pos += compressedSize;

		totalCompressed += compressedSize;
		totalUncompressed += data.size();

		BigEndian::writeUint32(buff.subSpan(pos), crc32(compressedSpan));
		pos += CheckSumSize;

		prefixes.emplace_back(
			std::make_shared<PrefixIndexRec>(h, prefixes.size(), uint32_t(compressedSize + CheckSumSize))
		);
	}

	void TableWriter::writeIndex()
	{
		std::sort(prefixes.begin(), prefixes.end(), [](const std::shared_ptr<PrefixIndexRec>& a, const std::shared_ptr<PrefixIndexRec>& b) {
			return a->prefix < b->prefix;
			});
		auto lenOffset = pos + (PrefixSize + OrdinalSize) * prefixes.size();
		auto offsetOffset = lenOffset + LengthSize * prefixes.size();
		auto suffixOffset = offsetOffset + OffsetSize * prefixes.size();
		for (const auto& rec : prefixes) {
			// prefix
			BigEndian::writeUint64(buff.subSpan(pos), rec->prefix);
			pos += PrefixSize;
			// order
			BigEndian::writeUint32(buff.subSpan(pos), rec->order);
			pos += OrdinalSize;
			// length
			BigEndian::writeUint32(buff.subSpan(lenOffset + rec->order * LengthSize), rec->size);
			// suffix
			auto suffixPos = suffixOffset + rec->order * SuffixSize;
			std::copy(rec->suffix.begin(), rec->suffix.end(), buff.subSpan(suffixPos).begin());
		}
		// offsets
		uint32_t currentOffset = 0;
		for (size_t i = 0; i < prefixes.size(); ++i) {
			BigEndian::writeUint32(buff.subSpan(offsetOffset + i * OffsetSize), currentOffset);
			currentOffset += BigEndian::uint32(buff.subSpan(lenOffset + i * LengthSize));
		}

		auto suffixLen = SuffixSize * prefixes.size();
		blockHasher.update(buff.subSpan(suffixOffset, suffixLen));
		pos = suffixOffset + suffixLen;
	}

	void TableWriter::writeFooter()
	{
		// chunk count
		BigEndian::writeUint32(buff.subSpan(pos), prefixes.size());
		pos += CheckSumSize;
		// total uncompressed length
		BigEndian::writeUint64(buff.subSpan(pos), totalUncompressed);
		pos += Uint64Size;
		// magic number
		BigEndian::writeUint64(buff.subSpan(pos), MagicNumber);
		pos += MagicNumberSize;
	}


	std::pair<Hash, ByteSlice> TableWriter::finish()
	{
		writeIndex();
		writeFooter();
		const auto tableSize = pos;
		const Hash tableHash = blockHasher.final();
		return { tableHash, buff.subSlice(0, tableSize) };
	}



}