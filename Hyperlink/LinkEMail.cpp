#include "stdafx.h"

#include "Link.h"
#include "LinkEMail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CLinkEMail, CLink, 0)

CLinkEMail::CLinkEMail()
{
}

CLink* CLinkEMail::Clone()
{
	CLinkEMail* pClone = new CLinkEMail;

	pClone->m_dwGeomId = m_dwGeomId;
	pClone->m_strTarget = m_strTarget;

	ASSERT_VALID(pClone);
	return pClone;
}

BOOL CLinkEMail::Into(CWnd* pWnd)
{
	CWinApp* pApp = AfxGetApp();
	const CString strTarget = _T("Mailto:")+m_strTarget;
	const HWND hWndDesktop = ::GetDesktopWindow();
	::ShellExecute(hWndDesktop, _T("open"), strTarget, nullptr, _T(""), SW_SHOW);

	return FALSE;
}

void CLinkEMail::Serialize(CArchive& ar, const DWORD& dwVersion)
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

void CLinkEMail::ReleaseDCOM(CArchive& ar)
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
