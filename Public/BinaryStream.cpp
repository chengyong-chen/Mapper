#include "stdafx.h"
#include "BinaryStream.h"
#include <iostream>
#include <cstring>

BinaryStream::BinaryStream(const unsigned int size)
{
	bytes = size;
	buffer = new unsigned char[bytes];
	reader = 0;
	length = 0;
}
BinaryStream::BinaryStream(const unsigned char buf[], const unsigned int size)
	:BinaryStream(size)
{
	memcpy(buffer, buf, size*sizeof(unsigned char));
}

BinaryStream& BinaryStream::operator=(const BinaryStream& source)
{
	this->free();

	if(this!=&source)
	{
		bytes = source.bytes;
		reader = source.reader;
		length = source.length;

		buffer = new unsigned char[bytes];
		memcpy(buffer, source.buffer, bytes);

	}
	return *this;
}

BinaryStream::~BinaryStream()
{
	delete[] buffer;

	bytes = 0;
	reader = 0;
	length = 0;
}
void BinaryStream::free()
{
	delete[] buffer;

	bytes = 0;
	reader = 0;
	length = 0;
}
void BinaryStream::append(const unsigned char byte)
{
	if(length<(bytes-reader))
	{
		buffer[length+reader] = byte;
		length++;
	}
	else if(reader>0)
	{
		reorder();
		append(byte);
	}
}

unsigned char BinaryStream::remove()
{
	unsigned char byte = 0; // Not the best solution, it returns a 0 if length = 0
	if(length>0)
	{
		byte = buffer[reader];
		reader++;
		length--;
	}
	return byte;
}

void BinaryStream::reorder()
{
	for(unsigned int i = 0; i<bytes; i++)
	{
		buffer[i] = buffer[i+reader];
	}
	reader = 0;
}




BinaryStream& BinaryStream::operator<<(const bool val)
{
	const unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(bool); i++)
	{
		append(pointer[i]);
	}
	return *this;
}

BinaryStream& BinaryStream::operator<<(const unsigned char val)
{
	const unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(unsigned char); i++)
	{
		append(pointer[i]);
	}
	return *this;
}

BinaryStream& BinaryStream::operator<<(const char val)
{
	const unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(char); i++)
	{
		append(pointer[i]);
	}
	return *this;
}

BinaryStream& BinaryStream::operator<<(const unsigned short int val)
{
	const unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(unsigned short int); i++)
	{
		append(pointer[i]);
	}
	return *this;
}

BinaryStream& BinaryStream::operator<<(const short int val)
{
	const unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(short int); i++)
	{
		append(pointer[i]);
	}
	return *this;
}

BinaryStream& BinaryStream::operator<<(const unsigned int val)
{
	const unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(unsigned int); i++)
	{
		append(pointer[i]);
	}
	return *this;
}

BinaryStream& BinaryStream::operator<<(const int val)
{
	const unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(int); i++)
	{
		append(pointer[i]);
	}
	return *this;
}

BinaryStream& BinaryStream::operator<<(const unsigned long int val)
{
	const unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(unsigned long int); i++)
	{
		append(pointer[i]);
	}
	return *this;
}

BinaryStream& BinaryStream::operator<<(const long int val)
{
	const unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(long int); i++)
	{
		append(pointer[i]);
	}
	return *this;
}

BinaryStream& BinaryStream::operator<<(const unsigned long long int val)
{
	const unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(unsigned long long int); i++)
	{
		append(pointer[i]);
	}
	return *this;
}

BinaryStream& BinaryStream::operator<<(const long long int val)
{
	const unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(long long int); i++)
	{
		append(pointer[i]);
	}
	return *this;
}

BinaryStream& BinaryStream::operator<<(const float val)
{
	const unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(float); i++)
	{
		append(pointer[i]);
	}
	return *this;
}

BinaryStream& BinaryStream::operator<<(const double val)
{
	const unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(double); i++)
	{
		append(pointer[i]);
	}
	return *this;
}

BinaryStream& BinaryStream::operator<<(const long double val)
{
	const unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(long double); i++)
	{
		append(pointer[i]);
	}
	return *this;
}

BinaryStream& BinaryStream::operator<<(const char string[])
{
	for(unsigned int i = 0; i<(strlen(string)+1); i++)
	{
		*this<<string[i];
	}
	return *this;
}

BinaryStream& BinaryStream::operator>>(const bool& val)
{
	unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(bool); i++)
	{
		pointer[i] = remove();
	}
	return *this;
}

BinaryStream& BinaryStream::operator>>(const unsigned char& val)
{
	unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(unsigned char); i++)
	{
		pointer[i] = remove();
	}
	return *this;
}

BinaryStream& BinaryStream::operator>>(const char& val)
{
	unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(char); i++)
	{
		pointer[i] = remove();
	}
	return *this;
}

BinaryStream& BinaryStream::operator>>(const unsigned short int& val)
{
	unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(unsigned short int); i++)
	{
		pointer[i] = remove();
	}
	return *this;
}

BinaryStream& BinaryStream::operator>>(const short int& val)
{
	unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(short int); i++)
	{
		pointer[i] = remove();
	}
	return *this;
}

BinaryStream& BinaryStream::operator>>(const unsigned int& val)
{
	unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(unsigned int); i++)
	{
		pointer[i] = remove();
	}
	return *this;
}

BinaryStream& BinaryStream::operator>>(const int& val)
{
	unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(int); i++)
	{
		pointer[i] = remove();
	}
	return *this;
}

BinaryStream& BinaryStream::operator>>(const unsigned long int& val)
{
	unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(unsigned long int); i++)
	{
		pointer[i] = remove();
	}
	return *this;
}

BinaryStream& BinaryStream::operator>>(const long int& val)
{
	unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(long int); i++)
	{
		pointer[i] = remove();
	}
	return *this;
}

BinaryStream& BinaryStream::operator>>(const unsigned long long int& val)
{
	unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(unsigned long long int); i++)
	{
		pointer[i] = remove();
	}
	return *this;
}

BinaryStream& BinaryStream::operator>>(const long long int& val)
{
	unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(long long int); i++)
	{
		pointer[i] = remove();
	}
	return *this;
}

BinaryStream& BinaryStream::operator>>(const float& val)
{
	unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(float); i++)
	{
		pointer[i] = remove();
	}
	return *this;
}

BinaryStream& BinaryStream::operator>>(const double& val)
{
	unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(double); i++)
	{
		pointer[i] = remove();
	}
	return *this;
}

BinaryStream& BinaryStream::operator>>(const long double& val)
{
	unsigned char* pointer = (unsigned char*)&val;
	for(unsigned int i = 0; i<sizeof(long double); i++)
	{
		pointer[i] = remove();
	}
	return *this;
}

BinaryStream& BinaryStream::operator>>(const char string[])
{
	unsigned int i = 0;

	while(buffer[reader-1]!=0&&length>0)
	{
		*this>>string[i];
		i++;
	}
	return *this;
}