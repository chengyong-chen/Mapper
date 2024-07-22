#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <utility>

namespace pbf {
	using tag = uint8_t;
	enum class type : uint8_t {
		varint = 0, // int32/64, uint32/64, sint32/64, bool, enum
		fixed64 = 1, // fixed64, sfixed64, double
		bytes = 2, // string, bytes, embedded messages,
		// packed repeated fields
		fixed32 = 5, // fixed32, sfixed32, float
		ignore = 6,
		unknown = 99 // used for default setting in this library
	};
}