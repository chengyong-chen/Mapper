#include "stdafx.h"

#include "Link.h"
#include "LinkMap.h"
#include "LinkHttp.h"
#include "LinkEMail.h"
#include "LinkFile.h"
#include "LinkHtml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CLink, CObject)

CLink::CLink()
{
	m_dwGeomId = 0;
}

BOOL CLink::Into(CWnd* pWnd)
{
	CWinApp* pApp = AfxGetApp();

	return FALSE;
}

void CLink::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar<<m_dwGeomId;
	}
	else
	{
		ar>>m_dwGeomId;
	}
}

void CLink::ReleaseDCOM(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar<<m_dwGeomId;
	}
	else
	{
		ar>>m_dwGeomId;
	}
}

CLink* CLink::Apply(BYTE index)
{
	CLink* link = nullptr;

	switch(index)
	{
	case 0:
		break;
	case 1:
		link = new CLinkMap;
		break;
	case 2:
		link = new CLinkHttp;
		break;
	case 3:
		link = new CLinkHtml;
		break;
	case 4:
		link = new CLinkEMail;
		break;
	case 5:
		link = new CLinkFile;
		break;
	}

	return link;
}
