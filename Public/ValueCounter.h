#pragma once

#include <map>
using namespace std;

template<class T>
class __declspec(dllexport) CValueCounter
{
public:
	CValueCounter(void)
	{
	}

	virtual ~CValueCounter(void)
	{
		elements.clear();
	}

public:
	map<const T*, int> elements;

public:
	const T* CountIn(const T* pObject)
	{
		if(pObject==nullptr)
			return nullptr;

		typename map<const T*, int>::iterator it = elements.find(pObject);
		if(it!=elements.end())
		{
			elements[pObject]++;
			return nullptr;
		}
		else
		{
			for(it = elements.begin(); it!=elements.end(); it++)
			{
				const T* object = (*it).first;
				if(*object==*pObject)
				{
					(*it).second++;
					return pObject;
				}
			}
		}
		elements[pObject] = 1;
		return nullptr;
	}

public:
	const T* GetMaxObject()
	{
		const T* pObject = nullptr;
		int maximum = -1;
		for(typename map<const T*, int>::iterator it = elements.begin(); it!=elements.end(); it++)
		{
			if((*it).second>maximum)
			{
				pObject = (*it).first;
				maximum = (*it).second;
			}
		}
		return pObject;
	}

	void RemoveObject(const T* pObject)
	{
		typename map<const T*, int>::iterator it = elements.find(pObject);
		if(it!=elements.end())
		{
			elements.erase(it);
		}
	}

	void Clear()
	{
		for(typename map<const T*, int>::iterator it = elements.begin(); it!=elements.end(); it++)
		{
			delete (*it).first;
		}
		elements.clear();
	}
};
