#pragma once 
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <memory>

namespace nomp {
	class ByteSlice {
		std::shared_ptr<std::byte[]> data;
		size_t offset;
		size_t sz;

	public:
		ByteSlice() :data(nullptr), offset(0), sz(0) {}
		ByteSlice(size_t sz) : data(std::make_shared<std::byte[]>(sz)), offset(0), sz(sz) {}
		ByteSlice(std::shared_ptr<std::byte[]> ptr, size_t sz) : data(ptr), offset(0), sz(sz) {}
		ByteSlice(std::shared_ptr<std::byte[]> ptr, size_t offset, size_t sz) : data(ptr), offset(offset), sz(sz) {}
		ByteSlice(std::span<std::byte> src) :data(std::make_shared<std::byte[]>(src.size())), offset(0), sz(src.size()) {
			std::copy(src.begin(), src.end(), data.get());
		}
		ByteSlice(std::span<const std::byte> src) :data(std::make_shared<std::byte[]>(src.size())), offset(0), sz(src.size()) {
			std::copy(src.begin(), src.end(), data.get());
		}
		ByteSlice(std::string src) :data(std::make_shared<std::byte[]>(src.size())), offset(0), sz(src.size()) {
			std::copy((std::byte*)src.data(), (std::byte*)src.data() + src.size(), data.get());
		}
		ByteSlice(const ByteSlice& other) : data(other.data), offset(other.offset), sz(other.sz) {}
		ByteSlice& operator=(const ByteSlice& other) {
			if (this != &other) {
				data = other.data;
				offset = other.offset;
				sz = other.sz;
			}
			return *this;
		}
		ByteSlice(ByteSlice&& other) noexcept : data(std::move(other.data)), offset(other.offset), sz(other.sz) {}
		ByteSlice& operator=(ByteSlice&& other) noexcept {
			if (this != &other) {
				data = std::move(other.data);
				offset = other.offset;
				sz = other.sz;
			}
			return *this;
		}
		bool operator==(const ByteSlice& rhs) const noexcept {
			return (*this <=> rhs) == std::strong_ordering::equal;
		}
		std::strong_ordering operator<=>(const ByteSlice& rhs) const noexcept {
			size_t minSize = sz < rhs.sz ? sz : rhs.sz;
			for (size_t i = 0; i < minSize; ++i) {
				if (data[i] < rhs.data[i]) return std::strong_ordering::less;
				if (data[i] > rhs.data[i]) return std::strong_ordering::greater;
			}
			if (offset < rhs.offset) return std::strong_ordering::less;
			if (offset > rhs.offset) return std::strong_ordering::greater;
			if (sz < rhs.sz) return std::strong_ordering::less;
			if (sz > rhs.sz) return std::strong_ordering::greater;
			return std::strong_ordering::equal;
		}
		ByteSlice operator+(const ByteSlice& other) const {
			auto newData = std::make_shared<std::byte[]>(sz + other.sz);
			std::copy(data.get(), data.get() + sz, newData.get());
			std::copy(other.data.get(), other.data.get() + other.sz, newData.get() + sz);
			return ByteSlice(std::span{ newData.get(), sz + other.sz });
		}
		std::byte operator[](size_t i) const {
			return data[offset + i];
		}
		std::span<const std::byte> span() const {
			return { (const std::byte*)data.get() + offset, sz };
		}
		std::span<std::byte> span() {
			return { data.get() + offset, sz };
		}
		std::span<const std::byte> subSpan(size_t offset) const {
			return { data.get() + this->offset + offset, sz - offset };
		}
		std::span<const std::byte> subSpan(size_t offset, size_t length) const {
			return { data.get() + this->offset + offset, length };
		}
		std::span<std::byte> subSpan(size_t offset) {
			return { data.get() + this->offset + offset, sz - offset };
		}
		std::span<std::byte> subSpan(size_t offset, size_t length) {
			return { data.get() + this->offset + offset, length };
		}
		ByteSlice subSlice(size_t offset) const {
			return ByteSlice(data, this->offset + offset, sz - offset);
		}
		ByteSlice subSlice(size_t offset, size_t length) const {
			return ByteSlice(data, this->offset + offset, length);
		}
		ByteSlice copy() const {
			return ByteSlice(span());
		}
		size_t size() const { return sz; }

		bool empty() const {
			return sz == 0;
		}

		
		struct Editor {
			ByteSlice& slice;
			std::byte& operator[](size_t i) {
				return slice.data[slice.offset + i];
			}
		};
		friend class ByteSlice::Editor;
		Editor edit() {
			return Editor{ *this };
		}
	};
}