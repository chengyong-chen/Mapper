#pragma once
#include <any>
#include <list>
#include <stack>
#include <string>

extern "C"
{
	__declspec(dllexport) std::list<std::pair<const char*, std::stack<std::any>>>* ParseFile(std::string ext, std::wstring file);

	 class __declspec(dllexport) FontDescriptor
	{
	public:
		std::string file;
		std::string family;
		std::string name;
	} ;
}
