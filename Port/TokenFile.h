#pragma once

using namespace std;
#include <string>

class CTokenFile : public CFile
{
public:
	void read_newline(int c);
	std::string read_string();
	std::string read_hex_string();//read <FFFFFFFFF>	
	void BACK(int c);
	char GETC();

	static bool IsFloat(std::string);

	static bool IsInt(std::string);
public:
	int beginning_of_line;
};
