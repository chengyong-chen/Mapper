#include "stdafx.h"

#include "Link.h"
#include "LinkMap.h"

#include "../Viewer/Global.h"

#include "../Public/Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CLinkMap, CLink, 0)

CLinkMap::CLinkMap()
{
	m_strTarget.Empty();
	m_Anchor = CPoint(0, 0);
	m_nScale = 10000;
	m_bAutarchy = false;
}

BOOL CLinkMap::Into(CWnd* pWnd)
{
	CWinApp* pApp = AfxGetApp();
	AfxGetApp()->WriteProfileInt(_T("NewView"), _T("DocX"), m_Anchor.x);
	AfxGetApp()->WriteProfileInt(_T("NewView"), _T("DocY"), m_Anchor.y);

	if(pWnd!=nullptr)
	{
		CDocument* pDocument = (CDocument*)pWnd->SendMessage(WM_OPENDOCUMENTFILE, (UINT)&m_strTarget, m_nScale);
		if(pDocument!=nullptr)
		{
			pWnd->SendMessage(WM_ADDHISTORY, (UINT)pDocument, 1);
			return TRUE;
		}
		else
			return FALSE;
	}
	else
	{
		CDocument* pDocument = AfxGetApp()->OpenDocumentFile(m_strTarget);
		return pDocument==nullptr ? FALSE : TRUE;
	}
}

CLink* CLinkMap::Clone()
{
	CLinkMap* pClone = new CLinkMap;

	pClone->m_dwGeomId = m_dwGeomId;
	pClone->m_strTarget = m_strTarget;
	pClone->m_Anchor = m_Anchor;
	pClone->m_nScale = m_nScale;
	pClone->m_bAutarchy = m_bAutarchy;

	ASSERT_VALID(pClone);
	return pClone;
}

void CLinkMap::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CLink::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		CString strMap = m_strTarget;
		AfxToRelatedPath(ar.m_strFileName, strMap);
		ar<<strMap;
		ar<<m_Anchor;
		ar<<m_nScale;
		ar<<m_bAutarchy;
	}
	else
	{
		ar>>m_strTarget;
		ar>>m_Anchor;
		WORD uZoom;
		ar>>uZoom;
		m_nScale = uZoom;
		ar>>m_nScale;
		ar>>m_bAutarchy;
		AfxToFullPath(ar.m_strFileName, m_strTarget);
	}
}

void CLinkMap::ReleaseDCOM(CArchive& ar)
{
	CLink::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		CString strMap = m_strTarget;
		AfxToRelatedPath(ar.m_strFileName, strMap);
		ar<<strMap;
		ar<<m_Anchor;
		ar<<m_nScale;
		ar<<m_bAutarchy;
	}
	else
	{
		ar>>m_strTarget;
		ar>>m_Anchor;
		ar>>m_nScale;
		ar>>m_bAutarchy;

		AfxToFullPath(ar.m_strFileName, m_strTarget);
	}
}
