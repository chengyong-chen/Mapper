#pragma once

unsigned char* memclone(const unsigned char* pointer, unsigned int len)
{
	if(len == 0)
		return nullptr;
	else if(pointer == nullptr)
		return nullptr;
	else
	{
		unsigned char* buf = (unsigned char*)malloc(len);
		memcpy(buf,pointer,len);
		return buf;
	}
}