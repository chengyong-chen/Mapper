#pragma once
#include <any>

#include "TokenFile.h"

using namespace std;

#include "../../ThirdParty/boost/1.83.0/boost/any.hpp"

#define	NAME		258
#define	INT		    259
#define	FLOAT		260
#define	STRING		261
#define MAX_DATA_TOKEN  261
#define	OPERATOR	262
#define DSC_COMMENT	263
#define END		    264

class CPSTokenFile : public CTokenFile
{
public:
	CPSTokenFile();

public:
	int ai_pseudo_comments;
	int ai_dsc;
public:
	std::string ReadWholeLine();// %%.... or %!.......
	void discard_comment();
	std::string read_comment();
	std::string read_name();
	std::any read_number(int* token);
	std::any Next(int& token);
	std::string Next_dsc();
	void skip_to_dsc(std::string string1, std::string string2);
};
