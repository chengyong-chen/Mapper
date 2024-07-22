#pragma once
#include <cassert>

namespace arr
{
	static bool equal(const unsigned int array1[5], const unsigned int array2[5])
	{
		for(int index = 0; index<5; index++)
		{
			if(array1[index]!=array2[index])
				return false;
		}
		return true;
	}
}