#include "common.h"
#include "proxy.h"


namespace nomp {
	namespace interface {
		PRO_DEF_MEM_DISPATCH(MemCompress, compress);
		PRO_DEF_MEM_DISPATCH(MemCompressInplace, compressInplace);
		struct Compresser : pro::facade_builder
			::support_copy<pro::constraint_level::nontrivial>
			::support_relocation<pro::constraint_level::nontrivial>
			::add_convention<MemCompress, ByteSlice(const ByteSlice& src)>
			::add_convention<MemCompressInplace, size_t(const ByteSlice& src, std::span<std::byte> dest)>
			::build {
		};
		using ICompresser = pro::proxy<Compresser>;

		PRO_DEF_MEM_DISPATCH(MemDecompress, decompress);
		PRO_DEF_MEM_DISPATCH(MemDecompressInplace, decompressInplace);
		struct Decompresser : pro::facade_builder
			::support_copy<pro::constraint_level::nontrivial>
			::support_relocation<pro::constraint_level::nontrivial>
			::add_convention<MemDecompress, ByteSlice(const ByteSlice& src, size_t originalSize)>
			::add_convention<MemDecompressInplace, size_t(const ByteSlice& src, std::span<std::byte> dest)>
			::build {
		};
	}

	class LZ4Compresser {
	public:
		ByteSlice compress(const ByteSlice& src);
		size_t compressInplace(const ByteSlice& src, std::span<std::byte> dest);
	};
	class LZ4Decompresser {
	public:
		ByteSlice decompress(const ByteSlice& src, size_t originalSize);
		size_t decompressInplace(const ByteSlice& src, std::span<std::byte> dest);
	};

}