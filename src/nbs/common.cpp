#include "common.h"
#include <format>

namespace noms {
	std::string toString(proxy<Stringable> p) noexcept {
		return p->toString();
	}
}