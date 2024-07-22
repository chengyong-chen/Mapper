// TweenyDoc.cpp : implementation of the CGeoDoc class
//

#include "stdafx.h"
#include "GisDoc.h"
#include "GeoDoc.h"
#include "GisView.h"
#include "GeoView.h"
#include "Resource.h"

#include "Global.h"

#include "../Geometry/Geom.h"

#include "../Layer/Layer.h"
#include "../Layer/Laydb.h"

#include "../Topology/Topo.h"
#include "../Topology/Topodb.h"
#include "../Topology/RoadTopodb.h"
#include "../Topology/TopoPane.h"
#include "../Topology/TopoCtrl.h"
#include "../Topology/TopodbCtrl.h"

extern float a_fJoinTolerance;
extern float a_fTopoTolerance;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGeoDoc

IMPLEMENT_DYNCREATE(CGeoDoc, CGisDoc)

BEGIN_MESSAGE_MAP(CGeoDoc, CGisDoc)
	//{{AFX_MSG_MAP(CGeoDoc)
	ON_COMMAND(ID_TOPOEDGE_CREATE, OnTopoCreateEdges)
	ON_COMMAND(ID_TOPOEDGE_REMOVE, OnTopoRemoveEdges)
	ON_COMMAND(ID_RELEASE_VIEWER, OnReleaseViewer)
	//}}AFX_MSG_MAP)
END_MESSAGE_MAP()

CGeoDoc::CGeoDoc() noexcept
	: CGisDoc()
{
	CTopoPane* pTopoPane = (CTopoPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'T', 0);
	if(pTopoPane!=nullptr)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Topology.dll"));
		AfxSetResourceHandle(hInst);

		m_pTopoCtrl = new CTopodbCtrl(*this, m_Datainfo, m_topolist, m_actionStack);
		m_pTopoCtrl->Create((CWnd*)pTopoPane);
		m_pTopoCtrl->ModifyStyleEx(0, LVS_SHOWSELALWAYS|LVS_SINGLESEL);

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}

	m_Datainfo.m_strType = "Geo";
}

CGeoDoc::~CGeoDoc()
{
}
void CGeoDoc::Serialize(CArchive& ar)
{
	DWORD dwCurrent = AfxGetCurrentArVersion();
	DWORD dwVersion = dwCurrent;
	if(ar.IsStoring())
	{
		ar << dwVersion;
	}
	else
	{
		ar >> dwVersion;
		if(dwVersion > AfxGetCurrentArVersion())
		{
			OutputDebugString(this->GetPathName());
			OutputDebugStringA("     ");
			CStringA string;
			string.Format("File version:%0X, Software version:%0X ", dwVersion, dwCurrent);
			OutputDebugStringA(string);
			AfxMessageBox(_T("Your software is too old!\nPlease install the new version."));
			this->SetModifiedFlag(TRUE);
			return;
		}
	}
	CGeoDoc::Serialize(ar, dwVersion);
}
void CGeoDoc::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CGisDoc::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
	}
	else
	{
	}
}

BOOL CGeoDoc::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo==nullptr)
	{
		if(nCode==CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return COleDocument::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			POSITION pos = GetFirstViewPosition();
			CGeoView* pView = (CGeoView*)GetNextView(pos);

			switch(nId)
			{
			case ID_TAG_CREATE:
			case ID_TAG_DELETE:
			case ID_TAG_SETUP:
			case ID_TAG_CREATEOPTIMALLY:
				pCmdUI->Enable(FALSE);
				return TRUE;
			break;
			default:
				break;
			}
			pCmdUI->m_bContinueRouting = false;
		}
		else if(nCode==CN_COMMAND)
		{
		}
	}

	return CGisDoc::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}

// CGeoDoc diagnostics
#ifdef _DEBUG
void CGeoDoc::AssertValid() const
{
	CGisDoc::AssertValid();
}

void CGeoDoc::Dump(CDumpContext& dc) const
{
	CGisDoc::Dump(dc);
}
#endif //_DEBUG

// CGeoDoc commands
void CGeoDoc::Draw(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect)
{
	static CRectF geoLastRect(0, 0, 0, 0);

	CRect rect;
	pView->GetClientRect(rect);
	rect.NormalizeRect();
	if(rect.IsRectNull())
		return;

	AfxGetApp()->BeginWaitCursor();

	CRectF mapRect = m_Datainfo.DocToMap(invalidRect);
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it!=m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		layer->Reload(pView, m_Datainfo, viewinfo, mapRect);
	}
	
	for(std::list<CTopology*>::reverse_iterator it=m_topolist.rbegin(); it != m_topolist.rend(); ++it)
	{
		CTopodb* topo=(CTopodb*)*it;
		topo->OnRectChanged(mapRect, m_Datainfo);
	}
	AfxGetApp()->EndWaitCursor();

	CGisDoc::Draw(pView, viewinfo, g, invalidRect);
}

void CGeoDoc::OnTopoCreateEdges()
{
	POSITION pos = GetFirstViewPosition();
	if(pos==nullptr)
		return;

	CGrfView* pView = (CGrfView*)GetNextView(pos);
	if(pView==nullptr)
		return;

	if(m_pTopoCtrl==nullptr)
		return;

	CTopodb* pTopology = (CTopodb*)m_pTopoCtrl->GetSelTopo();
	if(pTopology==nullptr)
		return;

	const unsigned int& tolerence = m_Datainfo.m_zoomPointToDoc*a_fTopoTolerance;
	if(typeid(pTopology) == typeid(CRoadTopodb))
	{
		const CRoadTopodb* pRoadTopodb = (CRoadTopodb*)pTopology;

		for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it!=m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			if(layer->Gettype() <= 5)
				continue;
			if(layer->Gettype() >= 10)
				continue;

			CLaydb* laydb = (CLaydb*)layer;
			if(laydb->m_geoDatasource.GetHeaderProfile().m_tableA!=pRoadTopodb->m_tablePolyData)
				continue;

			POSITION pos2 = layer->m_geomlist.GetTailPosition();
			while(pos2!=nullptr)
			{
				CGeom* pGeom = layer->m_geomlist.GetPrev(pos2);
				if(pGeom->m_bActive==false)
					continue;
				if(pGeom->m_bClosed==true)
					continue;

				if(pGeom->m_bType==1)
				{
					pTopology->InsertEdge(pView, pView->m_viewMonitor, -1, (CPoly*)pGeom, tolerence);
				}
			}
		}
	}
}

void CGeoDoc::OnReleaseViewer()
{	
	for(std::list<CTopology*>::iterator it=m_topolist.begin(); it != m_topolist.end(); ++it)
	{
		CTopology* pTopology=*it;
		
		CString strFile = GetPathName();
		const CString strTopo = strFile.Left(strFile.ReverseFind(_T('.')));
		pTopology->ExportPC(strTopo, m_Datainfo);
	}
}
