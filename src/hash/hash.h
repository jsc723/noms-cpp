#pragma once
#include <string>
#include <string_view>
#include <span>
#include <memory>
#include <optional>
#include <unordered_set>
#include <stdexcept>
#include "common.h"


namespace nomp {

	// binary to base32 encoding
	// size of data must be divisible by 5
	std::string encode32(std::span<const char> data);

	// base32 decoding to binary
	void decode32(std::span<const char> encoded, std::span<char> data);

	// hash length
	constexpr auto ByteLen = 20;

	// base32 encoded length = ByteLen * 8 / 5
	constexpr auto StringLen = 32; // 5 bytes = 8 chars, 20 bytes = 32 chars

	struct Hash {
	protected:
		char *data;
	public:


		bool isEmpty() const {
			for (size_t i = 0; i < ByteLen; ++i) {
				if (data[i] != 0) return false;
			}
			return true;
		}

		operator std::span<char>() const {
			return std::span{ data, ByteLen };
		}

		operator std::span<const char>() const {
			return std::span{ data, ByteLen };
		}

		operator std::span<std::byte>() const {
			return std::span{ reinterpret_cast<std::byte*>(data), ByteLen };
		}

		operator std::span<const std::byte>() const {
			return std::span{ reinterpret_cast<const std::byte*>(data), ByteLen };
		}

        std::string toString() const {
			return encode32(*this);
		}

		char operator[](size_t i) const {
			return data[i];
		}

		Hash() : data(new char[ByteLen]) {
			std::fill(data, data + ByteLen, 0);
		}
		Hash(const Hash& other) : data(new char[ByteLen]) {
			std::copy(other.data, other.data + ByteLen, data);
		}
		Hash(Hash&& other) noexcept : data(std::move(other.data)) {
			other.data = nullptr;
		}
		Hash& operator=(const Hash& other) {
			if (this != &other) {
				std::copy(other.data, other.data + ByteLen, data);
			}
			return *this;
		}
		Hash& operator=(Hash&& other) noexcept {
			if (this != &other) {
				data = std::move(other.data);
				other.data = nullptr;
			}
			return *this;
		}
		~Hash() {
			delete[] data;
		}

		// copy from d
		Hash(std::span<const char> d) {
			if (d.size() != ByteLen) {
				throw std::invalid_argument("Hash data must be 20 bytes, got " + std::to_string(d.size()));
			}
			data = new char[ByteLen];
			std::copy(d.begin(), d.end(), data);
		}

		// hash of data
		static Hash Of(std::span<const char> data);
		static Hash Of(std::span<const std::byte> data) {
			return Of(std::span{ (const char*)data.data(), data.size() });
		}
		static Hash Of(const ByteSlice &data) {
			return Of(data.span());
		}
		
		// parse from base32 string
		static std::optional<Hash> MaybeParse(std::span<const char> encoded);
		static Hash Parse(const char* encoded) {
			return Parse(std::span{ encoded, strlen(encoded) });
		}
		static Hash Parse(std::span<const char> encoded) {
			auto h = MaybeParse(encoded);
			if (!h.has_value()) {
				throw std::invalid_argument("Invalid hash string: " + std::string(encoded.data(), encoded.size()));
			}
			return h.value();
		}

		bool operator<(const Hash& other) const {
			return std::lexicographical_compare(data, data + ByteLen, other.data, other.data + ByteLen);
		}
		bool less(const Hash& other) const {
			return *this < other;
		}
		bool operator==(const Hash& other) const {
			return std::memcmp(data, other.data, ByteLen) == 0;
		}

		uint64_t prefix() const {
			uint64_t value = 0;
			for (int i = 0; i < 8; ++i) {
				value = (value << 8) | static_cast<unsigned char>(data[i]);
			}
			return value;
		}

		uint32_t checkSum() const {
			uint32_t sum = 0;
			for (int i = 0; i < ByteLen; ++i) {
				sum += static_cast<unsigned char>(data[i]);
			}
			return sum;
		}



		// for unordered_set
		class Hasher {
		public:
			size_t operator()(const Hash& h) const noexcept {
				std::size_t hash_value = 0;
				for (size_t i = 0; i < nomp::ByteLen; ++i) {
					hash_value = hash_value * 31 + h.data[i];
				}
				return hash_value;
			}
		};
	};


	using HashSet = std::unordered_set<Hash, Hash::Hasher>;
	
}
