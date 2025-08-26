#include <gtest/gtest.h>
#include "common.h"
#include <string>
#include <cstdio>

using std::string;

TEST(Base32Test, TestBase32Encode) {
	char d[20] = { 0 };

    // Test with all zeros.
    EXPECT_TRUE("00000000000000000000000000000000" == noms::encode32(d));

    // Test with d[19] = 1.
    d[19] = 1;
    EXPECT_TRUE("00000000000000000000000000000001" == noms::encode32(d));

    // Test with d[19] = 10.
    d[19] = 10;
    EXPECT_TRUE("0000000000000000000000000000000a" == noms::encode32(d));

    // Test with d[19] = 20.
    d[19] = 20;
    EXPECT_TRUE("0000000000000000000000000000000k" == noms::encode32(d));

    // Test with d[19] = 31.
    d[19] = 31;
    EXPECT_TRUE("0000000000000000000000000000000v" == noms::encode32(d));

    // Test with d[19] = 32.
    d[19] = 32;
    EXPECT_TRUE("00000000000000000000000000000010" == noms::encode32(d));

    // Test with d[19] = 63.
    d[19] = 63;
    EXPECT_TRUE("0000000000000000000000000000001v" == noms::encode32(d));

    // Test with d[19] = 64.
    d[19] = 64;
    EXPECT_TRUE("00000000000000000000000000000020" == noms::encode32(d));

    // Largest value test: all bytes set to 0xff.
	std::fill(std::begin(d), std::end(d), (char)0xff);
    EXPECT_TRUE("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv" == noms::encode32(d));
}

TEST(Base32Test, TestBase32Decode) {
	char d[20] = { 0 };
    char e[20] = { 0 };
    noms::decode32(string("00000000000000000000000000000000"), d);
	EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));

	e[19] = 1;
	noms::decode32(string("00000000000000000000000000000001"), d);
	EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));

    e[19] = 10;
    noms::decode32(string("0000000000000000000000000000000a"), d);
    EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));
    for (size_t i = 0; i < 20; i++) {
		printf_s("%02x ", (unsigned char)d[i]);
	}

    e[19] = 20;
    noms::decode32(string("0000000000000000000000000000000k"), d);
    EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));
    

    e[19] = 31;
    noms::decode32(string("0000000000000000000000000000000v"), d);
    EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));

    e[19] = 32;
    noms::decode32(string("00000000000000000000000000000010"), d);
    EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));

    e[19] = 63;
    noms::decode32(string("0000000000000000000000000000001v"), d);
    EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));

    e[19] = 64;
    noms::decode32(string("00000000000000000000000000000020"), d);
    EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));

	// Largest value test: all bytes set to 0xff.
    std::fill(std::begin(e), std::end(e), (char)0xff);
    noms::decode32(string("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv"), d);
	EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));
}