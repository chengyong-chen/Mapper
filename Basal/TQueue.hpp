#pragma once

#include <queue>
#include <mutex>
#include <climits>
#include <condition_variable>

using namespace std;

template <typename T> 
class TQueue
{ 
private:
	std::queue<T> m_queue;
	std::mutex m_mutex;
	std::condition_variable m_condition;
	uint32_t m_maxitems;
	std::function<void(T&)> m_destructor;
public:
	TQueue() 
	{
		m_maxitems = UINT_MAX;
	}
	TQueue(unsigned int maxitems, std::function<void(T)> destructor)
		:m_maxitems(maxitems), m_destructor(destructor)
	{
	}
	~TQueue() 
	{
		m_condition.notify_one();
	}
	void push(T item) 
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		while (m_queue.size() > m_maxitems)
		{
			T first = m_queue.front();
			m_queue.pop();
			if (m_destructor != nullptr)
			{
				m_destructor(first);
			}
		}
		m_queue.push(item);
		lock.unlock();
		m_condition.notify_one();
	}
	T pop() 
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		T item = m_queue.front();
		m_queue.pop();
		lock.unlock();
		return item;
	}
	bool wait()
	{	
		if(this->size() > 0)
			return true;
		
		std::unique_lock<std::mutex> lock(m_mutex);
		m_condition.wait(lock);
		lock.unlock();
		return !empty();
	}
	void wakeup()
	{
		m_condition.notify_one();
	}
	int size() 
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		int size = m_queue.size();
		lock.unlock();
		return size;
	}
	bool empty() 
	{
		return size()>0 ? false : true;
	}
};