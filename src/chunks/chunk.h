#pragma once
#include "common.h"
#include "hash/hash.h"
#include "bytes.h"
#include <memory>
#include <vector>
#include <optional>

namespace nomp {
	class Chunk {
		ByteSlice m_data;
		Hash r;
	public:
		Chunk() {}
		Chunk(std::span<std::byte> data) : m_data(data) {
			r = m_data.hash();
		}
		Chunk(std::span<std::byte> data, const Hash& hash) : m_data(data), r(hash) {
		}
		Chunk(std::shared_ptr<std::byte[]> data, size_t sz) : m_data(data, sz) {
			r = m_data.hash();
		}
		Chunk(std::shared_ptr<std::byte[]> data, size_t sz, const Hash& hash) : m_data(data, sz), r(hash) {
		}
		static Chunk FromString(const std::string& str) {
			return Chunk(std::span{ (std::byte*)str.data(), str.size() });
		}
		static Chunk FromStringView(std::string_view str) {
			return Chunk(std::span{ (std::byte*)str.data(), str.size() });
		}
		Chunk(const Chunk& other) : r(other.r), m_data(other.m_data) {
		}
		Chunk& operator=(const Chunk& other) {
			if (this != &other) {
				r = other.r;
				m_data = other.m_data;
			}
			return *this;
		}
		Chunk(Chunk&& other) noexcept : r(std::move(other.r)), m_data(std::move(other.m_data)) {}
		Chunk& operator=(Chunk&& other) noexcept {
			if (this != &other) {
				r = std::move(other.r);
				m_data = std::move(other.m_data);
			}
			return *this;
		}
		bool operator==(const Chunk& rhs) const noexcept {
			return r == rhs.r && m_data == rhs.m_data;
		}
		const Hash& hash() const { return r; }
		ByteSlice data() const { return m_data; }
		size_t size() const { return m_data.size(); }

		void serialize(interface::IWriter writer);
		static std::optional<Chunk> deserialize(interface::IReader reader);
	};

	class ChunkWriter {
		std::vector<std::byte> buffer;
	public:
		int write(std::span<const std::byte> data) {
			buffer.insert(buffer.end(), data.begin(), data.end());
			return (int)data.size();
		}
		Chunk getChunk() {
			Chunk c(std::span{ buffer.data(), buffer.size() });
			buffer.clear();
			return c;
		}
	};
	
}
