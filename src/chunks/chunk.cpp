#include "chunk.h"
#include "binary/binary.h"


namespace nomp {
	static void checkChunkWriter() {
		pro::make_proxy<interface::Writer, ChunkWriter>();
	}

	/*
	    Chunk Serialization:
			Chunk 0
			Chunk 1
			 ..
			Chunk N

		Chunk:
			Hash // 20 bytes
			Len // 4 bytes
			Data // Len bytes
	*/
	void Chunk::serialize(interface::IWriter writer) {
		writer->write(r);
		// Because of chunking at higher levels, no chunk should never be more than 4GB
		BigEndian::writeUint32(writer, static_cast<uint32_t>(size()));
		int n = writer->write(m_data.span());
		if (n != (int)size()) {
			throw std::runtime_error("Failed to write chunk data, expected " + std::to_string(size()) + " bytes, wrote " + std::to_string(n));
		}
	}

	std::optional<Chunk> Chunk::deserialize(interface::IReader reader)
	{
		nomp::Hash h;
		int n = reader->read(h);
		if (n != (int)nomp::ByteLen) {
			if (n == 0) return std::nullopt; // EOF
			throw std::runtime_error("Failed to read chunk hash, expected " + std::to_string(nomp::ByteLen) + " bytes, got " + std::to_string(n));
		}
		uint32_t sz;
		BigEndian::readUint32(reader, sz);
		auto data = std::make_shared<std::byte[]>(sz);
		n = reader->read(std::span{ data.get(), sz });
		if (n != (int)sz) {
			throw std::runtime_error("Failed to read chunk data, expected " + std::to_string(sz) + " bytes, got " + std::to_string(n));
		}
		return Chunk(data, sz, h);
	}



}