#pragma once
#include <cassert>

namespace str
{
	static char* replace(const char* s1, const char* s2, const char* s3)
	{
		const unsigned int s1_len = strlen(s1);
		const unsigned int s2_len = strlen(s2);

		if(s1==nullptr||s1_len==0)
			return nullptr;
		else if(s2==nullptr||s2_len==0)
			return _strdup(s1);

		/*
		* Two-pass approach: figure out how much space to allocate for
		* the new string, pre-allocate it, then perform replacement(s).
		*/

		unsigned int count = 0;
		const char* p = s1;
		do
		{
			p = strstr(p, s2);
			if(p)
			{
				p += s2_len;
				++count;
			}
		} while(p);

		if(count==0)
			return _strdup(s1);

		/*
		* The following size arithmetic is extremely cautious, to guard
		* against unsigned int overflows.
		*/
		assert(s1_len>=count*s2_len);
		assert(count);
		const unsigned int s1_without_s2_len = s1_len-count*s2_len;
		const unsigned int s3_len = strlen(s3);
		const unsigned int newstr_len = s1_without_s2_len+count*s3_len;
		if(s3_len&&((newstr_len<=s1_without_s2_len)||(newstr_len+1==0)))/* Overflow. */
			return nullptr;

		char* newstr = (char*)malloc(newstr_len+1); /* w/ terminator */
		if(!newstr)/* ENOMEM, but no good way to signal it. */
			return nullptr;

		char* dst = newstr;
		const char* start_substr = s1;
		unsigned int i;
		for(i = 0; i!=count; ++i)
		{
			const char* end_substr = strstr(start_substr, s2);
			assert(end_substr);
			const unsigned int substr_len = end_substr-start_substr;
			memcpy(dst, start_substr, substr_len);
			dst += substr_len;
			memcpy(dst, s3, s3_len);
			dst += s3_len;
			start_substr = end_substr+s2_len;
		}

		/* copy remainder of s1, including trailing '\0' */
		const unsigned int remains = s1_len-(start_substr-s1)+1;
		assert(dst+remains==newstr+newstr_len+1);
		memcpy(dst, start_substr, remains);
		assert(strlen(newstr)==newstr_len);
		return newstr;
	};
}