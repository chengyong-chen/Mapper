#include "stdafx.h"

#include "../Public/Global.h"

#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UNIT a_UnitArray[] =
{
	{_T("Inch"), 72.0f, 1.0f},
	{_T("Point"), 1.0f, 72.0f},
	{_T("Millimetre"),2.834f, 25.4f}
};

CString a_strUnitType = _T("Map");

BYTE a_nUnitIndex = 0;
long a_nGreekType = 3;
long a_nUndoNumber = 8;
float a_fJoinTolerance = 0.4f;
float a_fTopoTolerance = 3.0f;
float a_fDensity = 4;
unsigned char a_nRange = 5;

#define BASECOLOR 2
#define BASEDASH  1
