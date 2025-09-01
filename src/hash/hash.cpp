#include "hash.h"
#include <format>
#include <openssl/sha.h>
#include <regex>


namespace nomp {

	// binary to base32 encoding
	// size of data must be divisible by 5
	std::string encode32(std::span<const char> data) {
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

	size_t encodedCharToValue(char c) {
		if (c >= '0' && c <= '9') return c - '0';
		size_t res = c + 10 - 'a';
		if (res < 32) return res;
		throw std::invalid_argument("Invalid character in encoded string: " + std::string(1, c));
	}

	// base32 decoding to binary
	void decode32(std::span<const char> encoded, std::span<char> data) {
		if (encoded.size() * 5 % 8 != 0 || data.size() * 8 != encoded.size() * 5) {
			throw std::invalid_argument(std::format("Invalid encoded size or data size encode sz = {}, decode sz = {}", encoded.size(), data.size()));
		}
		for (size_t i = 0, j = 0; i < encoded.size(); i += 8, j += 5) {
			memset(&data[j], 0, 5);
			data[j] |= (encodedCharToValue(encoded[i]) << 3);
			int v1 = encodedCharToValue(encoded[i + 1]);
			data[j] |= (v1 >> 2);
			data[j + 1] |= (v1 << 6);
			data[j + 1] |= (encodedCharToValue(encoded[i + 2]) << 1);
			int v3 = encodedCharToValue(encoded[i + 3]);
			data[j + 1] |= (v3 >> 4);
			data[j + 2] |= (v3 << 4);
			int v4 = encodedCharToValue(encoded[i + 4]);
			data[j + 2] |= (v4 >> 1);
			data[j + 3] |= (v4 << 7);
			data[j + 3] |= (encodedCharToValue(encoded[i + 5]) << 2);
			int v6 = encodedCharToValue(encoded[i + 6]);
			data[j + 3] |= (v6 >> 3);
			data[j + 4] |= (v6 << 5);
			data[j + 4] |= encodedCharToValue(encoded[i + 7]);
		}
	}
	Hash Hash::Of(std::span<const char> data)
	{
		SHA512_CTX ctx;
		SHA512_Init(&ctx);
		SHA512_Update(&ctx, data.data(), data.size());
		unsigned char buf[SHA512_DIGEST_LENGTH];
		SHA512_Final(buf, &ctx);
		return Hash(std::span{ (char*)buf, ByteLen });
	}
	// parse from base32 string
	std::optional<Hash> Hash::MaybeParse(std::span<const char> hash)
	{
		// Create the regex object
		static const std::regex hashPattern("^([0-9a-v]{" + std::to_string(StringLen) + "})$");
		if (std::regex_match(hash.data(), hash.data() + hash.size(), hashPattern)) {
			Hash h;
			decode32(hash, h);
			return h;
		}
		return std::optional<Hash>();
	}

}
