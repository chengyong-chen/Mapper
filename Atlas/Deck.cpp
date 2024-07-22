#include "stdafx.h"
#include <stdlib.h>
#include <io.h>

#include "Deck.h"
#include "MapItem.h"

#include "../Mapper/Global.h"
#include "../Viewer/Global.h"
#include "../Geometry/PolyF.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL AFXAPI AfxComparePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2);

IMPLEMENT_SERIAL(CDeck, CTreeNode1, 0)

/////////////////////////////////////////////////////////////////////////////
// CDeck

CDeck::CDeck()
	: CTreeNode1()
{
	m_bNavigatable = FALSE;
	m_bQueryble = TRUE;

	m_Rect.SetRectEmpty();

	m_wType = 1;
	m_pMapItem = nullptr;
	m_pParent = nullptr;
}

CDeck::~CDeck()
{
	POSITION pos = m_polylist.GetHeadPosition();
	while(pos!=nullptr)
	{
		const CPolyF* poly = m_polylist.GetNext(pos);
		delete poly;
	}
	m_polylist.RemoveAll();

	delete m_pMapItem;
}

CDeck* CDeck::GetParent() const
{
	return m_pParent;
}

CDeck* CDeck::GetChild(const CPointF& point)
{
	POSITION pos = m_Children.GetHeadPosition();
	while(pos!=nullptr)
	{
		CDeck* pDeck = m_Children.GetNext(pos);
		if(pDeck->PointIn(point)==true)
		{
			return pDeck;
		}
	}

	return nullptr;
}

void CDeck::RecalRect()
{
	m_Rect.EmptyRect();

	POSITION pos = m_polylist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPolyF* poly = m_polylist.GetNext(pos);
		const CRectF rect = poly->m_rect;
		m_Rect.UnionRect(rect);
	}
}

bool CDeck::PointIn(const CPointF& point) const
{
	if(m_Rect.PtInRect(point)==FALSE)
		return false;

	POSITION pos = m_polylist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CPolyF* poly = m_polylist.GetNext(pos);
		if(poly->PickIn(point)==true)
			return true;
	}

	return false;
}

void CDeck::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CTreeNode1::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		CString strFile = m_strFile;
		AfxToRelatedPath(ar.m_strFileName, strFile);
		CString strHtml = m_strHtml;
		AfxToRelatedPath(ar.m_strFileName, strHtml);
		CString strLegend = m_strLegend;
		AfxToRelatedPath(ar.m_strFileName, strLegend);

		ar<<strFile;
		ar<<strHtml;
		ar<<strLegend;

		ar<<m_bNavigatable;
		ar<<m_bQueryble;
	}
	else
	{
		ar>>m_strFile;
		ar>>m_strHtml;
		ar>>m_strLegend;

		ar>>m_bNavigatable;
		ar>>m_bQueryble;

		m_strFile.MakeUpper();
		if(m_strFile.IsEmpty()==false)
		{
			AfxToFullPath(ar.m_strFileName, m_strFile);
		}
		if(m_strHtml.IsEmpty()==false)
		{
			AfxToFullPath(ar.m_strFileName, m_strHtml);
		}
		if(m_strLegend.IsEmpty()==false)
		{
			AfxToFullPath(ar.m_strFileName, m_strLegend);
		}
	}

	CPolyF::SerializeList(ar, m_polylist);
	//	m_polylist.Serialize(ar);

	if(ar.IsStoring())
	{
	}
	else
	{
		POSITION pos = m_polylist.GetHeadPosition();
		if(pos!=nullptr)
		{
			CPolyF* poly = m_polylist.GetNext(pos);
			if(poly!=nullptr)
			{
				m_Rect = poly->m_rect;
				while(pos!=nullptr)
				{
					CPolyF* poly = m_polylist.GetNext(pos);
					const CRectF rect = poly->m_rect;
					m_Rect.UnionRect(rect);
				}
			}
		}
	}
}

void CDeck::SerializeWEB(CArchive& ar) const
{
}

void CDeck::PublishPC(CDaoDatabase* pDB, CDaoRecordset& rs, CString strTable)
{
	if(_taccess(m_strFile, 00)==-1)
	{
		CString string;
		string.Format(_T("%s wasn't found!"), m_strFile);
		AfxMessageBox(string);

		CString strSQL;
		strSQL.Format(_T("DELETE FROM %s WHERE Map=%u"), strTable, m_wId);
		pDB->Execute(strSQL);
	}
	else
	{
		CString strFile = m_strFile;
		strFile = strFile.Left(strFile.ReverseFind(_T('.')));
		const CString strGrid = strFile+_T(".Grd");
		const CString strGeom = strFile+_T(".Geo");
		if(_taccess(strGrid, 0)==-1||_taccess(strGeom, 0)==-1)
		{
			CString strSQL;
			strSQL.Format(_T("DELETE FROM %s WHERE Map=%u"), strTable, m_wId);
			pDB->Execute(strSQL);

			CDocument* pDocument = AfxGetApp()->OpenDocumentFile(m_strFile);
			if(pDocument!=nullptr)
			{
				POSITION pos = pDocument->GetFirstViewPosition();
				if(pos!=nullptr)
				{
					const CView* pView = pDocument->GetNextView(pos);
					pView->SendMessage(WM_PUBLISHPC, (UINT)this, (LONG)&rs);
				}
				pDocument->OnCloseDocument();
			}
		}
	}
}

void CDeck::PublishCE()
{
	if(_taccess(m_strFile, 00)==-1)
	{
		CString string;
		string.Format(_T("%s wasn't found!"), m_strFile);
		AfxMessageBox(string);
	}
	else
	{
		CString strFile = m_strFile;
		strFile = strFile.Left(strFile.ReverseFind(_T('.')));
		const CString strGrid = strFile+_T(".Grd");
		const CString strGeom = strFile+_T(".Geo");
		if(_taccess(strGrid, 0)==-1||_taccess(strGeom, 0)==-1)
		{
			CDocument* pDocument = AfxGetApp()->OpenDocumentFile(m_strFile);
			if(pDocument!=nullptr)
			{
				POSITION pos = pDocument->GetFirstViewPosition();
				if(pos!=nullptr)
				{
					const CView* pView = pDocument->GetNextView(pos);
					pView->SendMessage(WM_PUBLISHCE, (UINT)this, 0);
				}
				pDocument->OnCloseDocument();
			}
		}
	}
}

void CDeck::PublishWEB() const
{
	if(_taccess(m_strFile, 00)==-1)
	{
		CString string;
		string.Format(_T("%s wasn't found!"), m_strFile);
		AfxMessageBox(string);
	}
	else
	{
		CDocument* pDocument = AfxGetApp()->OpenDocumentFile(m_strFile);
		if(pDocument!=nullptr)
		{
			/*POSITION pos = pDocument->GetFirstViewPosition();
			if(pos != nullptr)
			{
				CView* pView  = pDocument->GetNextView(pos);
				pView->SendMessage(WM_PUBLISHWEB,(UINT)this,0);
			}*/
			pDocument->OnCloseDocument();
		}
	}
}

CDocument* CDeck::Open(CWnd* pWnd, unsigned int nScale) const
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
					GetLongPathName(m_strFile, szMap, _MAX_PATH);
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
				pDocument = (CDocument*)pWnd->SendMessage(WM_OPENDOCUMENTFILE, (UINT)&m_strFile, nScale);
				pWnd->SendMessage(WM_ADDHISTORY, (UINT)pDocument, nScale==1000 ? 0 : 1);
			}
			else
				pDocument = AfxGetApp()->OpenDocumentFile(m_strFile);

			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"), _T(""));
			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"), _T(""));
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"), 0);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"), 0);
			return pDocument;
		}
		else
		{
			return (CDocument*)pWnd->SendMessage(WM_OPENDOCUMENTFILE, (UINT)&m_strFile, nScale);
		}
	}
	else
	{
		CDocument* pDocument = AfxGetApp()->OpenDocumentFile(m_strFile);
		return pDocument;
	}
}

BOOL CDeck::Open()
{
	if(m_pMapItem!=nullptr)
	{
		return TRUE;
	}
	else
	{
		/*	CMapItem* pMapItem = new CMapItem();
			pMapItem->m_wMapId = m_wId;
			if(pMapItem->Open(m_strFile) == TRUE)
			{
				m_pMapItem = pMapItem;
				return TRUE;
			}
			else
			{
				delete pMapItem;
				pMapItem = nullptr;
				return FALSE;
			}*/

		m_pMapItem = new CMapItem();
		m_pMapItem->m_wMapId = m_wId;
		if(m_pMapItem->Open(m_strFile)==TRUE)
		{
			return TRUE;
		}
		else
		{
			delete m_pMapItem;
			m_pMapItem = nullptr;
			return FALSE;
		}
	}
}
