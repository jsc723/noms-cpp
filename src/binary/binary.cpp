#include "binary.h"
#include <string>
namespace nomp {
	static void checkBinary() {
		pro::make_proxy<interface::Encoder, BigEndian>();
	}

    uint32_t crc32(const std::byte* data, size_t length) {
        uint32_t crc = 0xFFFFFFFF;
        for (size_t i = 0; i < length; ++i) {
            crc ^= static_cast<uint32_t>(data[i]);
            for (int j = 0; j < 8; ++j) {
                if (crc & 1) {
                    crc = (crc >> 1) ^ 0xEDB88320;
                }
                else {
                    crc >>= 1;
                }
            }
        }
        return ~crc;
    }
    uint32_t crc32(std::span<const std::byte> data) {
        return crc32(data.data(), data.size());
    }
}
