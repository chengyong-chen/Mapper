#include "stdafx.h"
#include "Link.h"
#include "LinkFile.h"

#include "../Public/Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CLinkFile, CLink, 0)

CLinkFile::CLinkFile()
{
}

CLink* CLinkFile::Clone()
{
	CLinkFile* pClone = new CLinkFile;

	pClone->m_dwGeomId = m_dwGeomId;
	pClone->m_strTarget = m_strTarget;

	ASSERT_VALID(pClone);
	return pClone;
}

BOOL CLinkFile::Into(CWnd* pWnd)
{
	CWinApp* pApp = AfxGetApp();
	const HWND hWndDesktop = ::GetDesktopWindow();
	::ShellExecute(hWndDesktop, _T("open"), m_strTarget, nullptr, _T(""), SW_SHOW);

	return FALSE;
}

void CLinkFile::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CLink::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		CString strFile = m_strTarget;
		AfxToRelatedPath(ar.m_strFileName, strFile);
		ar<<strFile;
	}
	else
	{
		ar>>m_strTarget;
		AfxToFullPath(ar.m_strFileName, m_strTarget);
	}
}

void CLinkFile::ReleaseDCOM(CArchive& ar)
{
	CLink::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		CString strFile = m_strTarget;
		AfxToRelatedPath(ar.m_strFileName, strFile);
		ar<<strFile;
	}
	else
	{
		ar>>m_strTarget;
		AfxToFullPath(ar.m_strFileName, m_strTarget);
	}
}
