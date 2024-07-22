#pragma once

#include "Chartab.h"

using namespace std;
#include <string>
#include <sstream>

namespace StyleSheet
{
	class CTokenString
	{
	public:
		static bool MovetoNenetline(char*& it)
		{
			while(*it!=0&&*it!='\r')
				it++;

			char c = *it;
			if(c=='\r')
			{
				c = Next(it);
				if(c!='\n')
					Back(it);
				return true;
			}
			else
				return false;
		}

		static std::string read_string(char*& it, char start, char end)
		{
			while(*it!=0&&*it!=start)
				it++;

			int deepth = 0;
			std::string str;
			while(*it!=0)
			{
				const char c = *it++;
				if(c==0)
					return "";
				else if(c==start)
				{
					if(deepth!=0)
						str += c;
					deepth++;
				}
				else if(c==end)
				{
					deepth--;
					if(deepth==0)
					{
						it++;
						return str;
					}
					else
						str += c;
				}
				else
					str += c;
			}
			return "";
		}

		static std::string read_word(char*& it, WORD chars)
		{
			const WORD ignore = CHAR_WHITESPACE|CHAR_NEWLINE|CHAR_DELIMITER;
			while(*it!=0&&char_types[*it]&ignore!=0)
				it++;

			std::string str;
			char c = *it;
			while(*it!=0&&(c&chars)!=0)
			{
				str += c;
				c = *(++it);
			}

			return str;
		}

		static std::string read_string(char*& it, char end)
		{
			const WORD ignore = CHAR_WHITESPACE|CHAR_NEWLINE|CHAR_DELIMITER;
			while(*it!=0&&char_types[*it]&ignore!=0)
				it++;

			std::string str;
			char c = *it;
			while(*it!=0&&c!=end)
			{
				str += c;
				c = *(++it);
			}
			if(c==end)
				it++;
			return str;
		}

		static void Back(char*& it)
		{
			it--;
		}

		static char Next(char*& it)
		{
			if(it==0)
				return 0;
			else
				return *(++it);
		}

		static bool IsFloat(std::string str)
		{
			if(str.find('.')==std::string::npos)
				return false;

			istringstream iss(str.c_str());
			double dTestSink;
			iss>>dTestSink;
			return (iss.rdbuf()->in_avail()==0);
		}

		static bool IsInt(std::string str)
		{
			if(str.find('.')!=std::string::npos)
				return false;

			std::istringstream iss(str.c_str());
			int dTestSink;
			iss>>dTestSink;
			return iss.rdbuf()->in_avail()==0;
		}
	};
}
