#include "nbs/common.h"
#include "nbs/MemTable.h"
#include "nbs/table.h"

#include <iostream>
#include <string>

using namespace noms;

int main() {
	proxy<Stringable> s = pro::make_proxy<Stringable>(noms::addr{});
	std::cout << toString(s);
}