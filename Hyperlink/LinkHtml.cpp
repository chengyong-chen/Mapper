#include "stdafx.h"
#include "Link.h"
#include "LinkHtml.h"

#include "../Viewer/Global.h"

#include "../Public/Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CLinkHtml, CLink, 0)

CLinkHtml::CLinkHtml()
{
}

CLink* CLinkHtml::Clone()
{
	CLinkHtml* pClone = new CLinkHtml;

	pClone->m_dwGeomId = m_dwGeomId;
	pClone->m_strTarget = m_strTarget;

	ASSERT_VALID(pClone);
	return pClone;
}

BOOL CLinkHtml::Into(CWnd* pWnd)
{
	AfxGetMainWnd()->SendMessage(WM_BROWSEHTML, (UINT)&m_strTarget, 1);
	return FALSE;
}

void CLinkHtml::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CLink::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		CString strHtml = m_strTarget;
		AfxToRelatedPath(ar.m_strFileName, strHtml);
		ar<<strHtml;
	}
	else
	{
		ar>>m_strTarget;
		AfxToFullPath(ar.m_strFileName, m_strTarget);
	}
}

void CLinkHtml::ReleaseDCOM(CArchive& ar)
{
	CLink::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		CString strHtml = m_strTarget;
		AfxToRelatedPath(ar.m_strFileName, strHtml);
		ar<<strHtml;
	}
	else
	{
		ar>>m_strTarget;

		AfxToFullPath(ar.m_strFileName, m_strTarget);
	}
}
