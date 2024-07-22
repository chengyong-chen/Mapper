#include "Stdafx.h"
#include "PSTokenFile.h"
#include "Chartab.h"

#include <cstdlib>

#include <boost/any.hpp>
#include <boost/Tokenizer.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPSTokenFile::CPSTokenFile()
{
	beginning_of_line = 1;
	ai_pseudo_comments = 1;
	ai_dsc = 1;
}

/* Return the contents of a DSC-comment as a string object.
*
* The stream is assumed to be positioned after the '%%' or '%!'.
* Afterwards, the stream is positioned at the beginning of the next
* line or EOF. The string does not contain the final newline character.
*/
std::string CPSTokenFile::ReadWholeLine()// %%.... or %!.......
{
	std::string str;
	do
	{
		const unsigned char c = GETC();
		if(c==EOF)
			break;
		else if((char_types[c]&CHAR_NEWLINE)==CHAR_NEWLINE)
		{
			read_newline(c);
			break;
		}
		else
			str += c;
	} while(true);

	return str;
}

void CPSTokenFile::skip_to_dsc(std::string string1, std::string string2)
{
	typedef boost::tokenizer<boost::char_separator<char>> Tokenizer;

	while(true)
	{
		std::string value = Next_dsc();
		if(value=="")
			break;

		std::string strKey;
		if(value.find(':')!=-1)
		{
			Tokenizer tok(value, boost::char_separator<char>(":"));
			Tokenizer::iterator it = tok.begin();
			strKey = *it++;
		}
		else
			strKey = value;

		if(strKey==string1)
			break;
		else if(strKey==string2)
			break;
	}
}

void CPSTokenFile::discard_comment()
{
	do
	{
		const char c = GETC();
		if(c==EOF)
			break;
		else if(char_types[c]&CHAR_NEWLINE)
		{
			read_newline(c);
			break;
		}
	} while(true);
}

/* Read a comment. The stream is assumed to be positioned just after the
* initial '%'. If the comment is a DSC-comment (the next char is a '%'
* or a '!' and the comment is at the beginning of a line), the contents
* of the comment (without the leading '%%' or '%!') are returned as a
* string object.
*
* In a normal comments discard all input until the next newline and
* return nullptr.
*
* Furthermore, if ai_pseudo_comments is true, recognize Adobe
* Illustrator pseudo comments, which start with '%_' and discard just
* these two characters. The rest of the comment is treated as normal
* input.
*/
std::string CPSTokenFile::read_comment()
{
	std::string str;
	const char c = GETC();
	if(beginning_of_line&&(c=='%'||c=='!'))
		return ReadWholeLine();
	else if(c=='_'&&ai_pseudo_comments==true)
	{
	}
	else if(beginning_of_line&&c=='A'&&ai_dsc)
	{
		BACK(c);
		return ReadWholeLine();
	}
	else if(c!=EOF&&(char_types[c]&CHAR_NEWLINE))
		read_newline(c);
	else
		discard_comment();

	return str;
}

std::string CPSTokenFile::read_name()
{
	std::string value;
	do
	{
		const char c = GETC();
		if(c==EOF)
		{
			break;
		}
		else if((char_types[c]&CHAR_NAMECHAR)==0)
		{
			BACK(c);
			break;
		}
		else
		{
			value += c;
		}
	} while(true);

	return value;
}

std::any CPSTokenFile::read_number(int* token)
{
	std::string value;
	do
	{
		const char c = GETC();
		if(c==EOF)
			break;
		else if((char_types[c]&CHAR_NAMECHAR)==0)
		{
			BACK(c);
			break;
		}
		else
			value += c;
	} while(true);

	if(this->IsFloat(value)==true)
	{
		*token = FLOAT;
		return strtod(value.c_str(), nullptr);
	}
	else if(this->IsInt(value)==true)
	{
		*token = INT;
		return atoi(value.c_str());
	}
	else
	{
		*token = OPERATOR;
		return value;
	}
}

std::any CPSTokenFile::Next(int& token)
{
	token = 0;
	while(token==0)
	{
		char c = GETC();
		if(c!='%')
			beginning_of_line = 0;

		switch(c)
		{
		case EOF:
			token = END;
			return -1;
			break;
		case '%':
			token = DSC_COMMENT;
			return read_comment();
			break;
		case '[': /* a single character token */
		case ']':
		case '{':
		case '}':
		{
			token = OPERATOR;
			std::string str;
			str += c;
			return str;
		}
		break;
		case '(':
			token = STRING;
			return read_string();
			break;
		case '<':
			/* this case should check the next character to also
			recognize the << operator and base85 encoded strings */
			token = STRING;
			return read_hex_string();
			break;
		case '/':
			token = NAME;
			return read_name();
			break;
		default:
			int ctype = char_types[c];
			if(ctype&CHAR_WHITESPACE)
			{
				while(ctype&CHAR_WHITESPACE)
				{
					beginning_of_line = (ctype&CHAR_NEWLINE)==CHAR_NEWLINE;

					c = GETC();
					if(c==EOF)
						break;
					else
						ctype = char_types[c];
				}
				if(c!=EOF)
					BACK(c);
			}
			else if(ctype&CHAR_NAMECHAR)
			{
				BACK(c);
				/* NAMECHAR includes digits */
				return read_number(&token);
			}
			else
			{
				token = END;
				return -1;
			}
			break;
		}
	}
	return -1;
}

/*
*
*/

std::string CPSTokenFile::Next_dsc()
{
	for(;;)
	{
		const char c = GETC();
		if(c==EOF)
			break;
		else if(char_types[c]&CHAR_NEWLINE)
			read_newline(c);
		else if(beginning_of_line&&c=='%')
			return read_comment();
		else
			beginning_of_line = 0;
	}

	return "";
}
