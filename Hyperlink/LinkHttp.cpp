#include "stdafx.h"
#include "Link.h"
#include "LinkHttp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CLinkHttp, CLink, 0)

CLinkHttp::CLinkHttp()
{
}

CLink* CLinkHttp::Clone()
{
	CLinkHttp* pClone = new CLinkHttp;

	pClone->m_dwGeomId = m_dwGeomId;
	pClone->m_strTarget = m_strTarget;

	ASSERT_VALID(pClone);
	return pClone;
}

BOOL CLinkHttp::Into(CWnd* pWnd)
{
	CWinApp* pApp = AfxGetApp();
	const CString strTarget = _T("Http://"+m_strTarget);
	const HWND hWndDesktop = ::GetDesktopWindow();
	::ShellExecute(hWndDesktop, _T("open"), strTarget, nullptr, _T(""), SW_SHOW);

	return FALSE;
}

void CLinkHttp::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CLink::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_strTarget;
	}
	else
	{
		ar>>m_strTarget;
	}
}

void CLinkHttp::ReleaseDCOM(CArchive& ar)
{
	CLink::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_strTarget;
	}
	else
	{
		ar>>m_strTarget;
	}
}
