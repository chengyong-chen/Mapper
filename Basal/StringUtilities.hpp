#pragma once

#include <string>
#include <functional>

using namespace std;

class StringUtils
{
	// Trims the characters specified in trimChars at the start of inString
public:
	static std::string TrimStart(const std::string& inString, const std::string& trimChars = " \t")
	{
		if(inString.empty())
		{
			return inString;
		}
		const size_t posLeft = inString.find_first_not_of(trimChars, 0);
		return inString.substr(posLeft, inString.length()-posLeft);
	}

	// Trims the characters specified in trimChars at the end of inString
public:
	static std::string TrimEnd(const std::string& inString, const std::string& trimChars = " \t")
	{
		if(inString.empty())
		{
			return inString;
		}
		const size_t posRight = inString.find_last_not_of(trimChars);
		return inString.substr(0, posRight+1);
	}

	// Trims the characters specified in trimChars at the start and end of inString
public:
	static std::string Trim(const std::string& inString, const std::string& trimChars = " \t")
	{
		if(inString.empty())
		{
			return inString;
		}

		const size_t posLeft = inString.find_first_not_of(trimChars, 0);
		const size_t posRight = inString.find_last_not_of(trimChars);

		return inString.substr(posLeft, posRight-posLeft+1);
	}
};

template<template<typename, typename> class TStlContainer, typename TElementType>
static std::string ToString(TStlContainer<TElementType, std::allocator<TElementType>> stlContainer, const std::string& delimiter = ", ")
{
	std::ostringstream ostr;

	std::copy(stlContainer.begin(), stlContainer.end(), std::ostream_iterator<TElementType>(ostr, delimiter.c_str()));

	std::string fmtText = ostr.str();
	const std::string::size_type index = fmtText.rfind(delimiter);
	fmtText.erase(index, delimiter.length());

	return fmtText;
}
