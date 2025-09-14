#pragma once
#include "proxy.h"
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <memory>

namespace nomp {
	inline static constexpr std::string_view NOMP_VERSION = "v0.0.1";

	namespace interface {
		PRO_DEF_MEM_DISPATCH(MemRead, read);
		PRO_DEF_MEM_DISPATCH(MemWrite, write);
		PRO_DEF_MEM_DISPATCH(MemClose, close);
		PRO_DEF_MEM_DISPATCH(MemSeek, seek);

		struct Copyable : pro::facade_builder
			::support_copy<pro::constraint_level::nontrivial>
			::build {
		};
		using ICopyable = pro::proxy<Copyable>;

		struct Reader : pro::facade_builder
			::add_facade<Copyable>
			::add_convention<MemRead, int(std::span<std::byte> buf)>
			::build {
		};
		using IReader = pro::proxy<Reader>;

		struct Writer : pro::facade_builder
			::add_facade<Copyable>
			::add_convention<MemWrite, int(std::span<const std::byte> buf)>
			::build {
		};
		using IWriter = pro::proxy<Writer>;

		struct Closer : pro::facade_builder
			::add_convention<MemClose, void()>
			::build {
		};
		using ICloser = pro::proxy<Closer>;

		struct Seeker : pro::facade_builder
			::add_facade<Copyable>
			::add_convention<MemSeek, int64_t(int64_t offset, int whence)>
			::build {
		};
		using ISeeker = pro::proxy<Seeker>;

		struct ReadCloser : pro::facade_builder
			::add_facade<Reader>
			::add_facade<Closer>
			::build {
		};
		using IReadCloser = pro::proxy<ReadCloser>;

		struct ReadSeeker : pro::facade_builder
			::add_facade<Reader>
			::add_facade<Seeker>
			::build {
		};
		using IReadSeeker = pro::proxy<ReadSeeker>;

		struct ReadSeekCloser : pro::facade_builder
			::add_facade<Reader>
			::add_facade<Seeker>
			::add_facade<Closer>
			::build {
		};
		using IReadSeekCloser = pro::proxy<ReadSeekCloser>;

		struct WriteCloser : pro::facade_builder
			::add_facade<Writer>
			::add_facade<Closer>
			::build {
		};
		using IWriteCloser = pro::proxy<WriteCloser>;

		struct WriteSeeker : pro::facade_builder
			::add_facade<Writer>
			::add_facade<Seeker>
			::build {
		};
		using IWriteSeeker = pro::proxy<WriteSeeker>;

		struct WriteSeekCloser : pro::facade_builder
			::add_facade<Writer>
			::add_facade<Seeker>
			::add_facade<Closer>
			::build {
		};
		using IWriteSeekCloser = pro::proxy<WriteSeekCloser>;

		struct ReadWriter : pro::facade_builder
			::add_facade<Reader>
			::add_facade<Writer>
			::build {
		};
		using IReadWriter = pro::proxy<ReadWriter>;


		struct ReadWriteCloser : pro::facade_builder
			::add_facade<Reader>
			::add_facade<Writer>
			::add_facade<Closer>
			::build {
		};
		using IReadWriteCloser = pro::proxy<ReadWriteCloser>;
	}


	class ByteSlice {
		std::shared_ptr<std::byte[]> data;
		size_t sz;
	public:
		ByteSlice() :data(nullptr), sz(0) {}
		ByteSlice(std::shared_ptr<std::byte[]> ptr, size_t sz) : data(ptr), sz(sz) {}
		ByteSlice(std::span<std::byte> src) :data(std::make_shared<std::byte[]>(src.size())), sz(src.size()) {
			std::copy(src.begin(), src.end(), data.get());
		}
		ByteSlice(std::string src) :data(std::make_shared<std::byte[]>(src.size())), sz(src.size()) {
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
			return (*this <=> rhs) == std::strong_ordering::equal;
		}
		std::strong_ordering operator<=>(const ByteSlice& rhs) const noexcept {
			size_t minSize = sz < rhs.sz ? sz : rhs.sz;
			for (size_t i = 0; i < minSize; ++i) {
				if (data[i] < rhs.data[i]) return std::strong_ordering::less;
				if (data[i] > rhs.data[i]) return std::strong_ordering::greater;
			}
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
			if (i >= sz) throw std::out_of_range("ByteSlice index out of range");
			return data[i];
		}
		std::span<const std::byte> span() const {
			return { (const std::byte*)data.get(), sz };
		}
		size_t size() const { return sz; }

		bool empty() const {
			return sz == 0;
		}

	};
}