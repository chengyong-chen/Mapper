#pragma once			
#include <list>
template<typename T>
struct Crossing
{
	T* vertex = nullptr;
	bool isStart = false;
	bool isEnter = false;
	bool visited = false;
	Crossing* other = nullptr;
};

