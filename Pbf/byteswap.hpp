#pragma once


#include <cassert>
#include <cstdint>

namespace pbf {
	namespace detail {
		inline uint32_t byteswap(uint32_t value) noexcept {
			return ((value & 0xff000000) >> 24) |
				((value & 0x00ff0000) >> 8) |
				((value & 0x0000ff00) << 8) |
				((value & 0x000000ff) << 24);
		}
		inline uint64_t byteswap(uint64_t value) noexcept {
			return ((value & 0xff00000000000000ULL) >> 56) |
				((value & 0x00ff000000000000ULL) >> 40) |
				((value & 0x0000ff0000000000ULL) >> 24) |
				((value & 0x000000ff00000000ULL) >> 8) |
				((value & 0x00000000ff000000ULL) << 8) |
				((value & 0x0000000000ff0000ULL) << 24) |
				((value & 0x000000000000ff00ULL) << 40) |
				((value & 0x00000000000000ffULL) << 56);
		}
		inline void byteswap(uint32_t* ptr) noexcept {
			*ptr=byteswap(*ptr);
		}
		inline void byteswap(uint64_t* ptr) noexcept {
			*ptr=byteswap(*ptr);
		}
		inline void byteswap(int32_t* ptr) noexcept {
			auto bptr=reinterpret_cast<uint32_t*>(ptr);
			*bptr=byteswap(*bptr);
		}
		inline void byteswap(int64_t* ptr) noexcept {
			auto bptr=reinterpret_cast<uint64_t*>(ptr);
			*bptr=byteswap(*bptr);
		}
		inline void byteswap(float* ptr) noexcept {
			auto bptr=reinterpret_cast<uint32_t*>(ptr);
			*bptr=byteswap(*bptr);
		}
		inline void byteswap(double* ptr) noexcept {
			auto bptr=reinterpret_cast<uint64_t*>(ptr);
			*bptr=byteswap(*bptr);
		}
	}
}
