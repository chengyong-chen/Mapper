#include "stdafx.h"
#include "Page.h"

#include  <stdlib.h>

#include "../Viewer/Global.h"

#include "../Public/Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL AFXAPI AfxComparePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2);

IMPLEMENT_SERIAL(CPage, CTreeNode1, 0)

CPage::CPage()
	: CTreeNode1()
{
	m_wType = 0;

	m_wId = 0;
	m_wParent = 0;
	m_strName = _T("Map");

	m_Anchor.x = 0;
	m_Anchor.y = 0;
}

CPage::~CPage()
{
}

void CPage::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CTreeNode1::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		CString strTarget = m_strTarget;
		if(m_wType!=CPage::typeFolder)
		{
			AfxToRelatedPath(ar.m_strFileName, strTarget);
		}

		ar<<strTarget;
	}
	else
	{
		ar>>m_strTarget;

		if(m_wType!=CPage::typeFolder)
		{
			m_strTarget.MakeUpper();
			if(m_strTarget.IsEmpty()==false)
			{
				AfxToFullPath(ar.m_strFileName, m_strTarget);
			}
		}
	}
}

void CPage::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
		CString strFile = m_strTarget;
		AfxToRelatedPath(ar.m_strFileName, strFile);
		strFile.MakeUpper();
		strFile.Replace(_T(".GIS"), _T(".Cis"));
		strFile.Replace(_T(".Grf"), _T(".Cis"));

		SerializeStrCE(ar, strFile);
	}
}

CDocument* CPage::Open(CWnd* pWnd, unsigned int nScale) const
{
	if(pWnd!=nullptr)
	{
		if(pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd))==TRUE)
		{
			CView* pView = ((CFrameWnd*)pWnd)->GetActiveView();
			if(pView!=nullptr)
			{
				CDocument* pDocument = pView->GetDocument();
				if(pDocument!=nullptr)
				{
					const CString strFile = pDocument->GetPathName();

					TCHAR szMap[_MAX_PATH];
					GetLongPathName(m_strTarget, szMap, _MAX_PATH);
					TCHAR szFile[_MAX_PATH];
					GetLongPathName(strFile, szFile, _MAX_PATH);

					if(AfxComparePath(szMap, szFile)==TRUE)
					{
						return pDocument;
					}
				}
			}

			CDocument* pDocument = nullptr;
			if(pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd))==TRUE)
			{
				pDocument = (CDocument*)pWnd->SendMessage(WM_OPENDOCUMENTFILE, (UINT)&m_strTarget, nScale);
				pWnd->SendMessage(WM_ADDHISTORY, (UINT)pDocument, nScale==1000 ? 0 : 1);
			}
			else
				pDocument = AfxGetApp()->OpenDocumentFile(m_strTarget);

			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"), _T(""));
			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"), _T(""));
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"), 0);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"), 0);
			return pDocument;
		}
		else
		{
			return (CDocument*)pWnd->SendMessage(WM_OPENDOCUMENTFILE, (UINT)&m_strTarget, nScale);
		}
	}
	else
	{
		return AfxGetApp()->OpenDocumentFile(m_strTarget);
	}
}
