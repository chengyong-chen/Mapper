#pragma once

#include <vector>
#include <mutex>

using namespace std;

template <typename T> 
class TVector
{ 
private:
	std::vector<T> m_items;
	std::mutex m_mutex;

public:
	TVector() 
	{
	}
	~TVector() 
	{
	}
	void push_back(T item) 
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_items.push_back(item);
		lock.unlock();
	}
	T getat(unsigned int index) 
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		unsigned int size = m_items.size();
		T item = index<size ? m_items[index] : NULL;
		lock.unlock();
		return item;
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