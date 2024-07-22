#pragma once
#include <cassert>
#include <boost/compute/detail/sha1.hpp>

namespace cstr
{
	static void sha1(CString string, boost::uuids::detail::sha1& sha1)
	{
		const wchar_t* buffer = string.GetBuffer();
		const unsigned int len = (string.GetLength() + 1) * sizeof(wchar_t); // Include null terminator
		const unsigned char* bytes = reinterpret_cast<const unsigned char*>(buffer);
		for(size_t index = 0; index < len; ++index)
		{
			sha1.process_byte(bytes[index]);
		}
		string.ReleaseBuffer();
	}
	static void sha1(CStringA string, boost::uuids::detail::sha1& sha1)
	{
		const char* buffer = string.GetBuffer();
		for(size_t index = 0; index <::strlen(buffer); ++index)
		{
			sha1.process_byte(buffer[index]);
		}
		string.ReleaseBuffer();
	}
}