#include "Stdafx.h"
#include "Stdstack.h"

#include <vector>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CStdstack::Push(std::any item)
{
	m_items.push(item);
}

std::any CStdstack::Pop()
{
	if(m_items.size()>0)
	{
		std::any item = m_items.top();
		m_items.pop();
		return item;
	}
	else
		return nullptr;
}
std::any CStdstack::Pop(std::stack<std::any>& items)
{
	if(items.size()>0)
	{
		std::any item = items.top();
		items.pop();
		return item;
	}
	else
		return nullptr;
}
Object CStdstack::Pop(std::stack<Object>& items)
{
	Object item = std::move(items.top());
	items.pop();
	return item;
}
std::vector<std::any> CStdstack::Pop(int count)
{
	std::vector<std::any> items;
	for(int index = 0; index<count; index++)
	{
		std::any item = this->Pop();

		items.push_back(item);
	}
	std::reverse(items.begin(), items.end());
	return items;
}

std::vector<std::any> CStdstack::PopTo(std::string str)
{
	std::vector<std::any> items;
	while(m_items.empty()==false)
	{
		std::any item = this->Pop();

		if(item.type()!=typeid(std::string))
		{
			items.push_back(item);
		}
		else if(str.compare(std::any_cast<std::string>(item))==0)
			break;
	}
	std::reverse(items.begin(), items.end());
	return items;
}

int CStdstack::PopInt()
{
	if(m_items.empty()==true)
		return -1;

	const std::any item = this->Pop();
	if(item.type()==typeid(int))
		return std::any_cast<int>(item);
	else if(item.type()==typeid(float))
		return round(std::any_cast<float>(item));
	else if(item.type()==typeid(double))
		return round(std::any_cast<double>(item));
	else
		return -1;
}
int CStdstack::PopInt(std::stack<std::any>& items)
{
	if(items.empty()==true)
		return -1;

	const std::any item = CStdstack::Pop(items);
	if(item.type()==typeid(int))
		return std::any_cast<int>(item);
	else if(item.type()==typeid(float))
		return round(std::any_cast<float>(item));
	else if(item.type()==typeid(double))
		return round(std::any_cast<double>(item));
	else
		return -1;
}
int CStdstack::PopInt(std::stack<Object>& items)
{
	if(items.empty()==true)
		return -1;

	const Object item = CStdstack::Pop(items);
	if(item.isInt())
		return item.getInt();
	else
		return -1;
}

float CStdstack::PopFloat()
{
	if(m_items.empty()==true)
		return -1;

	const std::any item = this->Pop();
	if(item.type()==typeid(float))
		return std::any_cast<float>(item);
	if(item.type()==typeid(double))
		return std::any_cast<double>(item);
	else if(item.type()==typeid(int))
		return std::any_cast<int>(item);
	else
		return -1.f;
}
double CStdstack::PopDouble()
{
	if(m_items.empty()==true)
		return -1;

	const std::any item = this->Pop();
	if(item.type()==typeid(double))
		return std::any_cast<double>(item);
	if(item.type()==typeid(float))
		return std::any_cast<float>(item);
	else if(item.type()==typeid(int))
		return std::any_cast<int>(item);
	else
		return -1.f;
}
double CStdstack::PopDouble(std::stack<std::any>& items)
{
	if(items.empty()==true)
		return -1;

	const std::any item = CStdstack::Pop(items);
	if(item.type()==typeid(double))
		return std::any_cast<double>(item);
	if(item.type()==typeid(float))
		return std::any_cast<float>(item);
	else if(item.type()==typeid(int))
		return std::any_cast<int>(item);
	else
		return -1.f;
}
double CStdstack::PopDouble(std::stack<Object>& items)
{
	if(items.empty()==true)
		return -1;

	const Object item = CStdstack::Pop(items);
	if(item.isReal())
		return item.getReal();
	else if(item.isInt())
		return item.getInt();
	else if(item.isNum())
		return item.getNum();
	else
		return -1.f;
}
float CStdstack::PopFloat(std::stack<std::any>& items)
{
	if(items.empty()==true)
		return -1;

	const std::any item = CStdstack::Pop(items);
	if(item.type()==typeid(float))
		return std::any_cast<float>(item);
	if(item.type()==typeid(double))
		return std::any_cast<double>(item);
	else if(item.type()==typeid(int))
		return std::any_cast<int>(item);
	else
		return -1.f;
}
float CStdstack::PopFloat(std::stack<Object>& items)
{
	if(items.empty()==true)
		return -1;

	const Object item = CStdstack::Pop(items);
	if(item.isReal())
		return (float)item.getReal();
	else if(item.isInt())
		return (float)item.getInt();
	else
		return -1.f;
}

std::string CStdstack::PopString()
{
	if(m_items.empty()==true)
		return "";

	std::any item = this->Pop();
	if(item.type()==typeid(std::string))
		return std::any_cast<std::string>(item);
	else
		return "";
}

std::string CStdstack::PopString(std::stack<std::any>& items)
{
	if(items.empty()==true)
		return "";

	std::any item = CStdstack::Pop(items);
	if(item.type()==typeid(std::string))
		return std::any_cast<std::string>(item);
	else
		return "";
}
std::string CStdstack::PopString(std::stack<Object>& items)
{
	if(items.empty()==true)
		return "";

	const Object item = CStdstack::Pop(items);
	if(item.isName())
		return item.getName();
	else if(item.isString())
		return item.getString()->toStr();
	else
		return "";
}
bool CStdstack::PopBool()
{
	if(m_items.empty()==true)
		return false;

	const std::any item = this->Pop();
	if(item.type()==typeid(bool))
		return std::any_cast<bool>(item);
	else if(item.type()==typeid(int))
		return std::any_cast<int>(item)==0 ? false : true;
	else
		return false;
}
bool CStdstack::PopBool(std::stack<std::any>& items)
{
	if(items.empty()==true)
		return false;

	const std::any item = CStdstack::Pop(items);
	if(item.type()==typeid(bool))
		return std::any_cast<bool>(item);
	else if(item.type()==typeid(int))
		return std::any_cast<int>(item)==0 ? false : true;
	else
		return false;
}
bool CStdstack::PopBool(std::stack<Object>& items)
{
	if(items.empty()==true)
		return false;

	const Object item = CStdstack::Pop(items);
	if(item.isBool())
		return item.getBool();
	else if(item.isInt())
		return item.getInt()==0 ? false : true;
	else
		return false;
}
void CStdstack::Clear()
{
	while(m_items.empty()==false)
	{
		m_items.pop();
	}
}
void CStdstack::Clear(std::stack<std::any>& items)
{
	while(items.empty()==false)
	{
		items.pop();
	}
}
void CStdstack::Clear(std::stack<Object>& items)
{
	while(items.empty()==false)
	{
		items.pop();
	}
}
