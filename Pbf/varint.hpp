#include <cstdint>
#include <exception>

namespace pbf {
	constexpr const int8_t max_varint_length=sizeof(uint64_t)*8/7 + 1;

	inline void skip_varint(const char** data, const char* end) {
		const int8_t* begin=reinterpret_cast<const int8_t*>(*data);
		const int8_t* iend=reinterpret_cast<const int8_t*>(end);
		const int8_t* p=begin;

		while(p != iend && *p < 0) {
			++p;
		}
		if(p >= begin + max_varint_length) {
			throw std::exception();
		}
		if(p == iend) {
			throw std::exception();
		}
		++p;
		*data=reinterpret_cast<const char*>(p);
	}
	template <typename T>
	inline int write_varint(T data, uint64_t value) {
		int n=1;
		while(value >= 0x80) {
			*data++=char((value & 0x7f) | 0x80);
			value>>=7;
			++n;
		}
		*data++=char(value);
		return n;
	}
	inline constexpr uint32_t encode_zigzag32(int32_t value) noexcept {
		return (static_cast<uint32_t>(value) << 1) ^ (static_cast<uint32_t>(value >> 31));
	}
	inline constexpr uint64_t encode_zigzag64(int64_t value) noexcept {
		return (static_cast<uint64_t>(value) << 1) ^ (static_cast<uint64_t>(value >> 63));
	}

}