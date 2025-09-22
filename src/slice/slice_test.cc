#include <gtest/gtest.h>
#include "common.h"
#include <vector>
#include <string>

using namespace nomp;

TEST(SliceTest, TestByteSliceFromString) {
	std::string str = "Hello, World!";
	ByteSlice slice = ByteSlice(str);
	EXPECT_EQ(slice.size(), str.size());
	EXPECT_EQ(std::string((char*)slice.span().data(), slice.size()), str);
}

TEST(SliceTest, TestSliceCopyMove) {
	std::string str = "Hello, World!";
	ByteSlice slice1 = ByteSlice(str);
	EXPECT_EQ(slice1.size(), str.size());
	EXPECT_EQ(std::string((char*)slice1.span().data(), slice1.size()), str);
	ByteSlice slice2 = slice1; // copy
	EXPECT_EQ(slice2.size(), str.size());
	EXPECT_EQ(std::string((char*)slice2.span().data(), slice2.size()), str);

	auto slice2Editor = slice2.edit();
	slice2Editor[0] = (std::byte)'h';
	EXPECT_EQ(std::string((char*)slice2.span().data(), slice2.size()), "hello, World!");
	// original will also change
	EXPECT_EQ(std::string((char*)slice1.span().data(), slice1.size()), "hello, World!");

	ByteSlice sliceRealCopy = slice1.copy();
	auto sliceRealCopyEditor = sliceRealCopy.edit();
	sliceRealCopyEditor[1] = (std::byte)'E';
	EXPECT_EQ(std::string((char*)sliceRealCopy.span().data(), sliceRealCopy.size()), "hEllo, World!");
	// original will not change
	EXPECT_EQ(std::string((char*)slice1.span().data(), slice1.size()), "hello, World!");


	ByteSlice slice3 = std::move(slice1); // move
	EXPECT_EQ(slice3.size(), str.size());
	EXPECT_EQ(std::string((char*)slice3.span().data(), slice3.size()), "hello, World!");
}

TEST(SliceTest, TestSubSlice) {
	std::string str = "Hello, World!";
	ByteSlice slice = ByteSlice(str);

	ByteSlice subSlice = slice.subSlice(7, 5);
	EXPECT_EQ(subSlice.size(), 5);
	EXPECT_EQ(std::string((char*)subSlice.span().data(), subSlice.size()), "World");

	for (size_t i = 0; i < subSlice.size(); ++i) {
		EXPECT_EQ(subSlice[i], (std::byte)str[7 + i]);
	}

	ByteSlice subSliceCopy = subSlice;
	EXPECT_EQ(subSlice.size(), 5);
	EXPECT_EQ(std::string((char*)subSlice.span().data(), subSlice.size()), "World");

	ByteSlice subSliceMove = std::move(subSlice);
	EXPECT_EQ(subSliceMove.size(), 5);
	EXPECT_EQ(std::string((char*)subSliceMove.span().data(), subSliceMove.size()), "World");

	ByteSlice subsubSlice = subSliceMove.subSlice(1, 3);
	EXPECT_EQ(subsubSlice.size(), 3);
	EXPECT_EQ(std::string((char*)subsubSlice.span().data(), subsubSlice.size()), "orl");

	auto subSpan = slice.subSpan(7, 5);
	EXPECT_EQ(subSpan.size(), 5);
	EXPECT_EQ(std::string((char*)subSpan.data(), subSpan.size()), "World");

	auto subSpan2 = slice.subSpan(7);
	EXPECT_EQ(subSpan2.size(), str.size() - 7);
	EXPECT_EQ(std::string((char*)subSpan2.data(), subSpan2.size()), "World!");

	const auto constSlice = ByteSlice(str);
	auto constSubSpan = constSlice.subSpan(7, 5);
	EXPECT_EQ(constSubSpan.size(), 5);
	EXPECT_EQ(std::string((char*)constSubSpan.data(), constSubSpan.size()), "World");
	auto constSubSpan2 = constSlice.subSpan(7);
	EXPECT_EQ(constSubSpan2.size(), str.size() - 7);
	EXPECT_EQ(std::string((char*)constSubSpan2.data(), constSubSpan2.size()), "World!");
}

TEST(SliceTest, TestSliceConcat) {
	std::string str1 = "Hello, ";
	std::string str2 = "World!";
	ByteSlice slice1 = ByteSlice(str1);
	ByteSlice slice2 = ByteSlice(str2);
	ByteSlice combined = slice1 + slice2;
	EXPECT_EQ(combined.size(), str1.size() + str2.size());
	EXPECT_EQ(std::string((char*)combined.span().data(), combined.size()), "Hello, World!");
}