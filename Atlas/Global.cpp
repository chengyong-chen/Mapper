#include "stdafx.h"
#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL IsWebFile(CString strFile)
{
	strFile.MakeLower();
	if(strFile.GetLength()>4&&strFile.Right(4)==_T(".htm"))
		return TRUE;
	else if(strFile.GetLength()>5&&strFile.Right(5)==_T(".html"))
		return TRUE;
	else if(strFile.GetLength()>4&&strFile.Right(4)==_T(".swf"))
		return TRUE;
	else
		return FALSE;
}

BOOL IsMapFile(CString strFile)
{
	strFile.MakeLower();
	if(strFile.GetLength()>4&&strFile.Right(4)==_T(".gis"))
		return TRUE;
	else if(strFile.GetLength()>4&&strFile.Right(4)==_T(".grf"))
		return TRUE;
	else
		return FALSE;
}
