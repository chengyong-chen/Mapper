#include "stdafx.h"

#include "FilterType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void AFXAPI operator <<(CArchive& ar, FilterType& filter)
{
	ar<<(BYTE)filter;
};

void AFXAPI operator >>(CArchive& ar, FilterType& filter)
{
	BYTE byte;
	ar>>byte;
	filter = (FilterType)byte;
};
