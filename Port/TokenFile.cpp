#include "Stdafx.h"
#include "TokenFile.h"
#include "Chartab.h"

using namespace std;
#include <string>
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CTokenFile::read_newline(int c)
{
	if(c=='\r')
	{
		c = GETC();
		if(c!='\n')
			BACK(c);
	}

	beginning_of_line = 1;
}

std::string CTokenFile::read_string()//read (aaaaaa)
{
	int deepth = 1;
	std::string str;
	do
	{
		char c = GETC();
		switch(c)
		{
		case EOF:
			return "";
		case '(':
			deepth += 1;
			str += c;
			break;
		case ')':
			deepth -= 1;
			if(deepth==0)
				return str;
			else
				str += c;
			break;
		case '\\':
			c = GETC();
			switch(c)
			{
			case 'b':
				str += '\b';
				break;
			case 'f':
				str += '\f';
				break;
			case 'n':
				str += '\n';
				break;
			case 'r':
				str += '\r';
				break;
			case 't':
				str += '\t';
				break;
			case '\\':
				str += '\\';
				break;
			case '(':
				str += '(';
				break;
			case ')':
				str += ')';
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			{
				char code = c-'0';
				c = GETC();
				if(c>='0'&&c<='7')
				{
					code = code*8+c-'0';
					c = GETC();
					if('0'<=c&&c<='7')
					{
						code = code*8+c-'0';
						c = GETC();
					}
				}
				str += code;
				BACK(c);
			}
			break;
			case '\r':
				c = GETC();
				if(c!='\n')
					BACK(c);
				break;
			case '\n':
				break;
			default:
				str += c;
				break;
			}
			break;
		case '\r':
			c = GETC();
			if(c!='\n')
				BACK(c);
			str += '\n';
			break;
		default:
			str += c;
			break;
		}
	} while(true);

	return "";
}

std::string CTokenFile::read_hex_string()//read <FFFFFFFFF>
{
	std::string str;

	do
	{
		const char c = GETC();
		switch(c)
		{
		case EOF:
			return "";
		case '>':
			return str;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			str += c;
			break;
		default:
			if(!(char_types[c]&CHAR_WHITESPACE))
				return "";
			break;
		}
	} while(true);
}

void CTokenFile::BACK(int c)
{
	CFile::Seek(-1, CFile::current);
}

char CTokenFile::GETC()
{
	char c;
	if(CFile::Read(&c, 1)!=1)
		return EOF;
	else
		return c;
}

bool CTokenFile::IsFloat(std::string str)
{
	if(str.find('.')==std::string::npos)
		return false;

	istringstream iss(str);
	double dTestSink;
	iss>>dTestSink;
	return (iss.rdbuf()->in_avail()==0);
}

bool CTokenFile::IsInt(std::string str)
{
	if(str.find('.')!=std::string::npos)
		return false;

	std::istringstream iss(str.c_str());
	int dTestSink;
	iss>>dTestSink;
	return iss.rdbuf()->in_avail()==0;
}
