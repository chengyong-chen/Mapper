#include "stdafx.h"

#include <stdio.h>
#include "../Public/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UNIT g_UnitArray[] =
{
	{_T("Inch"), 72.0f, 1.0f},
	{_T("Point"), 1.0f, 72.0f},
	{_T("Millimetre"),2.834f, 25.4f},
};

__declspec(dllexport) BYTE d_nUnitIndex = 0;
