#include <string>
#include <string_view>
#include <span>
#include <memory>
#include <optional>
#include <unordered_set>
#include <stdexcept>

namespace noms {

	// binary to base32 encoding
	// size of data must be divisible by 5
	std::string encode32(std::span<const char> data);

	// base32 decoding to binary
	void decode32(std::span<const char> encoded, std::span<char> data);

	// hash length
	constexpr auto ByteLen = 20;

	// base32 encoded length = ByteLen * 8 / 5
	constexpr auto StringLen = 32; // 5 bytes = 8 chars, 20 bytes = 32 chars

	struct Hash {
		std::unique_ptr<char[]> data;


		bool isEmpty() const {
			for (size_t i = 0; i < ByteLen; ++i) {
				if (data[i] != 0) return false;
			}
			return true;
		}

		operator std::span<char>() const {
			return std::span{ data.get(), ByteLen };
		}

		operator std::span<const char>() const {
			return std::span{ data.get(), ByteLen };
		}

        std::string toString() const {
			return encode32(*this);
		}
	

		Hash() : data(std::make_unique<char[]>(ByteLen)) {
		}
		Hash(const Hash& other) : data(std::make_unique<char[]>(ByteLen)) {
			std::copy(other.data.get(), other.data.get() + ByteLen, data.get());
		}
		Hash(Hash&& other) noexcept : data(std::move(other.data)) {
		}
		Hash& operator=(const Hash& other) {
			if (this != &other) {
				std::copy(other.data.get(), other.data.get() + ByteLen, data.get());
			}
			return *this;
		}
		Hash& operator=(Hash&& other) noexcept {
			if (this != &other) {
				data = std::move(other.data);
			}
			return *this;
		}

		// copy from d
		Hash(std::span<const char> d) {
			if (d.size() != ByteLen) {
				throw std::invalid_argument("Hash data must be 20 bytes, got " + std::to_string(d.size()));
			}
			data = std::make_unique<char[]>(ByteLen);
			std::copy(d.begin(), d.end(), data.get());
		}

		// hash of data
		static Hash Of(std::span<const char> data);

		// parse from base32 string
		static std::optional<Hash> MaybeParse(std::span<const char> encoded);
		static Hash Parse(const char* encoded) {
			return Parse(std::span{ encoded, strlen(encoded) });
		}
		static Hash Parse(std::span<const char> encoded) {
			auto h = MaybeParse(encoded);
			if (!h.has_value()) {
				throw std::invalid_argument("Invalid hash string: " + std::string(encoded.data(), encoded.size()));
			}
			return h.value();
		}

		bool operator<(const Hash& other) const {
			return std::lexicographical_compare(data.get(), data.get() + ByteLen, other.data.get(), other.data.get() + ByteLen);
		}
		bool less(const Hash& other) const {
			return *this < other;
		}
		bool operator==(const Hash& other) const {
			return std::memcmp(data.get(), other.data.get(), ByteLen) == 0;
		}


		// for unordered_set
		class Hasher {
		public:
			size_t operator()(const Hash& h) const noexcept {
				std::size_t hash_value = 0;
				auto buf = h.data.get();
				for (size_t i = 0; i < noms::ByteLen; ++i) {
					hash_value = hash_value * 31 + buf[i];
				}
				return hash_value;
			}
		};
	};

	class HashSet {
	private:
		std::unique_ptr<std::unordered_set<Hash>> set;
	public:
		HashSet();
		HashSet(const HashSet& other);
		HashSet(HashSet&& other) noexcept = default;
		HashSet(std::initializer_list<Hash> list);
		bool contains(const Hash& h) const;
		void insert(const Hash& h);
		size_t size() const;
		void clear();
	};

	



}