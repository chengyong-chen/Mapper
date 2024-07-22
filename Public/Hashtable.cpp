#include "stdafx.h"
#include "Hashtable.h"

#include <unordered_map>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

template<class T>
CHashtable<T>::CHashtable()
{
}

template<class T>
CHashtable<T>::~CHashtable()
{
	for(std::unordered_map::iterator it = std::unordered_map::begin(); it!=std::unordered_map::end(); ++it)
	{
		T* value = it->second;
		delete value;
		value = nullptr;
	}

	std::unordered_map::clear();
}

template<class T>
void CHashtable<T>::Add(WORD wKey, T* value)
{
	if(std::unordered_map[wKey]==0)
		std::unordered_map[wKey] = value;
	else
		return;
}

template<class T>
void CHashtable<T>::Remove(T* pValue)
{
	for(std::unordered_map::iterator it = std::unordered_map::begin(); it!=std::unordered_map::end(); ++it)
	{
		WORD wKey = it->first;
		T* value = it->second;
		if(value==pValue)
		{
			delete value;
			value = nullptr;

			std::unordered_map::erase(it);
		}
	}
}

template<class T>
void CHashtable<T>::RemoveAll()
{
	for(std::unordered_map::iterator it = std::unordered_map::begin(); it!=std::unordered_map::end(); ++it)
	{
		T* value = it->second;
		delete value;
		value = nullptr;
	}
	std::unordered_map::clear();
}

template<class T>
T* CHashtable<T>::GetValue(WORD wKey)
{
	return std::unordered_map[wKey];
}

template<class T>
WORD CHashtable<T>::GetKey(T* value)
{
	for(std::unordered_map::iterator it = std::unordered_map::begin(); it!=std::unordered_map::end(); ++it)
	{
		T* pValue = it->second;
		if(pValue==value)
			return it->first;
	}

	return -1;
}

template<class T>
WORD CHashtable<T>::GetKey(T value)
{
	for(std::unordered_map::iterator it = std::unordered_map::begin(); it!=std::unordered_map::end(); ++it)
	{
		T* pValue = it->second;
		if(*pValue==value)
			return it->first;
	}

	return -1;
}

template<class T>
WORD CHashtable<T>::GetMaxKey()
{
	WORD wMaxId = 0;
	for(std::unordered_map::iterator it = std::unordered_map::begin(); it!=std::unordered_map::end(); ++it)
	{
		WORD wKey = it->first;
		wMaxId = max(wMaxId, wKey);
	}

	return wMaxId;
}
