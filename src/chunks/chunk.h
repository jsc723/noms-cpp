#pragma once
#include "common.h"
#include "hash/hash.h"
#include <memory>
#include <vector>
#include <optional>

namespace nomp {
	class Chunk {
		Hash r;
		std::shared_ptr<std::byte[]> m_data; // todo: use const std::byte[]
		size_t sz;
	public:
		Chunk() : m_data(std::make_shared<std::byte[]>(0)), sz(0) {}
		Chunk(std::span<const std::byte> data) : m_data(std::make_shared<std::byte[]>(data.size())), sz(data.size()) {
			std::copy(data.begin(), data.end(), this->m_data.get());
			r = Hash::Of(data);
		}
		Chunk(std::span<std::byte> data, const Hash& hash) : m_data(std::make_shared<std::byte[]>(data.size())), r(hash), sz(data.size()) {
			std::copy(data.begin(), data.end(), this->m_data.get());
		}
		Chunk(std::shared_ptr<std::byte[]> data, size_t sz) : m_data(data), sz(sz) {
			r = Hash::Of(std::span{ data.get(), sz });
		}
		Chunk(std::shared_ptr<std::byte[]> data, size_t sz, const Hash& hash) : m_data(data), r(hash), sz(sz) {
		}
		static Chunk FromString(const std::string& str) {
			return Chunk(std::span{ (const std::byte*)str.data(), str.size() });
		}
		static Chunk FromStringView(std::string_view str) {
			return Chunk(std::span{ (const std::byte*)str.data(), str.size() });
		}
		Chunk(const Chunk& other) : r(other.r), m_data(other.m_data), sz(other.sz) {
		}
		Chunk& operator=(const Chunk& other) {
			if (this != &other) {
				r = other.r;
				m_data = other.m_data;
				sz = other.sz;
			}
			return *this;
		}
		Chunk(Chunk&& other) noexcept : r(std::move(other.r)), m_data(std::move(other.m_data)), sz(other.sz) {}
		Chunk& operator=(Chunk&& other) noexcept {
			if (this != &other) {
				r = std::move(other.r);
				m_data = std::move(other.m_data);
				sz = other.sz;
			}
			return *this;
		}
		bool operator==(const Chunk& rhs) const noexcept {
			return r == rhs.r && sz == rhs.sz && std::equal(m_data.get(), m_data.get() + sz, rhs.m_data.get());
		}
		const Hash& hash() const { return r; }
		std::shared_ptr<std::byte[]> data() const { return m_data; }
		size_t size() const { return sz; }

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
