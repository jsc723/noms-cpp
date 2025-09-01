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
}