#include "stdafx.h"
#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int atohex(char* p, int cnt)
{
	int val = 0;
	char ch = 0;
	int pow = 1;

	for(int i = cnt; i>0; i--)
	{
		int v = 0;
		ch = *(p+i-1);
		if('A'<=ch&&'Z'>=ch)
		{
			v = 10+(ch-'A');
		}
		else if('a'<=ch&&'z'>=ch)
		{
			v = 10+(ch-'a');
		}
		else if('0'<=ch&&'9'>=ch)
			v = ch-'0';

		if(i==cnt)
			pow = 1;
		else
			pow *= 16;

		val += v*pow;
	}

	return val;
}
