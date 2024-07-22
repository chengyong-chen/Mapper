#pragma once

class __declspec(dllexport) BinaryStream
{
public:
	BinaryStream(unsigned int size);
	BinaryStream(const unsigned char buf[], unsigned int size);
	BinaryStream& operator=(const BinaryStream& source); // Copy assignment operator
	~BinaryStream();

	unsigned char* getBuf() const
	{
		return buffer;
	};

	unsigned int getCapacity() const
	{
		return bytes;
	};

	unsigned int getLength() const
	{
		return length;
	};


	BinaryStream& operator<<(bool val);
	BinaryStream& operator<<(unsigned char val);
	BinaryStream& operator<<(char val);
	BinaryStream& operator<<(unsigned short int val);
	BinaryStream& operator<<(short int val);
	BinaryStream& operator<<(unsigned int val);
	BinaryStream& operator<<(int val);
	BinaryStream& operator<<(unsigned long int val);
	BinaryStream& operator<<(long int val);
	BinaryStream& operator<<(unsigned long long int val);
	BinaryStream& operator<<(long long int val);
	BinaryStream& operator<<(float val);
	BinaryStream& operator<<(double val);
	BinaryStream& operator<<(long double val);
	BinaryStream& operator<<(const char string[]);

	BinaryStream& operator>>(const bool& val);
	BinaryStream& operator>>(const unsigned char& val);
	BinaryStream& operator>>(const char& val);
	BinaryStream& operator>>(const unsigned short int& val);
	BinaryStream& operator>>(const short int& val);
	BinaryStream& operator>>(const unsigned int& val);
	BinaryStream& operator>>(const int& val);
	BinaryStream& operator>>(const unsigned long int& val);
	BinaryStream& operator>>(const long int& val);
	BinaryStream& operator>>(const unsigned long long int& val);
	BinaryStream& operator>>(const long long int& val);
	BinaryStream& operator>>(const float& val);
	BinaryStream& operator>>(const double& val);
	BinaryStream& operator>>(const long double& val);
	BinaryStream& operator>>(const char string[]);



private:
	void append(unsigned char byte);
	unsigned char remove();
	void reorder();
	void free();

	unsigned char* buffer;
	unsigned int bytes;
	unsigned int reader;
	unsigned int length;
};