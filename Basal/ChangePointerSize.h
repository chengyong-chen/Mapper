unsigned char* ChangePointerSize(unsigned char* pointer, unsigned int orglen, unsigned int newlen)
{
	if(orglen == newlen)
		return pointer;
	else if(newlen == 0)
	{
		if(pointer != nullptr)
			free(pointer);

		return nullptr;
	}
	else if(orglen == 0)
		return (unsigned char*)malloc(newlen);
	else if(orglen > newlen)
	{
		unsigned char* buf = (unsigned char*)malloc(newlen);
		memcpy(buf,pointer,newlen);
		free(pointer);
		return buf;
	}
	else if(orglen < newlen)
	{
		unsigned char* buf = (unsigned char*)malloc(newlen);
		memset(buf,0,newlen);
		memcpy(buf,pointer,orglen);
		free(pointer);
		return buf;
	}
	else
		return nullptr;
}