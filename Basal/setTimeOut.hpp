#pragma once

#include "time.h" 
#include <future>
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;

class Javascript
{
private:
	static std::mutex lock;
	static std::set<int> intervals;

public:
	template <typename... ParamTypes>
	static void setTimeOut(int milliseconds, std::function<void(ParamTypes...)> func, ParamTypes... parames)
	{
		std::function<void()> call=[=](){
			Sleep(milliseconds);
			func(parames...);
		};
		std::async(std::launch::async, call);
	};

	template <typename... ParamTypes>
	static int setInterval(int milliseconds,std::function<void(ParamTypes...)> func,ParamTypes... parames)
	{	
		std::unique_lock<std::mutex> locker(lock);

		srand (time (0));
		int id;
		do
		{
			id = rand();
		}while(intervals.find(id) != intervals.end());
		intervals.insert(id);
		locker.unlock();

		std::function<void()> call=[=](){
			Sleep(milliseconds);

			while(intervals.find(id) != intervals.end())
			{
				func(parames...);
				Sleep(milliseconds);
			}
		};
		std::async(std::launch::async,call);
		return id;
	};

	static void clearInterval(int id)
	{
		std::unique_lock<std::mutex> locker(lock);
		intervals.erase(id);
		locker.unlock();
	};
};
