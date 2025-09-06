#include <gtest/gtest.h>
#include "hash.h"
#include <string>
#include <cstdio>
#include <algorithm>

using std::string;

TEST(Base32Test, TestBase32Encode) {
	char d[20] = { 0 };

    // Test with all zeros.
    EXPECT_TRUE("00000000000000000000000000000000" == nomp::encode32(d));

    // Test with d[19] = 1.
    d[19] = 1;
    EXPECT_TRUE("00000000000000000000000000000001" == nomp::encode32(d));

    // Test with d[19] = 10.
    d[19] = 10;
    EXPECT_TRUE("0000000000000000000000000000000a" == nomp::encode32(d));

    // Test with d[19] = 20.
    d[19] = 20;
    EXPECT_TRUE("0000000000000000000000000000000k" == nomp::encode32(d));

    // Test with d[19] = 31.
    d[19] = 31;
    EXPECT_TRUE("0000000000000000000000000000000v" == nomp::encode32(d));

    // Test with d[19] = 32.
    d[19] = 32;
    EXPECT_TRUE("00000000000000000000000000000010" == nomp::encode32(d));

    // Test with d[19] = 63.
    d[19] = 63;
    EXPECT_TRUE("0000000000000000000000000000001v" == nomp::encode32(d));

    // Test with d[19] = 64.
    d[19] = 64;
    EXPECT_TRUE("00000000000000000000000000000020" == nomp::encode32(d));

    // Largest value test: all bytes set to 0xff.
	std::fill(std::begin(d), std::end(d), (char)0xff);
    EXPECT_TRUE("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv" == nomp::encode32(d));
}

TEST(Base32Test, TestBase32Decode) {
	char d[20] = { 0 };
    char e[20] = { 0 };
    nomp::decode32(string("00000000000000000000000000000000"), d);
    EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));

    e[19] = 1;
    nomp::decode32(string("00000000000000000000000000000001"), d);
    EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));

    e[19] = 10;
    nomp::decode32(string("0000000000000000000000000000000a"), d);
    EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));

    e[19] = 20;
    nomp::decode32(string("0000000000000000000000000000000k"), d);
    EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));


    e[19] = 31;
    nomp::decode32(string("0000000000000000000000000000000v"), d);
    EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));

    e[19] = 32;
    nomp::decode32(string("00000000000000000000000000000010"), d);
    EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));

    e[19] = 63;
    nomp::decode32(string("0000000000000000000000000000001v"), d);
    EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));

    e[19] = 64;
    nomp::decode32(string("00000000000000000000000000000020"), d);
    EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));

    // Largest value test: all bytes set to 0xff.
    std::fill(std::begin(e), std::end(e), (char)0xff);
    nomp::decode32(string("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv"), d);
	EXPECT_TRUE(std::equal(std::begin(d), std::end(d), std::begin(e)));
}

TEST(Base32Test, TestBase32Roundtrip) {
    char d[20] = { 0 };
    string e = "6292irvasdfsdfasd453fqfghjghjgj3";
    nomp::decode32(e, d);
	string r = nomp::encode32(d);
	EXPECT_EQ(e, r);

    char d2[] = {
        (char)0x62, (char)0x92, (char)0x1a, (char)0x2b, (char)0x5c, (char)0xde, (char)0xf1, (char)0x7a, (char)0x2d, (char)0xe4,
        (char)0x53, (char)0xf4, (char)0x3f, (char)0x67, (char)0x8e, (char)0x3a, (char)0x1b, (char)0x29, (char)0xb1, (char)0xd3
	};
    string e2 = nomp::encode32(d2);
	char r2[20] = { 0 };
	nomp::decode32(e2, r2);
	EXPECT_TRUE(std::equal(std::begin(d2), std::end(d2), std::begin(r2)));
}

TEST(HashTest, TestParseError) {
    auto assertParseError = [](const string& s) {
        try {
            auto h = nomp::Hash::Parse(s);
        }
        catch (const std::invalid_argument& e) {
            return;
        }
        FAIL() << "Expected invalid_argument exception for input: " << s;
    };
    // too short
	assertParseError("foo");

    // too long
	assertParseError("000000000000000000000000000000000");

    // 'w' not valid base32
    assertParseError("00000000000000000000000000000000w");

    // no prefix
    assertParseError("sha1-00000000000000000000000000000000");
    assertParseError("sha2-00000000000000000000000000000000");

    auto r = nomp::Hash::Parse("00000000000000000000000000000000");
	EXPECT_TRUE(r[0] == 0);
}


TEST(TestHash, TestMaybeParse) {
    auto parse = [](const string& s, bool success) {
        auto r = nomp::Hash::MaybeParse(s);
        if (success) {
            EXPECT_TRUE(r.has_value()) << "Expected success=true for " << s;
            if (r.has_value()) {
                EXPECT_EQ(s, r->toString());
            }
        }
        else {
            EXPECT_FALSE(r.has_value()) << "Expected success=false for " << s;
        }
	};

    parse("00000000000000000000000000000000", true);
    parse("00000000000000000000000000000001", true);
    parse("1234567abcdefghvv000000000000001", true);
    parse("", false);
    parse("adsfasdf", false);
    parse("sha2-00000000000000000000000000000000", false);
    parse("0000000000000000000000000000000w", false);
}

TEST(TestHash, TestEqual) {
    auto r0 = nomp::Hash::Parse("00000000000000000000000000000000");
    auto r01 = nomp::Hash::Parse("00000000000000000000000000000000");
    auto r1 = nomp::Hash::Parse("00000000000000000000000000000001");
    EXPECT_EQ(r0, r01);
    EXPECT_EQ(r01, r0);
    EXPECT_NE(r0, r1);
	EXPECT_NE(r1, r0);
}

TEST(TestHash, TestLess) {
    auto r0 = nomp::Hash::Parse("00000000000000000000000000000000");
    auto r01 = nomp::Hash::Parse("00000000000000000000000000000000");
    auto r1 = nomp::Hash::Parse("00000000000000000000000000000001");
	EXPECT_FALSE(r0 < r01);
    EXPECT_FALSE(r01 < r0);
    EXPECT_TRUE(r0 < r1);
	EXPECT_FALSE(r1 < r0);

	auto r2 = nomp::Hash::Parse("000000000000000000000000000000a0");
	EXPECT_TRUE(r1 < r2);
	EXPECT_FALSE(r2 < r1);

    nomp::Hash r00;
	EXPECT_FALSE(r0 < r00);
	EXPECT_FALSE(r00 < r0);
	EXPECT_TRUE(r00 < r1);
}

TEST(TestHash, TestString) {
	auto s = "0123456789abcdefghijklmnopqrstuv";
    auto r = nomp::Hash::Parse(s);
	EXPECT_EQ(s, r.toString());
}

TEST(TestHash, TestOf) {
    string data = "abc";
    auto h = nomp::Hash::Of(std::span{ data.data(), data.size() });
    EXPECT_EQ("rmnjb8cjc5tblj21ed4qs821649eduie", h.toString());
}

TEST(TestHash, TestIsEmpty) {
    nomp::Hash r1;
	EXPECT_TRUE(r1.isEmpty());

    auto r2 = nomp::Hash::Parse("00000000000000000000000000000000");
	EXPECT_TRUE(r2.isEmpty());

    auto r3 = nomp::Hash::Parse("rmnjb8cjc5tblj21ed4qs821649eduie");
	EXPECT_FALSE(r3.isEmpty());
}

TEST(TestHash, TestHashSort) {
    std::vector<nomp::Hash> res{
        nomp::Hash::Parse("000000000000000000000000000000a0"),
        nomp::Hash::Parse("00000000000000000000000000000100"),
        nomp::Hash::Parse("00000000000000000000000000000010"),
        nomp::Hash::Parse("00000000000000000000000000000001"),
    };

    std::vector<nomp::Hash> exp{
        nomp::Hash::Parse("00000000000000000000000000000001"),
        nomp::Hash::Parse("00000000000000000000000000000010"),
        nomp::Hash::Parse("000000000000000000000000000000a0"),
        nomp::Hash::Parse("00000000000000000000000000000100"),
    };

	std::sort(res.begin(), res.end());
	EXPECT_EQ(exp, res);
}
