#include "stdafx.h"
#include "resource.h"
#include  <io.h>
#include  <stdio.h>
#include "GrfDoc.h"
#include "GrfView.h"
#include "MainFrm.h"

#include "../Mapper/Global.h"

#include "../Atlas/Atlas1.h"
#include "../Atlas/AtlasPane.h"
#include "../Atlas/LegendDlg.h"

#include "../Style/Spot.h"
#include "../Style/Midtable.hpp"

#include "../Layer/LayerPane.h"
#include "../Layer/LayerTreeCtrl.h"
#include "../Layer/GridMapFile.h"
#include "../Layer/GeomMapFile.h"

#include "../Geometry/Global.h"
#include "../Geometry/Geom.h"
#include "../Hyperlink/History.h"
#include "../Utility/Rect.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern UNIT a_UnitArray[3];
extern BYTE a_nUnitIndex;
extern BYTE a_nRange;

#include "../Tool/ZoomTool.h"
#include "../Tool/InfoTool.h"

extern __declspec(dllimport) CInfoTool infoTool;
extern __declspec(dllimport) CZoomTool zoominTool;

extern __declspec(dllimport) CArray<Gdiplus::RectF, Gdiplus::RectF&> d_oRectArray;

IMPLEMENT_DYNCREATE(CGrfDoc, COleDocument)

BEGIN_MESSAGE_MAP(CGrfDoc, COleDocument)
	//{{AFX_MSG_MAP(CGrfDoc)
	ON_COMMAND(ID_VIEW_LEGEND, OnViewLegend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CGrfDoc::CGrfDoc()
	: m_actionStack(*this)
{
	EnableCompoundFile(FALSE);
	m_dwMaxGeomId = 0;

	m_pLayerTreeCtrl = nullptr;

	CLayerPane* pLayerPane = (CLayerPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'L', 0);
	if(pLayerPane!=nullptr&&pLayerPane->m_hWnd!=nullptr)
	{
		m_pLayerTreeCtrl = new CLayerTreeCtrl(*this, m_Datainfo, m_layertree, m_actionStack);
		m_pLayerTreeCtrl->m_bShowhidelayer = false;
		m_pLayerTreeCtrl->Create(TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_CHECKBOXES|TVS_EDITLABELS|TVS_SHOWSELALWAYS|WS_VISIBLE|WS_CHILD, CRect(0, 0, 0, 0), (CWnd*)pLayerPane, 0);
	}

	m_pGridMapFile = nullptr;
	m_pGeomMapFile = nullptr;

	m_pDeck = nullptr;
	m_pTool = &zoominTool;
	m_oldTool = nullptr;
}

CGrfDoc::~CGrfDoc()
{
	if(m_pGridMapFile!=nullptr)
	{
		m_pGridMapFile->Close();
		delete m_pGridMapFile;
		m_pGridMapFile = nullptr;
	}
	if(m_pGeomMapFile!=nullptr)
	{
		m_pGeomMapFile->Close();
		delete m_pGeomMapFile;
		m_pGeomMapFile = nullptr;
	}

	if(m_pLayerTreeCtrl!=nullptr)
	{
		CLayerPane* pLayerPane = (CLayerPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'L', 0);
		if(pLayerPane->m_pLayerTreeCtrl==m_pLayerTreeCtrl)
		{
			pLayerPane->SetCtrl(nullptr);
		}

		m_pLayerTreeCtrl->PostMessage(WM_DESTROY, 0, 0);
		m_pLayerTreeCtrl->DestroyWindow();
		delete m_pLayerTreeCtrl;
		m_pLayerTreeCtrl = nullptr;
	}
}

void CGrfDoc::Draw(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect)
{
	d_oRectArray.RemoveAll();
	if(invalidRect.IsRectNull()==TRUE)
		return;

	for(CLayerTree::postiterator it = m_layertree.postbegin(); it!=m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(CMainFrame::m_Security.m_bViewLevel<layer->m_bViewLevel)
			continue;
		if(layer->m_bVisible==FALSE)
			continue;

		layer->Draw(g, viewinfo, invalidRect);
	}
}

void CGrfDoc::DrawTag(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect)
{
	for(CLayerTree::forwiterator it = m_layertree.forwbegin(); it!=m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		layer->DrawTag(g, viewinfo, invalidRect);
	}
}

void CGrfDoc::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();

	AfxGetApp()->BeginWaitCursor();

	if(ar.IsStoring())
	{
		m_Datainfo.Serialize(ar, dwVersion);
		ar<<m_dwMaxGeomId;
	}
	else
	{
		m_Datainfo.Serialize(ar, dwVersion);		
		ar>>m_dwMaxGeomId;
	}

	m_layertree.Serialize(ar, dwVersion);

	AfxGetApp()->EndWaitCursor();

	CDocument::Serialize(ar);

	if(ar.IsStoring())
	{
	}
	else
	{
		if(m_pLayerTreeCtrl!=nullptr)
		{
			m_pLayerTreeCtrl->BuildTree();
		}

		CString strFile = ar.m_strFileName;
		CString strGrid = ar.m_strFileName;
		CString strGeom = ar.m_strFileName;

		strGrid = strGrid.Left(strGrid.ReverseFind(_T('.')));
		strGeom = strGeom.Left(strGeom.ReverseFind(_T('.')));
		strGrid = strGrid+_T(".Grd");
		strGeom = strGeom+_T(".Geo");
		if(_taccess(strGrid, 00)!=-1||_taccess(strGeom, 00)!=-1)
		{
			m_pGridMapFile = new CGridMapFile();
			m_pGeomMapFile = new CGeomMapFile();
			if(m_pGridMapFile->Open(strGrid)==false||m_pGeomMapFile->Open(strGeom)==false)
			{
				delete m_pGridMapFile;
				m_pGridMapFile = nullptr;
				delete m_pGeomMapFile;
				m_pGeomMapFile = nullptr;
			}
		}
	}
}

#ifdef _DEBUG
void CGrfDoc::AssertValid() const
{
	COleDocument::AssertValid();
}

void CGrfDoc::Dump(CDumpContext& dc) const
{
	COleDocument::Dump(dc);
}
#endif//_DEBUG

void CGrfDoc::SetTitle(LPCTSTR lpszTitle)
{
	if(m_Datainfo.m_strName.IsEmpty()==false)
		COleDocument::SetTitle(m_Datainfo.m_strName);
	else
		COleDocument::SetTitle(lpszTitle);
}

BOOL CGrfDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CAtlasPane* pAtlasPane = (CAtlasPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'A', 0);
	if(pAtlasPane!=nullptr&&pAtlasPane->m_hWnd!=nullptr)
	{
		pAtlasPane->m_CatalogTree.SelectMap(lpszPathName);
	}

	const CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if(pMainFrame->m_pAtlas!=nullptr)
	{
		CDeck* pDeck = pMainFrame->m_pAtlas->m_Pyramid.GetDeck(lpszPathName);
		if(pDeck!=nullptr)
		{
			m_pDeck = pDeck;
		}
	}

	return COleDocument::OnOpenDocument(lpszPathName);
}

void CGrfDoc::OnViewLegend()
{
	if(m_pDeck!=nullptr)
	{
		const CString strLegend = m_pDeck->m_strLegend;
		if(_taccess(strLegend, 0)!=-1)
		{
			const HINSTANCE hInstOld = AfxGetResourceHandle();
			HINSTANCE hInst = ::LoadLibrary(_T("Atlas.dll"));
			AfxSetResourceHandle(hInst);

			CLegendDlg dlg(nullptr, strLegend);
			if(dlg.DoModal()==IDOK)
			{
			}

			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
		}
	}
}

void CGrfDoc::FindLocation(LOCATION& location, double& lng, double& lat)
{
	const CPointF geoPoint(lng, lat);
	//	unsigned long docDecimal = m_Datainfo.GetDecimal();
	const CSize docOffset = m_Datainfo.GetDocOrigin();

	POSITION pos = GetFirstViewPosition();
	if(pos!=nullptr)
	{
		CGrfView* pView = (CGrfView*)GetNextView(pos);
		const CPoint docPoint = m_Datainfo.GeoToDoc(geoPoint);
		for(CLayerTree::forwiterator it = m_layertree.forwbegin(); it!=m_layertree.forwend(); ++it)
		{
			CLayer* layer = *it;
			}
	}
}

CGeom* CGrfDoc::GetGeom(const DWORD& dwId, const CRect& inRect) const
{
	//	unsigned long docDecimal = m_Datainfo.GetDecimal();
	const CSize docOffset = m_Datainfo.GetDocOrigin();

	POSITION pos = GetFirstViewPosition();
	const CGrfView* pView = (CGrfView*)GetNextView(pos);
	for(CLayerTree::forwiterator it = m_layertree.forwbegin(); it!=m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bVisible==false)
			continue;

		if(pView->m_viewMonitor.m_levelCurrent < layer->m_minLevelObj)
			continue;
		if(pView->m_viewMonitor.m_levelCurrent >= layer->m_maxLevelObj)
			continue;

		POSITION pos2 = layer->m_geomlist.GetHeadPosition();
		while(pos2!=nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetNext(pos2);
			CRect rect = pGeom->m_Rect;
			if(Util::Rect::Intersect(rect, inRect)==false)
				continue;

			if(pGeom->PickIn(inRect)==TRUE)
				return pGeom;
		}

		if(m_pGridMapFile!=nullptr&&m_pGeomMapFile!=nullptr)
		{
			if(m_pGridMapFile->m_Head.dwWidth==0)
				continue;
			if(m_pGridMapFile->m_Head.dwHeight==0)
				continue;
			const int wLayerIndex = m_pGridMapFile->GetLayerIndex(layer->m_wId);
			if(wLayerIndex==-1)
				continue;

			int xMin = (inRect.left-docOffset.cx)/(int)(m_pGridMapFile->m_Head.dwWidth);
			int yMin = (inRect.top-docOffset.cy)/(int)(m_pGridMapFile->m_Head.dwHeight);
			int xMax = (inRect.right-docOffset.cx)/(int)(m_pGridMapFile->m_Head.dwWidth);
			int yMax = (inRect.bottom-docOffset.cy)/(int)(m_pGridMapFile->m_Head.dwHeight);

			if(xMin<0)
				xMin = 0;
			if(yMin<0)
				yMin = 0;
			if(xMax>=m_pGridMapFile->m_Head.xCount)
				xMax = m_pGridMapFile->m_Head.xCount-1;
			if(yMax>=m_pGridMapFile->m_Head.yCount)
				yMax = m_pGridMapFile->m_Head.yCount-1;

			for(short y = yMin; y<=yMax; y++)
				for(short x = xMin; x<=xMax; x++)
				{
					CGridMapFile::GRID* pLayerGrid = m_pGridMapFile->GetGrid(wLayerIndex, y, x);
					DWORD* dwGeomlist = m_pGridMapFile->GetGridGeomList(pLayerGrid);
					for(WORD index = 0; index<pLayerGrid->wGeomCount; index++)
					{
						DWORD dwGeomIndex = *dwGeomlist;
						dwGeomlist++;

						CGeomMapFile::GEOMHEAD* pGeomHead = m_pGeomMapFile->GetGeomHeadByIndex(wLayerIndex, dwGeomIndex);
						if(Util::Rect::Intersect(pGeomHead->rect, inRect)==false)
							continue;

						delete m_pGeomMapFile->m_pGet;
						m_pGeomMapFile->m_pGet = nullptr;

						CGeom* pGeom = m_pGeomMapFile->GetGeom(pGeomHead);
						if(pGeom!=nullptr)
						{
							m_pGeomMapFile->m_pGet = pGeom;
							return pGeom;
						}

						if(pGeom->PickIn(inRect)==TRUE)
						{
							m_pGeomMapFile->m_pGet = pGeom;
							return pGeom;
						}
						else
						{
							delete m_pGeomMapFile->m_pGet;
							m_pGeomMapFile->m_pGet = nullptr;
						}
					}
				}
		}
	}

	return nullptr;
}
