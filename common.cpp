#include "common.h"

namespace noms {
	std::string toString(proxy<Stringable> p) noexcept {
		return p->toString();
	}

	// size of data must be divisible by 5
	std::string encode32(std::span<char> data) {
		const static char alphabet[] = "0123456789abcdefghijklmnopqrstuv";
		const auto N = data.size();
		if (N % 5 != 0) {
			throw std::invalid_argument("Data size must be divisible by 5, got " + std::to_string(N));
		}
		std::string result = std::string(N * 8 / 5, '0');
		for (size_t i = 0, j = 0; i < result.size(); i += 8, j += 5) {
			result[i] = alphabet[(data[j] >> 3) & 0x1F];
			result[i + 1] = alphabet[((data[j] & 0x07) << 2) | ((data[j + 1] >> 6) & 0x03)];
			result[i + 2] = alphabet[(data[j + 1] >> 1) & 0x1F];
			result[i + 3] = alphabet[((data[j + 1] & 0x01) << 4) | ((data[j + 2] >> 4) & 0x0F)];
			result[i + 4] = alphabet[((data[j + 2] & 0x0F) << 1) | ((data[j + 3] >> 7) & 0x01)];
			result[i + 5] = alphabet[(data[j + 3] >> 2) & 0x1F];
			result[i + 6] = alphabet[((data[j + 3] & 0x03) << 3) | ((data[j + 4] >> 5) & 0x07)];
			result[i + 7] = alphabet[data[j + 4] & 0x1F];
		}
		return result;
	}

	void decode32(std::span<const char> encoded, std::span<char> data) {
		if (encoded.size() * 5 % 8 != 0 || data.size() * 8 != encoded.size() * 5) {
			throw std::invalid_argument("Invalid encoded size or data size");
		}
		const static char alphabet[] = "0123456789abcdefghijklmnopqrstuv";
		for (size_t i = 0, j = 0; i < encoded.size(); i += 8, j += 5) {
			for (size_t k = 0; k < 8; ++k) {
				auto pos = std::lower_bound(alphabet, alphabet + 32, encoded[i + k]);
				if (pos == alphabet + 32 || *pos != encoded[i + k]) {
					throw std::invalid_argument("Invalid character in encoded string: " + std::string(begin(data), end(data)));
				}
				data[j + k / 2] |= (pos - alphabet) << ((k % 2) ? 0 : 3);
			}
		}
	}
}