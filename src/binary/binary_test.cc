#include <gtest/gtest.h>
#include "binary.h"

TEST(BinaryTest, TestBigEndian) {
	uint64_t v64 = 0x0123456789ABCDEF;
	std::byte buf64[8];
	nomp::BigEndian::writeUint64(std::span{buf64, 8}, v64);
	EXPECT_EQ(buf64[0], std::byte{0x01});
	EXPECT_EQ(buf64[1], std::byte{0x23});
	EXPECT_EQ(buf64[2], std::byte{0x45});
	EXPECT_EQ(buf64[3], std::byte{0x67});
	EXPECT_EQ(buf64[4], std::byte{0x89});
	EXPECT_EQ(buf64[5], std::byte{0xAB});
	EXPECT_EQ(buf64[6], std::byte{0xCD});
	EXPECT_EQ(buf64[7], std::byte{0xEF});
	uint64_t v64_read = nomp::BigEndian::uint64(std::span{buf64, 8});
	EXPECT_EQ(v64, v64_read);

	uint32_t v32 = 0x89ABCDEF;
	std::byte buf32[4];
	nomp::BigEndian::writeUint32(std::span{buf32, 4}, v32);
	EXPECT_EQ(buf32[0], std::byte{0x89});
	EXPECT_EQ(buf32[1], std::byte{0xAB});
	EXPECT_EQ(buf32[2], std::byte{0xCD});
	EXPECT_EQ(buf32[3], std::byte{0xEF});
	uint32_t v32_read = nomp::BigEndian::uint32(std::span{buf32, 4});
	EXPECT_EQ(v32, v32_read);

	uint16_t v16 = 0xCDEF;
	std::byte buf16[2];
	nomp::BigEndian::writeUint16(std::span{ buf16, 2 }, v16);
	EXPECT_EQ(buf16[0], std::byte{ 0xCD });
	EXPECT_EQ(buf16[1], std::byte{ 0xEF });
	uint16_t v16_read = nomp::BigEndian::uint16(std::span{ buf16, 2 });
	EXPECT_EQ(v16, v16_read);
}