#include "common.h"
#include "MemTable.h"
#include "table.h"

#include <iostream>
#include <string>

using namespace noms;

int main() {
	proxy<Stringable> s = pro::make_proxy<Stringable>(noms::addr{});
	std::cout << toString(s);
}