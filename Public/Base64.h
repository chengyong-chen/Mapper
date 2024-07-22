#pragma once

#include <string>
static const char base64encode[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

bool isbase64(const char& c)
{
	return strchr(base64encode, c)!=nullptr;
}

inline char value(const char& c)
{
	const char* p = strchr(base64encode, c);
	return p ? p-base64encode : 0;
}
char* Base64_Encode(const char* str, unsigned int in_len) {
	int i = 0;
	int j = 0;
	char char_array_3[3];
	char char_array_4[4];
	std::string ret;
	while(in_len--) {
		char_array_3[i++] = *(str++);
		if(i==3) {
			char_array_4[0] = (char_array_3[0]&0xfc)>>2;
			char_array_4[1] = ((char_array_3[0]&0x03)<<4)+((char_array_3[1]&0xf0)>>4);
			char_array_4[2] = ((char_array_3[1]&0x0f)<<2)+((char_array_3[2]&0xc0)>>6);
			char_array_4[3] = char_array_3[2]&0x3f;

			for(i = 0; (i<4); i++)
				ret += base64encode[char_array_4[i]];
			i = 0;
		}
	}
	if(i)
	{
		for(j = i; j<3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0]&0xfc)>>2;
		char_array_4[1] = ((char_array_3[0]&0x03)<<4)+((char_array_3[1]&0xf0)>>4);
		char_array_4[2] = ((char_array_3[1]&0x0f)<<2)+((char_array_3[2]&0xc0)>>6);

		for(j = 0; (j<i+1); j++)
			ret += base64encode[char_array_4[j]];

		while((i++<3))
			ret += '=';
	}

	char* cstr = (char*)::malloc(ret.size()+1);
	strcpy(cstr, ret.c_str()); // or pass &s[0]
	return cstr;
}
int UnBase64(const unsigned char* src, unsigned char* dest, int srclen)
{
	if(srclen<4)
		return 0;
	if(src==nullptr)
		return 0;

	unsigned char* p = dest;
	while(*src!=0)
	{
		bool error = false;
		unsigned char seg[4];
		for(int index = 0; index<4; index++)
		{
			while(*src!=0&&isspace(*src))
				src++;
			while(*src!=0&&*src=='\r')
				src++;
			while(*src!=0&&*src=='\n')
				src++;
			while(*src!=0&&*src==' ')
				src++;

			if(*src==0)
				continue;
			else if(*src=='\0')
				seg[index] = 0;
			else if(*src=='=')
				seg[index] = 0;
			else if(isbase64(*src))
				seg[index] = *src;
			else
			{
				error = true;
				break;
			}
			src++;
		}
		if(error==false)
		{
			const char a = value(seg[0]);
			const char b = value(seg[1]);
			const char c = value(seg[2]);
			const char d = value(seg[3]);
			*p++ = (a<<2)|(b>>4);
			*p++ = (b<<4)|(c>>2);
			*p++ = (c<<6)|(d);
		}
	};

	*p = 0;
	return p-dest;
}
