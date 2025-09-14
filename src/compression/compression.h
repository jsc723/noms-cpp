#include "common.h"
#include "proxy.h"



namespace nomp {
	namespace interface {
		PRO_DEF_MEM_DISPATCH(MemEncode, encode);
		PRO_DEF_MEM_DISPATCH(MemDecode, decode);

		struct Compressor : pro::facade_builder
			::add_convention<MemEncode, ByteSlice(ByteSlice)>
			::add_convention<MemDecode, ByteSlice(ByteSlice)>
			::build {
		};
		using ICompressor = pro::proxy<Compressor>;
	}

	struct LZ4Compressor {
		static ByteSlice encode(ByteSlice data);
		static ByteSlice decode(ByteSlice data);
	};
}