#pragma once
#include <any>
#include <string>
#include <poppler/Object.h>

using namespace std;
#include <stack>
#include <vector>

class __declspec(dllexport) CStdstack
{
public:
	std::stack<std::any> m_items;

public:
	void Push(std::any item);
	std::any Pop();

	int PopInt();
	float PopFloat();
	double PopDouble();
	bool PopBool();
	std::string PopString();

	std::vector<std::any> Pop(int count);
	std::vector<std::any> PopTo(std::string str);
public:
	void Clear();
	static void Clear(std::stack<std::any>& items);
	static void Clear(std::stack<Object>& items);
public:
	static std::any Pop(std::stack<std::any>& items);
	static Object Pop(std::stack<Object>& items);
	static int PopInt(std::stack<std::any>& items);
	static int PopInt(std::stack<Object>& items);
	static float PopFloat(std::stack<std::any>& items);
	static float PopFloat(std::stack<Object>& items);
	static double PopDouble(std::stack<std::any>& items);
	static double PopDouble(std::stack<Object>& items);
	static bool PopBool(std::stack<std::any>& items);
	static bool PopBool(std::stack<Object>& items);
	static std::string PopString(std::stack<std::any>& items);
	static std::string PopString(std::stack<Object>& items);
};
