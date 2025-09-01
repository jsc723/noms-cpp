#pragma once
#include "proxy.h"
#include <string>
#include <span>
#include "hash/all.h"
#include "chunks/all.h"

namespace nomp {

	constexpr int ADDR_SIZE = 20;

	struct addr {
		char data[ADDR_SIZE];

		std::string toString() const {
			// to hex string
			std::string result;
			result.reserve(ADDR_SIZE * 2);
			for (int i = 0; i < ADDR_SIZE; ++i) {
				char buf[3];
				sprintf_s(buf, "%02x", static_cast<unsigned char>(data[i]));
				result.append(buf);
			}
			return result;
		}

		// first 8 bytes as big-endian uint64_t
		uint64_t prefix() const {
			uint64_t value = 0;
			for (int i = 0; i < 8; ++i) {
				value = (value << 8) | static_cast<unsigned char>(data[i]);
			}
			return value;
		}

		uint32_t checkSum() const {
			uint32_t sum = 0;
			for (int i = 0; i < ADDR_SIZE; ++i) {
				sum += static_cast<unsigned char>(data[i]);
			}
			return sum;
		}
	};

	struct hasRecord {
		std::shared_ptr<addr> addr;
		uint64_t prefix;
		int      order;
		bool     has;
	};

	struct getRecord {
		std::shared_ptr<addr> addr;
		uint64_t prefix;
		bool     found;
	};

	namespace interface {
		PRO_DEF_MEM_DISPATCH(MemCount, count);
		PRO_DEF_MEM_DISPATCH(MemUncompressedLen, uncompressedLen);

		struct ChunkReader : pro::facade_builder
			::support_copy<pro::constraint_level::nontrivial>
			// TODO
			::build {
		};
	}
	
}




