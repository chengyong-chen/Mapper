#pragma once

#include <unordered_map>

using namespace std;

template<class T>
class __declspec(dllexport) CHashtable : public unordered_map<WORD, T*>
{
protected:
	CHashtable();

public:
	virtual ~CHashtable();

public:
	static void Add(WORD wKey, T* value);
	void Remove(T* value);
	void Remove(WORD wKey);
	void RemoveAll();

	static T* GetValue(WORD wkey);
	WORD GetKey(T* value);
	WORD GetKey(T value);
public:
	WORD GetMaxKey();
};
