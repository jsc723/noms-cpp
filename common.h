#pragma once
#include <string>
#include <span>
#include "proxy.h"
#include "channel.h"

using pro::proxy;

namespace noms {

PRO_DEF_MEM_DISPATCH(MemToString, toString);

struct Stringable : pro::facade_builder
	::support_copy<pro::constraint_level::nontrivial>
	::add_convention<MemToString, std::string()const>
	::build {};
std::string toString(proxy<Stringable> p) noexcept;

// size of data must be divisible by 5
std::string encode32(std::span<char> data);
void decode32(std::span<const char> encoded, std::span<char> data);

constexpr auto ByteLen = 20;
constexpr auto StringLen = 32; // 5 bytes = 8 chars, 20 bytes = 32 chars

struct Hash {
	char data[ByteLen];
};

}




