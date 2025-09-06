#pragma once

#include "common.h"
#include "hash/all.h"
#include <cstddef> 
#include <span>

namespace nomp {
	class ByteSlice {
		std::shared_ptr<std::byte[]> data;
		size_t sz;
	public:
		ByteSlice() :data(nullptr), sz(0) {}
		ByteSlice(std::shared_ptr<std::byte[]> ptr, size_t sz): data(ptr), sz(sz) {}
		ByteSlice(std::span<std::byte> src):data(std::make_shared<std::byte[]>(src.size())), sz(src.size()) {
			std::copy(src.begin(), src.end(), data.get());
		}
		ByteSlice(std::string src):data(std::make_shared<std::byte[]>(src.size())), sz(src.size()) {
			std::copy((std::byte*)src.data(), (std::byte*)src.data() + src.size(), data.get());
		}
		ByteSlice(const ByteSlice& other) : data(other.data), sz(other.sz) {}
		ByteSlice& operator=(const ByteSlice& other) {
			if (this != &other) {
				data = other.data;
				sz = other.sz;
			}
			return *this;
		}
		ByteSlice(ByteSlice&& other) noexcept : data(std::move(other.data)), sz(other.sz) {}
		ByteSlice& operator=(ByteSlice&& other) noexcept {
			if (this != &other) {
				data = std::move(other.data);
				sz = other.sz;
			}
			return *this;
		}
		bool operator==(const ByteSlice& rhs) const noexcept {
			return (*this <=> rhs) == 0;
		}
		int operator<=>(const ByteSlice& rhs) const noexcept {
			size_t minSize = sz < rhs.sz ? sz : rhs.sz;
			for (size_t i = 0; i < minSize; ++i) {
				if (data[i] < rhs.data[i]) return -1;
				if (data[i] > rhs.data[i]) return 1;
			}
			if (sz < rhs.sz) return -1;
			if (sz > rhs.sz) return 1;
			return 0;
		}
		ByteSlice operator+(const ByteSlice& other) const {
			auto newData = std::make_shared<std::byte[]>(sz + other.sz);
			std::copy(data.get(), data.get() + sz, newData.get());
			std::copy(other.data.get(), other.data.get() + other.sz, newData.get() + sz);
			return ByteSlice(std::span{ newData.get(), sz + other.sz });
		}
		std::byte operator[](size_t i) const {
			if (i >= sz) throw std::out_of_range("ByteSlice index out of range");
			return data[i];
		}
		std::span<const std::byte> span() {
			return { (const std::byte*)data.get(), sz };
		}
		Hash hash() {
			return Hash::Of(span());
		}

		size_t size() const { return sz; }

		bool empty() const {
			return sz == 0;
		}

	};
}