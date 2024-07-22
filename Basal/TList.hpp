#pragma once

#include <list>
#include <mutex>

using namespace std;

template <typename T> 
class TList
{ 
private:
	std::list<T> m_items;
	std::mutex m_mutex;

public:
	TList() 
	{
	}
	~TList() 
	{
	}
	void push_back(T item) 
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_items.push_back(item);
		lock.unlock();
	}
	T* getat(int index) 
	{
		if(m_items.size()>0)
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			T* item = &(m_items[index]);
			lock.unlock();
			return item;
		}
		else
		{
			return NULL;
		}
	}
	int size() 
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		int size = m_items.size();
		lock.unlock();
		return size;
	}
	bool empty() 
	{
		return size()>0 ? false : true;
	}
};