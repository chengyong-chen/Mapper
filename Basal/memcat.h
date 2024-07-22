#pragma once

unsigned char* memcat(unsigned char* pointer1, unsigned int len1, unsigned char* pointer2, unsigned int len2)
{
	if(len1 == 0 && len2 == 0)
		return nullptr;
	else
	{
		unsigned char* buf = (unsigned char*)malloc(len1+len2);
		memcpy(buf,pointer1,len1);
		memcpy(buf+len1,pointer2,len2);
		return buf;
	}
}

unsigned char* memext(unsigned char* pointer1, unsigned int len1, unsigned char* pointer2, unsigned int len2, int align)
{
	if(len1==0&&len2==0)
		return nullptr;
	else
	{
		unsigned char* buf = (unsigned char*)malloc(len1+__max(len2, align));
		memcpy(buf, pointer1, len1);
		memcpy(buf+len1, pointer2, len2);
		memset(buf+len1+len2, 0, align-len2);
		free(pointer1);
		return buf;
	}
}