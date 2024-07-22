#include "stdafx.h"

#include "BackInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBackInfo::CBackInfo()
{
}

void CBackInfo::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar<<m_strProvider;
		ar<<m_strVariety;
	}
	else
	{
		ar>>m_strProvider;
		ar>>m_strVariety;
	}
}
