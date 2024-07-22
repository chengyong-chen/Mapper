#pragma once

#include <windows.h>

using namespace std;

inline bool WStringToCharArray(const std::wstring& input, std::vector<char>& output)
{
	if(input.empty())
	{
		return false;
	}
	const int size = ::WideCharToMultiByte(CP_ACP, 0, input.c_str(), input.size(), NULL, 0, NULL, NULL);
	if(size<=0)
	{
		return false;
	}
	output.resize(size+1);
	if(::WideCharToMultiByte(CP_ACP, 0, input.c_str(), input.size(), &output[0], size, NULL, NULL)<=0)
	{
		output.clear();
		return false;
	}
	output[size] = '\0';
	return true;
}
