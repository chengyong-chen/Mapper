#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <limits>
#include <string>
#include <utility>
#include <cassert>

#include "types.hpp"
#include "varint.hpp"

namespace pbf {
	class writer {
		std::string* m_data;
		void add_varint(uint64_t value) {
			assert(m_data);
			write_varint(std::back_inserter(*m_data), value);
		}
		void add_varint(pbf::tag tag, uint64_t value) {
			add_tag(tag, pbf::type::varint);
			add_varint(value);
		}
		template <typename T>
		void add_fixed(T value) {
			assert(m_data);
			m_data->append(reinterpret_cast<const char*>(&value), sizeof(T));
		}
		template <typename T, typename It>
		void add_vector_fixed(pbf::tag tag, It first, It last, std::input_iterator_tag) {
			if (first == last)
				return;

			writer sw(*this, tag);

			while (first != last) {
				sw.add_fixed<T>(*first++);
			}
		}
		template <typename T, typename It>
		void add_vector_fixed(pbf::tag tag, It first, It last, std::forward_iterator_tag) {
			if (first == last)
				return;

			const auto length = std::distance(first, last);
			add_varint(length);

			while (first != last) {
				add_fixed<T>(*first++);
			}
		}
		template <typename It>
		void add_vector_varint(It first, It last) {
			if (first == last)
				return;

			writer sw(*this);

			while (first != last) {
				sw.add_varint(uint64_t(*first++));
			}
		}
		template <typename It>
		void add_vector_varint(pbf::tag tag, It first, It last) {
			if (first == last)
				return;

			add_tag(tag, pbf::type::ignore);

			while (first != last) {
				add_varint(uint64_t(*first++));
			}
		}
		template <typename It>
		void add_vector_svarint(It first, It last) {
			if (first == last)
				return;

			while (first != last) {
				add_varint(encode_zigzag64(*first++));
			}
		}
		template <typename It>
		void add_vector_svarint(pbf::tag tag, It first, It last) {
			if (first == last)
				return;

			add_tag(tag, pbf::type::ignore);

			while (first != last) {
				add_varint(encode_zigzag64(*first++));
			}
		}

	public:
		explicit writer(std::string& data) noexcept :
			m_data(&data) {
		}
		writer() noexcept :
			m_data(nullptr) {
		}

		writer(const writer&) noexcept = default;
		writer& operator=(const writer&) noexcept = default;
		writer(writer&&) noexcept = default;
		writer& operator=(writer&&) noexcept = default;
		~writer() {
		}
		void add_tag(pbf::tag tag, pbf::type type) {
			assert(((tag > 0 && tag < 19000) || (tag > 19999 && tag <= ((1 << 29) - 1))) && "tag out of range");
			const unsigned char b = (tag << 3) | (uint8_t)type;
			add_byte(b);
		}
		void add_byte(unsigned char value) {
			m_data->append(1, value);
		}
		void add_byte(pbf::tag tag, unsigned char value) {
			add_tag(tag, pbf::type::ignore);
			m_data->append(1, value);
		}
		void add_bool(bool value) {
			m_data->append(1, value);
		}
		void add_bool(pbf::tag tag, bool value) {
			add_tag(tag, pbf::type::varint);
			assert(m_data);
			m_data->append(1, value);
		}
		void add_variant_enum(pbf::tag tag, int32_t value) {
			add_varint(tag, uint64_t(value));
		}
		void add_variant_int16(short value) {
			add_varint(uint64_t(value));
		}
		void add_variant_int16(pbf::tag tag, short value) {
			add_varint(tag, uint64_t(value));
		}
		void add_variant_uint16(unsigned short value) {
			add_varint(uint64_t(value));
		}
		void add_variant_uint16(pbf::tag tag, unsigned short value) {
			add_varint(tag, uint64_t(value));
		}
		void add_variant_sint32(int32_t value) {
			add_varint(encode_zigzag32(value));
		}
		void add_variant_sint32(pbf::tag tag, int32_t value) {
			add_varint(tag, encode_zigzag32(value));
		}
		void add_variant_uint32(int32_t value) {
			add_varint(uint64_t(value));
		}
		void add_variant_uint32(pbf::tag tag, uint32_t value) {
			add_varint(tag, value);
		}
		void add_variant_int64(pbf::tag tag, int64_t value) {
			add_varint(tag, uint64_t(value));
		}
		void add_variant_uint64(pbf::tag tag, uint64_t value) {
			add_varint(tag, value);
		}
		void add_fixed_uint16(uint16_t value) {
			add_fixed<uint16_t>(value);
		}
		void add_fixed_uint32(uint32_t value) {
			add_fixed<uint32_t>(value);
		}
		void add_fixed_uint32(pbf::tag tag, uint32_t value) {
			add_tag(tag, pbf::type::fixed32);
			add_fixed<uint32_t>(value);
		}
		void add_tag(pbf::tag tag, uint64_t value) {
			add_tag(tag, pbf::type::fixed64);
			add_fixed<uint64_t>(value);
		}
		void add_fixed_int64(pbf::tag tag, int64_t value) {
			add_tag(tag, pbf::type::fixed64);
			add_fixed<int64_t>(value);
		}
		void add_float(float value) {
			add_fixed<float>(value);
		}
		void add_float(pbf::tag tag, float value) {
			add_tag(tag, pbf::type::fixed32);
			add_fixed<float>(value);
		}
		void add_double(double value) {
			add_fixed<double>(value);
		}
		void add_double(pbf::tag tag, double value) {
			add_tag(tag, pbf::type::fixed64);
			add_fixed<double>(value);
		}
		//void add_bytes(const char* value) {			
		//	std::uint32_t size=std::strlen(value);
		//	add_varint(size);
		//	m_data->append(value, size);
		//}
		void add_bytes(const char* value, std::uint32_t size) {			
			assert(m_data);
			assert(size <= UINTMAX_MAX);
			add_varint(size);
			m_data->append(value, size);
		}

		void add_block(pbf::tag tag, const char* value, std::size_t size) {
			add_tag(tag, pbf::type::bytes);
			add_bytes(value, size);
		}
		void add_string(const std::string& value) {
			std::size_t size = value.size();
			add_varint(size);
			m_data->append(value.data(), size);
		}
		void add_string(const char* value) {
			std::size_t size = strlen(value);
			add_varint(size);
			m_data->append(value, size);
		}
		void add_string(const char* value, std::size_t size) {
			add_varint(size);
			m_data->append(value, size);
		}
		//void add_string(pbf::tag tag, const char* value, std::size_t size) {
		//	add_bytes(tag, value, size);
		//}
		//void add_string(pbf::tag tag, const std::string& value) {
		//	add_bytes(tag, value.data(), value.size());
		//}
		//void add_string(pbf::tag tag, const char* value) {
		//	add_bytes(tag, value, std::strlen(value));
		//}
		void add_bytes(const char* value) {
			std::uint32_t size = std::strlen(value);
			add_varint(size);
			m_data->append(value, size);
		}
		void add_bytes(const std::string& value) {
			std::size_t size = value.size();
			add_varint(size);
			m_data->append(value.data(), size);
		}
		template <typename InputIterator>
		void add_vector_bool(pbf::tag tag, InputIterator first, InputIterator last) {
			add_vector_varint(tag, first, last);
		}
		template <typename InputIterator>
		void add_vector_enum(pbf::tag tag, InputIterator first, InputIterator last) {
			add_vector_varint(tag, first, last);
		}
		template <typename InputIterator>
		void add_vector_int32(pbf::tag tag, InputIterator first, InputIterator last) {
			add_vector_varint(tag, first, last);
		}
		template <typename InputIterator>
		void add_vector_sint32(InputIterator first, InputIterator last) {
			add_vector_svarint(first, last);
		}
		template <typename InputIterator>
		void add_vector_sint32(pbf::tag tag, InputIterator first, InputIterator last) {
			add_vector_svarint(tag, first, last);
		}
		template <typename InputIterator>
		void add_vector_uint32(InputIterator first, InputIterator last) {
			add_vector_varint(first, last);
		}
		template <typename InputIterator>
		void add_vector_uint32(pbf::tag tag, InputIterator first, InputIterator last) {
			add_vector_varint(tag, first, last);
		}
		template <typename InputIterator>
		void add_vector_int64(pbf::tag tag, InputIterator first, InputIterator last) {
			add_vector_varint(tag, first, last);
		}
		template <typename InputIterator>
		void add_vector_sint64(pbf::tag tag, InputIterator first, InputIterator last) {
			add_vector_svarint(tag, first, last);
		}
		template <typename InputIterator>
		void add_vector_uint64(pbf::tag tag, InputIterator first, InputIterator last) {
			add_vector_varint(tag, first, last);
		}
		template <typename InputIterator>
		void add_vector_fixed32(pbf::tag tag, InputIterator first, InputIterator last) {
			add_vector_fixed<uint32_t, InputIterator>(tag, first, last, typename std::iterator_traits<InputIterator>::iterator_category());
		}
		template <typename InputIterator>
		void add_vector_sfixed32(pbf::tag tag, InputIterator first, InputIterator last) {
			add_vector_fixed<int32_t, InputIterator>(tag, first, last, typename std::iterator_traits<InputIterator>::iterator_category());
		}
		template <typename InputIterator>
		void add_vector_fixed64(pbf::tag tag, InputIterator first, InputIterator last) {
			add_vector_fixed<uint64_t, InputIterator>(tag, first, last, typename std::iterator_traits<InputIterator>::iterator_category());
		}
		template <typename InputIterator>
		void add_vector_sfixed64(pbf::tag tag, InputIterator first, InputIterator last) {
			add_vector_fixed<int64_t, InputIterator>(tag, first, last, typename std::iterator_traits<InputIterator>::iterator_category());
		}
		template <typename InputIterator>
		void add_vector_float(pbf::tag tag, InputIterator first, InputIterator last) {
			add_vector_fixed<float, InputIterator>(tag, first, last, typename std::iterator_traits<InputIterator>::iterator_category());
		}
		template <typename InputIterator>
		void add_vector_double(pbf::tag tag, InputIterator first, InputIterator last) {
			add_vector_fixed<double, InputIterator>(tag, first, last, typename std::iterator_traits<InputIterator>::iterator_category());
		}
	};
}