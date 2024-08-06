#include "stdafx.h"

#include <io.h>
#include <cmath>
#include <algorithm>
#include <cstdio>

#include "Global.h"

#include "GrfDoc.h"
#include "GrfView.h"
#include "MainFrm.h"
#include "MoveDlg.h"
#include "ReplaceDlg.h"
#include "PublishToWebDlg.h"
#include "Submap.h"
#include "Resource.h"
#include "PathBufferDlg.h"

#include "../Projection/Projection1.h"
#include "../Dataview/DatainfoDlg.h"
#include "../Dataview/ViewExporter.h"

#include "../Atlas/TransformDlg.h"
#include "../Atlas/Deck.h"

#include "../Style/Line.h"
#include "../Style/FillFlash.h"
#include "../Style/SpotSymbol.h"
#include "../Style/SpotFlash.h"
#include "../Style/SpotPattern.h"
#include "../Style/LineSymbol.h"
#include "../Style/FillSymbol.h"
#include "../Style/FillPattern.h"

#include "../Layer/Global.h"
#include "../Layer/LayerTreeCtrl.h"
#include "../Layer/LayerPane.h"

#include "../Tool/Global.h"
#include "../Tool/ZoomTool.h"
#include "../Tool/TopoPickTool.h"
//#include "../Tool/TopoSplitTool.h"
#include "../Tool/TopoCrossTool.h"
#include "../Tool/EroseTool.h"
#include "../Tool/PrintTool.h"

#include "../Topology/TopoPane.h"
#include "../Topology/Topo.h"
#include "../Topology/TopoCtrl.h"
#include "../Topology/TopoerCtrl.h"
#include "../Topology/RoadNodeForm.h"
#include "../Topology/RoadEdgeForm.h"
#include "../Topology/Node.h"
#include "../Topology/Edge.h"
#include "../Topology/RegionTopoer.h"
#include "../Topology/RoadTopoer.h"
#include "../Topology/Layon.h"

#include "../Geometry/Global.h"
#include "../Geometry/Geom.h"
#include "../Geometry/Group.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Bezier.h"
#include "../Geometry/PRect.h"
#include "../Geometry/Mark.h"
#include "../Geometry/Doughnut.h"
#include "../Geometry/TextPoly.h"
#include "../Geometry/Text.h"
#include "../Geometry/Clip.h"
#include "../Geometry/Mask.h"
#include "../Geometry/TagDlg.h"
#include "../Geometry/VectorTile.h"
#include "../Geometry/PolyF.h"
#include "../Port/ImportGrf.h"
#include "../Port/ExportTxt.h"
#include "../Port/ImportAI.h"
#include "../Port/ExportEps.h"
#include "../Port/ExportAI.h"
#include "../Port/ImportGdal.h"
#include "../Port/ImportSVG.h"
#include "../Port/ImportPostscript.h"

#include "../Public/PropertyPane.h"
#include "../Public/LicenceDlg.h"
#include "../Public/Function.h"
#include "../Public/TimeWatcher.h"
#include "../Public/WebService.h"
#include "../Public/ProgressDlg.h"
#include "../Public/ODBCDatabase.h"
#include "../Database/Datasource.h"
#include "../Rectify/Tin.h"
#include "../Action/Remove.h"
#include "../Action/Document/LayerTree/Layerlist/Layer.h"
#include "../Action/Document/LayerTreeCtrl.h"
#include "../Action/Document/Datainfo.h"

#include "../Pbf/writer.hpp"

#include <gdiplus.h>
#include "../Action/Modify.h"
#include "../Public/Base64.h"
#include "../Utility/Rect.hpp"
#include "../Utility/monochrome.hpp"
#include <numeric>
#include <regex>

#include <string>
#include <vector>
#include <map>

#include <boost/json.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>

#include "../Basal/UrlCoder.hpp"
#include "../../ThirdParty/tinyxml/tinyxml2/tinyxml2.h"
#include "../../ThirdParty/clipper/2.0/CPP/Clipper2Lib/include/clipper2/clipper.h"

using namespace std;
using namespace boost;
using namespace boost::property_tree;
using namespace tinyxml2;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern __declspec(dllimport) CZoomTool zoominTool;
extern __declspec(dllimport) CTopoPickTool topoPickTool;
extern __declspec(dllimport) CEroseTool eroseTool;
//extern __declspec(dllimport) CTopoSplitTool topoSplitTool;
extern __declspec(dllimport) CTopoCrossTool topoCrossTool;

extern UNIT a_UnitArray[3];
extern BYTE a_nUnitIndex;
extern float a_fJoinTolerance;
extern float a_fTopoTolerance;
extern float a_fDensity;
extern unsigned char a_nRange;

extern CString a_strLineTagtable;
extern CString a_strFillTagtable;
extern CString a_strSpotTagtable;

IMPLEMENT_DYNCREATE(CGrfDoc, COleDocument)

BEGIN_MESSAGE_MAP(CGrfDoc, COleDocument)
	//{{AFX_MSG_MAP(CGrfDoc)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_COMMAND(ID_PROCESS_WIPEOUT, OnProcessWipeout)
	ON_COMMAND(ID_PROCESS_NORMALIZE, OnProcessNormalize)
	ON_COMMAND(ID_PROCESS_CLEANDUPLICATED, OnProcessCleanDuplicated)
	ON_COMMAND(ID_PROCESS_CLEANOUTOFRANGE, OnProcessCleanOutofrange)
	ON_COMMAND(ID_PROCESS_CLEANORPHAN, OnProcessCleanOrphan)
	ON_COMMAND(ID_PROCESS_AUTOTAGGING, OnProcessAutoTagging)
	ON_COMMAND(ID_POLYGON_AUTOCOLORING, OnPolygonAutoColoring)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_EDIT_CLONE, OnEditClone)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_DUPLICATE, OnEditDuplicate)

	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_MOVE, OnEditMove)
	ON_COMMAND(ID_EDIT_TRANSFORM, OnEditTransform)
	ON_COMMAND(ID_GEOM_LOCK, OnGeomLock)
	ON_COMMAND(ID_GEOM_UNLOCK, OnGeomUnlock)
	ON_COMMAND(ID_GEOM_GROUP, OnGeomGroup)
	ON_COMMAND(ID_GEOM_UNGROUP, OnGeomUngroup)
	ON_COMMAND(ID_GEOM_COMBINE, OnGeomCombine)
	ON_COMMAND(ID_GEOM_DISBAND, OnGeomDisband)
	ON_COMMAND(ID_RECTIFY, OnRectify)
	ON_COMMAND(ID_DATUM_LOAD, OnDatumLoad)
	ON_COMMAND(ID_DATUM_SAVE, OnDatumStore)
	ON_COMMAND(ID_DATUM_CLEAR, OnDatumClear)

	ON_COMMAND_RANGE(ID_ALIGN_LEFT, ID_ALIGN_CENTER, OnAlign)

	ON_COMMAND(ID_PATH_REVERSE, OnPathReverse)
	ON_COMMAND(ID_PATH_DENSIFY, OnPathDensify)
	ON_COMMAND(ID_PATH_SIMPLIFY, OnPathSimplify)
	ON_COMMAND(ID_PATH_CLOSE, OnPathClose)
	ON_COMMAND(ID_EDIT_CLIPIN, OnEditClipIn)
	ON_COMMAND(ID_EDIT_CLIPOUT, OnEditClipOut)
	ON_COMMAND(ID_EDIT_CLIPRELEASE, OnEditClipRelease)
	ON_COMMAND(ID_EDIT_MASKRELEASE, OnEditMaskRelease)
	ON_COMMAND(ID_FILE_MAPINFO, OnFileMapinfo)
	ON_COMMAND(ID_PATH_BUFFER, OnPathBuffer)
	ON_COMMAND(ID_COVERT_BEZIERTOPOLY, OnCovertBezierToPoly)
	ON_COMMAND(ID_COVERT_POLYTOBEZIER, OnCovertPolyToBezier)

	ON_COMMAND(ID_GEOGROID_RESET, OnGeogroidReset)
	ON_COMMAND(ID_LABELOID_RESET, OnLabeloidReset)
	ON_COMMAND(ID_GEOGROID_PLANT, OnGeogroidPlant)
	ON_COMMAND(ID_LABELOID_PLANT, OnLabeloidPlant)

	ON_COMMAND_RANGE(ID_POLYGON_SCULPT1, ID_POLYGON_SCULPT5, OnPolygonSculpt)
	ON_COMMAND(ID_POINT_REMOVE, OnPointRemove)
	ON_COMMAND(ID_POINT_CORNER, OnPointCorner)
	ON_COMMAND(ID_POINT_CONNECT, OnPointConnect)
	ON_COMMAND(ID_EDIT_INSERTGPSROUTE, OnEditInsertGPSRoute)
	ON_COMMAND(ID_EDIT_INSERTSUBMAP, OnEditInsertSubmap)
	ON_COMMAND(ID_REPLACE_WITHMARK, OnReplaceWithMark)

	ON_COMMAND(ID_MAP_REIDALL, OnMapReIDAll)
	ON_COMMAND(ID_ANNO_SETUP, OnAnnoSetup)
	ON_COMMAND(ID_ANNO_CLEAR, OnAnnoClear)
	ON_COMMAND(ID_ANNO_REPLACE, OnAnnoReplace)
	ON_COMMAND(ID_ANNO_TOTEXT, OnAnnoToText)
	ON_COMMAND(ID_TAG_CREATE, OnTagCreate)
	ON_COMMAND(ID_TAG_CREATEOPTIMALLY, OnTagCreateOptimized)
	ON_COMMAND(ID_TAG_SETUP, OnTagSetup)
	ON_COMMAND(ID_TAG_DELETE, OnTagDelete)

	ON_COMMAND(ID_TOOL_TOPOPICK, OnTopoPickTool)
	ON_COMMAND(ID_TOOL_TOPOEROSE, OnTopoEroseTool)
	ON_COMMAND(ID_TOOL_TOPOSPLIT, OnTopoSplitTool)
	ON_COMMAND(ID_TOOL_TOPOCROSS, OnTopoCrossTool)
	ON_COMMAND(ID_TOPOEDGE_CREATE, OnTopoCreateEdges)
	ON_COMMAND(ID_TOPOEDGE_REMOVE, OnTopoRemoveEdges)
	ON_COMMAND(ID_TOPOEDGE_JOIN, OnTopoEdgesJoin)
	ON_COMMAND(ID_TOOL_RECTIFYDATUM, OnRectifyToolDatum)

	ON_COMMAND(ID_RELEASEWEB_VECTORPAGE, OnReleaseWEBVectorPage)
	ON_COMMAND(ID_RELEASEWEB_VECTORTILE, OnReleaseWEBVectorTile)
	ON_COMMAND(ID_RELEASEWEB_RASTERTILE, OnReleaseWEBRasterTile)
	//}}AFX_MSG_MAP

	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, COleDocument::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, COleDocument::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, COleDocument::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, COleDocument::OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST, COleDocument::OnUpdateObjectVerbMenu)
END_MESSAGE_MAP()

CGrfDoc::CGrfDoc() noexcept
	: m_actionStack(*this), m_LayerTreeCtrl(*this, m_Datainfo, m_layertree, m_actionStack), m_datumTool(m_Tin), m_dwMaxGeomId(0)
{
	EnableCompoundFile(FALSE);
	m_ClipboardFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("Diwatu Mapper Grf"));

	m_pTopoCtrl = nullptr;


	CWnd* pMainWnd = AfxGetMainWnd();
	if(pMainWnd == nullptr)
	{
	}
	else if(CLayerPane* pLayerPane = (CLayerPane*)pMainWnd->SendMessage(WM_GETPANE, 'L', 0); pLayerPane != nullptr)
	{
		m_LayerTreeCtrl.Create(WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_CHECKBOXES | TVS_EDITLABELS | TVS_SHOWSELALWAYS, CRect(0, 0, 0, 0), (CWnd*)pLayerPane, 0);
	}
	if(pMainWnd == nullptr)
	{
	}
	else if(CTopoPane* pTopoPane = (CTopoPane*)pMainWnd->SendMessage(WM_GETPANE, 'T', 0); pTopoPane != nullptr)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Topology.dll"));
		AfxSetResourceHandle(hInst);

		m_pTopoCtrl = new CTopoerCtrl(*this, m_Datainfo, m_topolist, m_layertree, m_LayerTreeCtrl, m_actionStack);
		m_pTopoCtrl->Create((CWnd*)pTopoPane);
		m_pTopoCtrl->ModifyStyleEx(0, LVS_SHOWSELALWAYS | LVS_SINGLESEL);

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
	m_Datainfo.m_strType = "Grf";

	m_pTool = &zoominTool;
	m_oldTool = nullptr;
}
CGrfDoc::~CGrfDoc()
{
	CWnd* pMainWnd = AfxGetMainWnd();
	if(pMainWnd == nullptr)
	{
	}
	else if(CLayerPane* pLayerPane = (CLayerPane*)pMainWnd->SendMessage(WM_GETPANE, 'L', 0); pLayerPane != nullptr && pLayerPane->m_pLayerTreeCtrl == &m_LayerTreeCtrl)
	{
		pLayerPane->SetCtrl((CLayerTreeCtrl*)nullptr);
	}
	if(m_LayerTreeCtrl.m_hWnd != nullptr)
	{
		m_LayerTreeCtrl.PostMessage(WM_DESTROY, 0, 0);
		m_LayerTreeCtrl.DestroyWindow();
	}
	if(pMainWnd == nullptr)
	{
	}
	else if(CTopoPane* pTopoPane = (CTopoPane*)pMainWnd->SendMessage(WM_GETPANE, 'T', 0); pTopoPane != nullptr)
	{
		pTopoPane->SetCtrl((CTopoCtrl*)nullptr);
	}
	if(m_pTopoCtrl != nullptr)
	{
		if(m_pTopoCtrl->m_hWnd != nullptr)
		{
			m_pTopoCtrl->PostMessage(WM_DESTROY, 0, 0);
			m_pTopoCtrl->DestroyWindow();
		}
		delete m_pTopoCtrl;
		m_pTopoCtrl = nullptr;
	}
	for(std::list<CTopology*>::iterator it = m_topolist.begin(); it != m_topolist.end(); it++)
	{
		CTopology* pTopology = *it;
		CPropertyPane* pPropertyPane = (CPropertyPane*)pMainWnd->SendMessage(WM_GETPANE, 'I', 0);
		if(pPropertyPane != nullptr)
		{
			CNodeForm* pNodeDlg = (CNodeForm*)pPropertyPane->GetTabWnd('N');
			CEdgeForm* pEdgeDlg = (CEdgeForm*)pPropertyPane->GetTabWnd('E');
			if(pNodeDlg != nullptr && pNodeDlg->m_pTopo == pTopology)
			{
				pNodeDlg->SetNode(nullptr, nullptr);
			}
			if(pEdgeDlg != nullptr && pEdgeDlg->m_pTopo == pTopology)
			{
				pEdgeDlg->SetEdge(nullptr, nullptr);
			}
		}

		delete pTopology;
		pTopology = nullptr;
	}
}

BOOL CGrfDoc::OnNewDocument()
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Dataview.dll"));
	AfxSetResourceHandle(hInst);

	CDatainfoDlg dlg(nullptr, m_Datainfo);
	if(dlg.DoModal() == IDOK)
	{
		m_layertree.Initialize();
		m_LayerTreeCtrl.BuildTree();

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
	else
	{
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return FALSE;
	}

	return COleDocument::OnNewDocument();
}

void CGrfDoc::PreCloseFrame(CFrameWnd* pFrameArg)
{
	m_actionStack.Clear();
	COleDocument::PreCloseFrame(pFrameArg);
}
unsigned int CGrfDoc::TotalActivated()
{
	unsigned int total = 0;
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		const CLayer* layer = *it;
		total += layer->m_nActiveCount;
	}
	return total;
}
CGeom* CGrfDoc::GetTheActivatedGeom(CLayer*& pLayer)
{
	CGeom* pActive = nullptr;
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;
		if(layer->m_nActiveCount > 1)
			return nullptr;
		else if(pActive != nullptr)
			return nullptr;
		else {
			std::list<CGeom*> list = layer->GetActivatedGeoms();
			if(list.size() == 1)
			{
				pLayer = layer;
				pActive = list.front();
			}
			else
				break;
		}
	}

	return pActive;
}
void CGrfDoc::Draw(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect)
{
	if(invalidRect.IsRectNull())
		return;

	AfxGetApp()->BeginWaitCursor();
	if(pView != nullptr)
	{
		m_frame.Draw1(g, viewinfo);
	}
#ifdef DEBUG
	CTimeWatcher watcher;
	watcher.Start();
#endif	
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		layer->Draw(g, viewinfo, invalidRect);
	}
#ifdef DEBUG
	watcher.Stop();
	CString message;
	message.Format(_T("CGrfDoc::Draw: %g seconds\n"), watcher.GetElapsed());
	OutputDebugString(message);
#endif

	for(std::list<CTopology*>::reverse_iterator it = m_topolist.rbegin(); it != m_topolist.rend(); it++)
	{
		CTopology* pTopology = *it;
		pTopology->Draw(g, viewinfo, invalidRect);
	}

	m_Tin.Draw(g, viewinfo);
	if(pView != nullptr)
	{
		m_frame.Draw2(g, viewinfo);
	}
	AfxGetApp()->EndWaitCursor();
}

void CGrfDoc::DrawTag(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect)
{
	if(this->preference.ShowCreatedTags || this->preference.ShowDynamicTags)
	{
		for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
		{
			CLayer* layer = *it;
			layer->DrawTag(g, viewinfo, invalidRect, this->preference.ShowCreatedTags, this->preference.ShowDynamicTags);
		}
	}
}

void CGrfDoc::Capture(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect)
{
	if(invalidRect.IsRectNull())
		return;

	AfxGetApp()->BeginWaitCursor();

	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bVisible == false)
			continue;

		layer->Capture(g, viewinfo, invalidRect);
	}

	AfxGetApp()->EndWaitCursor();
}
void CGrfDoc::Serialize(CArchive& ar)
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
	CGrfDoc::Serialize(ar, dwVersion);
}
void CGrfDoc::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		m_Datainfo.Serialize(ar, dwVersion);
		ar << m_dwMaxGeomId;
		ar << m_argbBackground;
		m_frame.Serialize(ar, dwVersion);
		ar.Write(&preference, sizeof(Preference));
	}
	else
	{
		m_Datainfo.Serialize(ar, dwVersion);
		ar >> m_dwMaxGeomId;
		ar >> m_argbBackground;
		m_frame.Serialize(ar, dwVersion);
		ar.Read(&preference, sizeof(Preference));
	}

	AfxGetApp()->BeginWaitCursor();
	m_layertree.Serialize(ar, dwVersion);
	AfxGetApp()->EndWaitCursor();

	CString strTopFile = ar.m_strFileName;
	strTopFile = strTopFile.Left(strTopFile.GetLength() - 4);
	strTopFile += _T(".top");
	if(ar.IsStoring())
	{
		int size = m_topolist.size();
		if(size > 0)
		{
			CFile file;
			if(file.Open(strTopFile, CFile::modeCreate | CFile::modeWrite) == TRUE)
			{
				CArchive ar(&file, CArchive::store);
				ar << dwVersion;
				ar << size;
				for(std::list<CTopology*>::reverse_iterator it = m_topolist.rbegin(); it != m_topolist.rend(); it++)
				{
					CTopology* pTopology = *it;
					const BYTE bType = pTopology->Gettype();
					ar << bType;
					switch(bType)
					{
						case 3:
							int count = ((CRegionTopoer*)pTopology)->m_layonlist.size();
							ar << count;
							break;
					}
					pTopology->Serialize(ar, dwVersion);
				}
				ar.Close();
				file.Close();
			}
		}
		else
		{
			_wremove(strTopFile.GetString());
		}
	}
	else
	{
		for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			Replace(layer->m_geomlist);
		}

		if(PathFileExists(strTopFile))
		{
			CFile file;
			if(file.Open(strTopFile, CFile::modeRead | CFile::shareDenyWrite) == TRUE)
			{
				CArchive ar(&file, CArchive::load);
				DWORD dwVersion;
				ar >> dwVersion;
				int count1;
				ar >> count1;
				for(int index1 = 0; index1 < count1; index1++)
				{
					BYTE bType;
					ar >> bType;
					switch(bType)
					{
						case 1:
							{
								CRoadTopoer* pTopology = new CRoadTopoer(m_layertree);
								pTopology->Serialize(ar, dwVersion);
							}
							break;
						case 3:
							int count2;
							ar >> count2;
							std::list<CLayon*> layonlist;
							for(int index2 = 0; index2 < count2; index2++)
							{
								CLayon* pLayon = new CLayon(m_layertree);
								layonlist.push_back(pLayon);
							}
							CRegionTopoer* pTopology = new CRegionTopoer(m_layertree, layonlist);
							for(auto it = layonlist.begin(); it != layonlist.end(); it++)
							{
								(*it)->m_pRegionTopoer = (CRegionTopoer*)pTopology;
							}
							pTopology->Serialize(ar, dwVersion);
							m_topolist.push_back(pTopology);
							for(auto it = layonlist.begin(); it != layonlist.end(); it++)
							{
								m_layertree.Replace((*it)->m_wId, (*it));
							}
							break;
					}
				}
				ar.Close();
				file.Close();
			}
		}
		if(m_pTopoCtrl != nullptr)
		{
			m_pTopoCtrl->Built();
		}
		if(dwVersion <= 0)
		{
			for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
			{
				CLayer* layer = *it;
				double maxLevelObj = m_Datainfo.GetLevelFromScale(layer->m_maxLevelObj);
				double minLevelObj = m_Datainfo.GetLevelFromScale(layer->m_minLevelObj);
				double maxLevelTag = m_Datainfo.GetLevelFromScale(layer->m_maxLevelTag);
				double minLevelTag = m_Datainfo.GetLevelFromScale(layer->m_minLevelTag);
				layer->m_minLevelObj = min(25, max(0, minLevelObj));
				layer->m_maxLevelObj = min(25, max(0, maxLevelObj));
				layer->m_minLevelTag = min(25, max(0, minLevelTag));
				layer->m_maxLevelTag = min(25, max(0, maxLevelTag));
			}
		}
		m_LayerTreeCtrl.BuildTree();
	}
}

void CGrfDoc::ReleaseCE(CArchive& ar) const
{
	m_Datainfo.ReleaseCE(ar);

	m_layertree.ReleaseCE(ar);
	if(ar.IsStoring())
	{
	}
	else
	{
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
#endif //_DEBUG

void CGrfDoc::OnGeomLock()
{
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->Lock(m_actionStack);
	}
	SetModifiedFlag(TRUE);
}

void CGrfDoc::OnGeomUnlock()
{
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->UnLock(m_actionStack);
	}
	SetModifiedFlag(TRUE);
}

void CGrfDoc::OnEditClear()
{
	POSITION pos = GetFirstViewPosition();
	if(pos == nullptr)
		return;

	CGrfView* pView = (CGrfView*)GetNextView(pos);
	if(pView == nullptr)
		return;

	if(pView->m_editgeom.Clear(pView) == TRUE)
	{
	}
	else
	{
		AfxGetApp()->BeginWaitCursor();
		m_actionStack.Start();

		for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			if(layer->m_nActiveCount == 0)
				continue;

			layer->Clear(pView, pView->m_viewMonitor, m_actionStack);

			SetModifiedFlag(TRUE);
		}

		m_actionStack.Stop();
		AfxGetApp()->EndWaitCursor();
	}
}

void CGrfDoc::OnEditClone()
{
	POSITION pos = GetFirstViewPosition();
	if(pos == nullptr)
		return;

	CGrfView* pView = (CGrfView*)GetNextView(pos);
	if(pView == nullptr)
		return;

	AfxGetApp()->BeginWaitCursor();
	m_actionStack.Start();
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->Duplicate(pView, m_Datainfo, pView->m_viewMonitor, CSize(0, 0), m_actionStack);
	}
	m_actionStack.Stop();
	AfxGetApp()->EndWaitCursor();
	SetModifiedFlag(TRUE);
}

void CGrfDoc::OnProcessCleanDuplicated()
{
	AfxGetApp()->BeginWaitCursor();
	m_actionStack.Start();
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bVisible == false)
			continue;

		m_actionStack.Start();
		POSITION pos1 = layer->m_geomlist.GetTailPosition();
		while(pos1 != nullptr)
		{
			const CGeom* pGeom1 = layer->m_geomlist.GetAt(pos1);

			POSITION pos2 = pos1;
			POSITION pos3 = pos1;
			layer->m_geomlist.GetPrev(pos2);
			while((pos3 = pos2) != nullptr)
			{
				CGeom* pGeom2 = layer->m_geomlist.GetPrev(pos2);
				if(*pGeom2 == *pGeom1)
				{
					int index = GetIndexFromOBList(layer->m_geomlist, pGeom2);
					std::list<std::pair<CGeom*, unsigned int>> geom;
					geom.push_back(std::pair<CGeom*, unsigned int>(pGeom2, index));
					Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pRemove = new Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(nullptr, layer, geom);
					pRemove->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
					pRemove->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
					pRemove->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_AppendRemove_Geoms;

					POSITION pos = GetFirstViewPosition();
					if(pos != nullptr)
					{
						const CGrfView* pView = (CGrfView*)GetNextView(pos);
						pView->SendMessage(WM_PREREMOVEGEOM, (LONG)layer, (UINT)pGeom2);
					}
					layer->m_geomlist.RemoveAt(pos3);
					layer->SetModifiedFlag(true);
				}
			}

			layer->m_geomlist.GetPrev(pos1);
		}
		m_actionStack.Stop();
	}
	m_actionStack.Stop();
	AfxGetApp()->EndWaitCursor();

	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr);
}

void CGrfDoc::OnProcessCleanOutofrange()
{
	AfxGetApp()->BeginWaitCursor();
	m_actionStack.Start();
	const CRect rectMap = m_Datainfo.GetDocRect();
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		std::list<std::pair<CGeom*, unsigned int>> geoms;
		POSITION pos2 = layer->m_geomlist.GetHeadPosition();
		POSITION pos3;
		while((pos3 = pos2) != nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetNext(pos2);
			if(Util::Rect::Intersect(pGeom->m_Rect, rectMap) == false)
			{
				int index = GetIndexFromOBList(layer->m_geomlist, pGeom);
				geoms.push_back(std::pair<CGeom*, unsigned int>(pGeom, index));

				POSITION pos4 = GetFirstViewPosition();
				if(pos4 != nullptr)
				{
					const CGrfView* pView = (CGrfView*)GetNextView(pos4);
					pView->SendMessage(WM_PREREMOVEGEOM, (LONG)layer, (UINT)pGeom);
				}
				layer->m_geomlist.RemoveAt(pos3);
				layer->SetModifiedFlag(true);
				this->SetModifiedFlag(TRUE);
			}
		}

		if(geoms.size() > 0)
		{
			Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pRemove = new Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(nullptr, layer, geoms);
			pRemove->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
			pRemove->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
			pRemove->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_AppendRemove_Geoms;
			m_actionStack.Record(pRemove);
		}
	}
	m_actionStack.Stop();
	AfxGetApp()->EndWaitCursor();
	UpdateAllViews(nullptr);
}
void CGrfDoc::OnProcessCleanOrphan()
{
	if(AfxMessageBox(IDS_LOSINGUNDOS, MB_YESNO) == IDYES)
	{
		AfxGetApp()->BeginWaitCursor();

		for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
		{
			CLayer* layer = *it;
			layer->CleanOrphan();
		}

		AfxGetApp()->EndWaitCursor();
		UpdateAllViews(nullptr);
	}
}
void CGrfDoc::OnEditCut()
{
	OnEditCopy();
	OnEditClear();
}

void CGrfDoc::OnEditCopy()
{
	POSITION pos = GetFirstViewPosition();
	if(pos == nullptr)
		return;
	CGrfView* pView = (CGrfView*)GetNextView(pos);
	if(pView == nullptr)
		return;

	ASSERT_VALID(this);
	ASSERT(m_ClipboardFormat != 0);

	AfxGetApp()->BeginWaitCursor();
	CSharedFile file;
	CArchive ar(&file, CArchive::store);

	CLayerTree layertree;
	auto attDbCopied = [&](CString dbname)
	{
		if(layertree.m_AttDBList.find(dbname) == layertree.m_AttDBList.end())
		{
			CODBCDatabase* pODBCDatabase = this->m_layertree.GetAttDatabase(dbname);
			if(pODBCDatabase != nullptr)
			{
				layertree.m_AttDBList[dbname] = pODBCDatabase->Clone();
			}
		}
	};

	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		//if(layer->Copyable() == true)
		if(layer->m_nActiveCount > 0)
		{
			CLayer* mylayer = new CLayer(layertree);
			if(layer->CopyGeomTo(mylayer, attDbCopied) == true)
			{
				layertree.m_root.AddLastChild(mylayer);
			}
			else
			{
				delete mylayer;
				mylayer = nullptr;
			}
		}
	}

	const CRect* rect = (CRect*)(pView->SendMessage(WM_GETACTIVERECT, 0, 0));
	CPoint objcenter = rect->CenterPoint();
	objcenter.x = rect->left + rect->Width() / 2;//µ±zoomRectµÄÖµ¹ý´óÊ±£¬CenterPoint£¨£©º¯ÊýÖ´ÐÐÓÐÎó
	objcenter.y = rect->top + rect->Height() / 2;
	const DWORD dwVersion = AfxGetCurrentArVersion();
	ar << dwVersion;
	ar << objcenter;
	m_Datainfo.Serialize(ar, dwVersion);
	layertree.Serialize(ar, dwVersion);
	ar.Close();
	for(CTree<CLayer>::postiterator it = layertree.postbegin(); it != layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer != nullptr)
		{
			layer->m_pSpot = nullptr;
			layer->m_pLine = nullptr;
			layer->m_pFill = nullptr;
			layer->m_pType = nullptr;
			layer->DisableAttribute();
			layer->m_geomlist.RemoveAll();
		}
	}

	COleDataSource* pDataSource = nullptr;
	try
	{
		pDataSource = new COleDataSource;
		pDataSource->CacheGlobalData(m_ClipboardFormat, file.Detach());
		pDataSource->SetClipboard();
	}
	catch(CException* ex)
	{
		//			OutputDebugString(ex->m_strError);
		delete pDataSource;
		ex->Delete();
	}

	AfxGetApp()->EndWaitCursor();
}

void CGrfDoc::OnEditPaste()
{
	if(m_ClipboardFormat != (CLIPFORMAT)::RegisterClipboardFormat(_T("Diwatu Mapper Grf")) && m_ClipboardFormat != (CLIPFORMAT)::RegisterClipboardFormat(_T("Diwatu Mapper Gis")))
		return;

	POSITION pos = GetFirstViewPosition();
	if(pos == nullptr)
		return;

	CGrfView* pView = (CGrfView*)GetNextView(pos);
	if(pView == nullptr)
		return;

	AfxGetApp()->BeginWaitCursor();

	pView->InactivateAll();
	COleDataObject dataObject;
	dataObject.AttachClipboard();

	if(dataObject.IsDataAvailable(m_ClipboardFormat))
	{
		CFile* pFile = dataObject.GetFileData(m_ClipboardFormat);
		if(pFile == nullptr)
			return;

		CArchive ar(pFile, CArchive::load);
		try
		{
			DWORD dwVersion;
			ar >> dwVersion;
			CPoint objcenter;
			ar >> objcenter;

			CDatainfo mapinfo;
			CLayerTree layertree;
			mapinfo.Serialize(ar, dwVersion);
			layertree.Serialize(ar, dwVersion);

			CSize Δ;
			if(::GetKeyState(VK_SHIFT) >= 0)
			{
				CRect rect;
				pView->GetClientRect(rect);
				CPoint clientcenter;
				clientcenter = rect.CenterPoint();
				clientcenter = pView->m_viewMonitor.ClientToDoc(clientcenter, false);
				Δ = clientcenter - objcenter;
			}
			else
			{
				Δ = CSize(0, 0);
			}

			std::list<CLayer*> layers;
			for(CTree<CLayer>::forwiterator it = layertree.forwbegin(); it != layertree.forwend(); ++it)
			{
				CLayer* layer = *it;
				POSITION pos2 = layer->m_geomlist.GetTailPosition();
				while(pos2 != nullptr)
				{
					CGeom* pGeom = layer->m_geomlist.GetPrev(pos2);
					pGeom->m_geoId = ++m_dwMaxGeomId;
					pGeom->Move(Δ);
					pGeom->m_bActive = true;
				}
				layer->m_nActiveCount = layer->m_geomlist.GetCount();
				layers.push_back(layer);
			}

			CGrfDoc::Paste(layertree);

			ar.Close();
			delete pFile;
		}
		catch(CException* ex)
		{
			//				OutputDebugString(ex->m_strError);
			ar.Close();
			delete pFile;
			ex->Delete();
		}
	}
	SetModifiedFlag(TRUE);

	AfxGetApp()->EndWaitCursor();
}

void CGrfDoc::Paste(CLayerTree& layertree)
{
	m_actionStack.Start();
	std::list<CLayer*> layers;
	for(CTree<CLayer>::forwiterator it = layertree.forwbegin(); it != layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer != &layertree.m_root)
		{
			layers.push_back(layer);
		}
	}
	layertree.m_root.NoChild();

	while(layers.empty() == false)
	{
		CLayer* flayer = layers.back();
		layers.pop_back();

		CLayer* tlayer = m_layertree.GetByName(flayer->m_strName);
		if(tlayer != nullptr)
		{
			std::list<std::pair<CGeom*, unsigned int>> geoms;
			POSITION pos = flayer->m_geomlist.GetTailPosition();
			while(pos != nullptr)
			{
				CGeom* pGeom = flayer->m_geomlist.GetPrev(pos);
				tlayer->m_geomlist.AddTail(pGeom);
				pGeom->m_bActive = true;
				tlayer->Invalidate(*this, pGeom);
				tlayer->m_nActiveCount++;
				if(pGeom->IsKindOf(RUNTIME_CLASS(CMark)) == TRUE)
				{
					((CMark*)pGeom)->CalCorner(tlayer->GetSpot(), m_Datainfo, m_layertree.m_library);
				}
				else if(pGeom->IsKindOf(RUNTIME_CLASS(CText)) == TRUE)
				{
					((CText*)pGeom)->CalCorner(tlayer->m_pType, m_Datainfo.m_zoomPointToDoc, 1.f);
				}

				geoms.push_back(std::pair<CGeom*, unsigned int>(pGeom, tlayer->m_geomlist.GetCount() - 1));
			}
			tlayer->SetModifiedFlag(true);
			if(geoms.size() > 0)
			{
				Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pAppend = new Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(nullptr, tlayer, geoms);
				pAppend->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
				pAppend->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
				pAppend->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_AppendRemove_Geoms;
				m_actionStack.Record(pAppend);
			}
			flayer->m_geomlist.RemoveAll();
			delete flayer;
			flayer = nullptr;
		}
		else
		{
			flayer->m_wId = m_layertree.ApplyId();
			TV_INSERTSTRUCT tvstruct;
			tvstruct.hParent = m_LayerTreeCtrl.GetItem((DWORD)&m_layertree.m_root);
			tvstruct.hInsertAfter = TVI_FIRST;
			tvstruct.item.mask = TVIF_TEXT | TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_STATE | TVIF_PARAM;
			tvstruct.item.stateMask = TVIS_STATEIMAGEMASK;
			tvstruct.item.iImage = flayer->Gettype();
			tvstruct.item.iSelectedImage = tvstruct.item.iImage;

			tvstruct.item.pszText = flayer->m_strName.GetBuffer(10);
			tvstruct.item.state = INDEXTOSTATEIMAGEMASK(flayer->m_bVisible + 1);
			tvstruct.item.lParam = (DWORD)flayer;
			HTREEITEM hItem = m_LayerTreeCtrl.InsertItem(&tvstruct);
			flayer->m_strName.ReleaseBuffer();

			m_layertree.m_library.m_libLineSymbol.Paste < CLineSymbol >(layertree.m_library.m_libLineSymbol, flayer->m_pLine);
			m_layertree.m_library.m_libFillSymbol.Paste < CFillSymbol >(layertree.m_library.m_libFillSymbol, flayer->m_pFill);
			m_layertree.m_library.m_libFillPattern.Paste < CFillPattern >(layertree.m_library.m_libFillPattern, flayer->m_pFill);
			m_layertree.m_library.m_libFillFlash.Paste < CFillFlash >(layertree.m_library.m_libFillFlash, flayer->m_pFill);
			m_layertree.m_library.m_libSpotSymbol.Paste < CSpotSymbol >(layertree.m_library.m_libSpotSymbol, flayer->m_pSpot);
			m_layertree.m_library.m_libSpotPattern.Paste < CSpotPattern >(layertree.m_library.m_libSpotPattern, flayer->m_pSpot);
			m_layertree.m_library.m_libSpotFlash.Paste < CSpotFlash >(layertree.m_library.m_libSpotFlash, flayer->m_pSpot);
			m_layertree.m_root.AddFirstChild(flayer);

			std::pair<std::pair<CLayer*, CLayer*>, CLayer*> object(std::pair<CLayer*, CLayer*>(&m_layertree.m_root, (CLayer*)nullptr), flayer);
			Undoredo::CAppend<CLayerTreeCtrl&, CLayerTree&, std::pair<std::pair<CLayer*, CLayer*>, CLayer*>>* pAppend = new Undoredo::CAppend<CLayerTreeCtrl&, CLayerTree&, std::pair<std::pair<CLayer*, CLayer*>, CLayer*>>(m_LayerTreeCtrl, m_layertree, object);
			pAppend->undo = Undoredo::Document::LayerTreeCtrl::Layerlist::Undo_Append_Layer;
			pAppend->redo = Undoredo::Document::LayerTreeCtrl::Layerlist::Redo_Append_Layer;
			pAppend->free = Undoredo::Document::LayerTreeCtrl::Layerlist::Free_Append_Layer;
			m_actionStack.Record(pAppend);

			CATTDatasource* pATTDatasource = flayer->GetAttDatasource();
			if(pATTDatasource != nullptr)
			{
				CODBCDatabase* fODBCDatabase = layertree.GetAttDatabase(pATTDatasource->m_strDatabase);
				CODBCDatabase* tODBCDatabase = this->m_layertree.GetAttDatabase(pATTDatasource->m_strDatabase);
				if(fODBCDatabase != nullptr && tODBCDatabase == nullptr)
				{
					this->m_layertree.m_AttDBList[pATTDatasource->m_strDatabase] = fODBCDatabase;
					layertree.m_AttDBList.erase(pATTDatasource->m_strDatabase);
				}
			}
		}
	}
	m_actionStack.Stop();
}

void CGrfDoc::OnEditDuplicate()
{
	POSITION pos1 = GetFirstViewPosition();
	if(pos1 == nullptr)
		return;

	CGrfView* pView = (CGrfView*)GetNextView(pos1);
	if(pView == nullptr)
		return;
	const CPoint point1 = pView->m_viewMonitor.ClientToDoc(CPoint(0, 0), false);
	const CPoint point2 = pView->m_viewMonitor.ClientToDoc(CPoint(50, 50), false);
	const CSize Δ(point2.x - point1.x, point2.y - point1.y);

	AfxGetApp()->BeginWaitCursor();
	m_actionStack.Start();
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->Duplicate(pView, m_Datainfo, pView->m_viewMonitor, Δ, m_actionStack);
	}
	SetModifiedFlag(TRUE);

	m_actionStack.Stop();
	AfxGetApp()->EndWaitCursor();
}
void CGrfDoc::OnMapReIDAll()
{
	m_dwMaxGeomId = 0;
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		POSITION pos2 = layer->m_geomlist.GetHeadPosition();
		while(pos2 != nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetNext(pos2);
			pGeom->m_geoId = ++m_dwMaxGeomId;
		}
	}
}
CPath* CGrfDoc::GetUniqueClosedPath()
{
	CPath* pPath = nullptr;
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;
		if(layer->m_nActiveCount > 1)
			return nullptr;
		else if(pPath != nullptr)
			return nullptr;

		POSITION pos = layer->m_geomlist.GetTailPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetPrev(pos);
			if(pGeom->m_bActive == false)
				continue;
			else if(pGeom->IsKindOf(RUNTIME_CLASS(CPath)) == FALSE)
				return nullptr;
			else if(pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly)) == TRUE)
				return nullptr;
			else if(pGeom->m_bClosed == false)
				return nullptr;
			else
			{
				pPath = (CPath*)pGeom;
				break;
			}
		}
	}

	return pPath;
}
void CGrfDoc::OnEditClipIn()
{
	CPath* pPath = GetUniqueClosedPath();
	if(pPath == nullptr)
	{
		AfxMessageBox(IDS_NEEDSINGLECLOSEDPATH);
	}
	else if(AfxMessageBox(IDS_LOSINGUNDOS, MB_YESNO) == IDYES)
	{
		m_actionStack.Clear();
		POSITION pos = GetFirstViewPosition();
		CGrfView* pView = (CGrfView*)GetNextView(pos);

		AfxGetApp()->BeginWaitCursor();
		//	m_actionStack.Start();
		for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			layer->Clip(pView, pPath, true, m_Datainfo.m_zoomPointToDoc, m_actionStack);
		}
		//	m_actionStack.Stop();
		AfxGetApp()->EndWaitCursor();

		SetModifiedFlag(TRUE);
		UpdateAllViews(nullptr);
	}
}

void CGrfDoc::OnEditClipOut()
{
	CPath* pPath = GetUniqueClosedPath();
	if(pPath == nullptr)
	{
		AfxMessageBox(IDS_NEEDSINGLECLOSEDPATH);
	}
	else
	{
		POSITION pos = GetFirstViewPosition();
		CGrfView* pView = (CGrfView*)GetNextView(pos);

		AfxGetApp()->BeginWaitCursor();
		m_actionStack.Start();
		for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			layer->Clip(pView, pPath, false, m_Datainfo.m_zoomPointToDoc, m_actionStack);
		}
		m_actionStack.Stop();
		AfxGetApp()->EndWaitCursor();

		SetModifiedFlag(TRUE);
		UpdateAllViews(nullptr);
	}
}

void CGrfDoc::OnEditClipRelease()
{
	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);

	m_actionStack.Start();
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->Letitgo(pView, pView->m_viewMonitor, m_actionStack);
	}
	m_actionStack.Stop();
	SetModifiedFlag(TRUE);
}
void CGrfDoc::OnEditMaskRelease()
{
	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);

	m_actionStack.Start();
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->Letitgo(pView, pView->m_viewMonitor, m_actionStack);
	}
	m_actionStack.Stop();
	SetModifiedFlag(TRUE);
}
void CGrfDoc::OnEditMove()
{
	POSITION pos = GetFirstViewPosition();
	if(pos == nullptr)
		return;

	CView* pView = GetNextView(pos);
	if(pView == nullptr)
		return;

	CString unit;
	if(m_Datainfo.m_pProjection == nullptr)
		unit = _T("Points");
	else
		unit = m_Datainfo.m_pProjection->m_strUnit;
	CMoveDlg dlg(nullptr, unit);
	if(dlg.DoModal() == IDOK)
	{
		CSize Δ = m_Datainfo.MapToDoc(CSizeF(dlg.m_fCx, dlg.m_fCy));
		pView->SendMessage(WM_MOVEGEOM, 0, (LONG)&Δ);
	}
	SetModifiedFlag(TRUE);
}

void CGrfDoc::OnRectify()
{
	m_actionStack.Start();
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->Rectify(m_Tin);
	}

	m_actionStack.Stop();
	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr);
}

void CGrfDoc::OnEditTransform()
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Atlas.dll"));
	AfxSetResourceHandle(hInst);

	CTransformDlg dlg(nullptr);
	if(dlg.DoModal() == IDOK)
	{
		const double m11 = dlg.m_a11;
		const double m12 = dlg.m_a12;
		const double m21 = dlg.m_a21;
		const double m22 = dlg.m_a22;
		const double m31 = dlg.m_a31;
		const double m32 = dlg.m_a32;

		double a[6];
		a[0] = m11, a[1] = m21, a[2] = m31, a[3] = m12, a[4] = m22, a[5] = m32;

		POSITION pos = GetFirstViewPosition();
		const CView* pView = GetNextView(pos);
		pView->SendMessage(WM_TRANSFORMED, (LONG)a, 0);
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CGrfDoc::OnDatumLoad()
{
	m_Tin.Load();

	SetModifiedFlag(TRUE);
	UpdateAllViews(nullptr);
}

void CGrfDoc::OnDatumStore()
{
	m_Tin.Store();
}

void CGrfDoc::OnDatumClear()
{
	m_Tin.Clear();
	UpdateAllViews(nullptr);
}

void CGrfDoc::OnGeomGroup()
{
	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);

	m_actionStack.Start();
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount < 2)
			continue;

		layer->Group(pView, pView->m_viewMonitor, m_actionStack);
	}
	m_actionStack.Stop();
	SetModifiedFlag(TRUE);
}

void CGrfDoc::OnGeomUngroup()
{
	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);

	m_actionStack.Start();
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->UnGroup(pView, pView->m_viewMonitor, m_actionStack);
	}
	m_actionStack.Stop();
	SetModifiedFlag(TRUE);
}

void CGrfDoc::OnGeomCombine()
{
	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);

	m_actionStack.Start();
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->Combine(pView, m_Datainfo, pView->m_viewMonitor, m_actionStack);
	}
	m_actionStack.Stop();
	SetModifiedFlag(TRUE);
	//  CCommand* command = new CCommand("None");
	//	this->SendMessage(WM_SUBMITACTION,0,(LONG)command);
}

void CGrfDoc::OnGeomDisband()
{
	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);

	m_actionStack.Start();
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->Disband(pView, pView->m_viewMonitor, m_actionStack);
	}
	m_actionStack.Stop();
	SetModifiedFlag(TRUE);
	//  CCommand* command = new CCommand("None");
	//	this->SendMessage(WM_SUBMITACTION,0,(LONG)command);
}

void CGrfDoc::OnAlign(UINT nId)
{
	const ALIGN align = ALIGN(nId - ID_ALIGN_LEFT);
	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);

	const CRect* rect = (CRect*)(pView->SendMessage(WM_GETACTIVERECT, 0, 0));

	m_actionStack.Start();
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->Align(pView, pView->m_viewMonitor, m_actionStack, rect, align);
	}
	m_actionStack.Stop();
	SetModifiedFlag(TRUE);
}

void CGrfDoc::OnEditUndo()
{
	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);
	pView->m_editgeom.UnFocusAll(pView);
	m_actionStack.Undo();
}

void CGrfDoc::OnEditRedo()
{
	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);
	pView->m_editgeom.UnFocusAll(pView);
	m_actionStack.Redo();
}

void CGrfDoc::OnEditInsertGPSRoute()
{
	TCHAR CurrentDir[256];
	GetCurrentDirectory(255, CurrentDir);

	CString strPath = AfxGetApp()->GetProfileString(_T(""), _T("Gps Route Path"), nullptr) + _T("*.Grf");
	CFileDialog dlg(TRUE,
		_T("*.Grf"),
		strPath,
		OFN_HIDEREADONLY,
		_T("GPS Route File (*.Grf)|*.Grf||"),
		AfxGetMainWnd());
	dlg.m_ofn.lpstrTitle = _T("Insert GPS Route");
	if(dlg.DoModal() != IDOK)
	{
		SetCurrentDirectory(CurrentDir);
		return;
	}
	CString strFile = dlg.GetPathName();
	CString strExt = dlg.GetFileExt();
	strExt.MakeUpper();

	if(strExt == _T("GRF"))
	{
		POSITION pos = this->GetFirstViewPosition();
		CGrfView* pView = (CGrfView*)this->GetNextView(pos);
		if(CLayer* layer = pView->GetActiveLayer(); layer != nullptr)
		{
			CImportGrf importer(m_Datainfo, m_layertree, m_dwMaxGeomId);
			importer.Import(strFile, layer);
		}
	}
}
void CGrfDoc::OnEditInsertSubmap()
{
	const CWinApp* pApp = AfxGetApp();
	CString strFile;
	if(pApp->m_pDocManager->DoPromptFileName(strFile, AFX_IDS_OPENFILE, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT, TRUE, nullptr) == FALSE)
		return;
	else if(strFile.CompareNoCase(this->m_strPathName) == 0)
		AfxMessageBox(_T("You can't insert this map as a submap to itself!"));
	else if(CDocTemplate* pTemplate = pApp->m_pDocManager->GetBestTemplate(strFile); pTemplate != nullptr)
	{
		CDocument* pDocument = pTemplate->CreateNewDocument();
		if(pDocument->OnOpenDocument(strFile) == TRUE)
		{
			if(((CGrfDoc*)pDocument)->m_Datainfo.m_dilationMapToDoc == this->m_Datainfo.m_dilationMapToDoc)
			{
				POSITION pos = GetFirstViewPosition();
				if(CGrfView* pView = (CGrfView*)GetNextView(pos); pView != nullptr)
				{
					CSubmap* pGeom = new CSubmap(strFile, (CGrfDoc*)pDocument);
					pView->SendMessage(WM_NEWGEOMDREW, (UINT)pGeom, 0);
				}
				else
				{
					delete pDocument;
					pDocument = nullptr;
				}
			}
			else
			{
				delete pDocument;
				pDocument = nullptr;
			}
		}
		else
		{
			delete pDocument;
			pDocument = nullptr;
		}
	}
}
void CGrfDoc::OnReplaceWithMark()
{
	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);

	m_actionStack.Start();
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->ReplaceWithMark(pView, pView->m_viewMonitor, m_actionStack);
	}
	m_actionStack.Stop();
	SetModifiedFlag(TRUE);
}
BOOL CGrfDoc::Import(CString strFile)
{
	CImport* pImport = nullptr;
	const CString strExt = strFile.Mid(strFile.ReverseFind(_T('.')) + 1);
	if(strExt.CompareNoCase(_T("pdf")) == 0)
		pImport = new CImportPostscript(m_Datainfo, m_layertree, m_dwMaxGeomId);
	else if(strExt.CompareNoCase(_T("eps")) == 0)
		pImport = new CImportPostscript(m_Datainfo, m_layertree, m_dwMaxGeomId);
	else if(strExt.CompareNoCase(_T("ai")) == 0)
		pImport = new CImportPostscript(m_Datainfo, m_layertree, m_dwMaxGeomId);
	else if(strExt.CompareNoCase(_T("svg")) == 0)
		pImport = new CImportSVG(m_Datainfo, m_layertree, m_dwMaxGeomId);

	CString strName = strFile.Mid(strFile.ReverseFind(_T('\\')) + 1);
	if(strName.ReverseFind(_T('.')) != -1)
		strName = strName.Left(strName.ReverseFind(_T('.')));
	//if(strName.GetLength()>0)
	//{
	//	strName.Replace(_T("High"), _T(""));
	//	CString firstletter = strName.Left(1);
	//	firstletter.MakeUpper();
	//	strName = firstletter+strName.Mid(1);
	//}
	m_Datainfo.m_strName = strName;
	this->SetTitle(strName);

	if(pImport != nullptr)
	{
		AfxGetApp()->BeginWaitCursor();
		if(pImport->Import(strFile) == TRUE)
		{
			delete pImport;
			pImport = nullptr;

			if(m_LayerTreeCtrl.m_hWnd != nullptr)
			{
				m_LayerTreeCtrl.DeleteAllItems();
				m_LayerTreeCtrl.BuildTree();
			}

			this->SendInitialUpdate();

			AfxGetApp()->EndWaitCursor();
			return TRUE;
		}
		else
		{
			delete pImport;
			pImport = nullptr;

			AfxGetApp()->EndWaitCursor();
			return FALSE;
		}
	}
	else
	{
		const CString strPath = strFile.Left(strFile.ReverseFind(_T('\\')));
		strFile = strFile.Mid(strFile.ReverseFind(_T('\\')) + 1);
		CStringList files;
		files.AddTail(strFile);

		CString dbname;
		CImportGdal importer(m_Datainfo, m_layertree, m_dwMaxGeomId);
		importer.hiddenlevel = this->hiddenlevel;
		return importer.Import(strPath, files, false, dbname);
	}
}

void CGrfDoc::OnFileExport()
{
	DWORD dwKey = AfxGetAuthorizationKey();
	if((dwKey & LICENCE::PUBLISHING) != LICENCE::PUBLISHING)
	{
		CLicenceDlg::Display();
		return;
	}

	TCHAR CurrentDir[256];
	GetCurrentDirectory(255, CurrentDir);
	CString strPath = AfxGetApp()->GetProfileString(_T(""), _T("Export Path"), CurrentDir);

	TCHAR BASED_CODE szIM_IM_Filter[] = //_T("ArcInfo Coverages  (*.arc;*.lab;*.cnt;*.pal;*.txt;*.dbf)|*.arc;*.lab;*.cnt;*.pal;*.txt;*.dbf|")
		//	_T("ARCView Shape File (*.shp;*.shx;*.dbf)|*.shp;*.shx;*.dbf|")
		//	_T("AutoCAD File (*.dxf)|*.dxf|")
		//	_T("MapInfo File (*.mif;*.mid)|*.mif;*.mid|")
		_T("Illustrator Ai File (*.Ai)|*.Ai|")
		_T("Illustrator Eps File (*.eps)|*.eps|")
		_T("Portable Document Format (*.pdf)|*.pdf|")
		_T("Image File (*.Png)|*.Png|")
		_T("Plain Text File (*.Txt)|*.Txt|");
	//	_T("Flash File (*.swf)|*.swf|");

	CFileDialog filedlg(FALSE,
		_T("dxf"),
		strPath,//strPath, 
		OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST,
		szIM_IM_Filter,
		nullptr);

	filedlg.m_ofn.lpstrTitle = _T("Export");
	if(filedlg.DoModal() != IDOK)
	{
		SetCurrentDirectory(CurrentDir);
		return;
	}
	SetCurrentDirectory(CurrentDir);

	CString pathname = filedlg.GetPathName();
	CString ext = filedlg.GetFileExt();
	ext.MakeUpper();

	AfxGetApp()->BeginWaitCursor();

	const unsigned int& tolerance = m_Datainfo.m_zoomPointToDoc * a_fJoinTolerance;
	CViewExporter viewinfo(m_Datainfo);
	viewinfo.m_bViewer = false;
	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);
	if(pView != nullptr)
	{
		viewinfo.ResetProjection(pView->m_viewMonitor.m_pGeocentric, true);
	}
	viewinfo.ScaleTo(nullptr, m_Datainfo.m_scaleSource, CPointF(0, 0));
	if(ext == _T("ARC"))
	{
		//	ExportArcInfo(&(m_layertree.m_layerlist),pathname);
	}
	else if(ext == _T("SHP"))
	{
		//	ExportArcView(&(m_layertree.m_layerlist),pathname);
	}
	else if(ext == _T("TAB"))
	{
		pathname = pathname.Left(pathname.ReverseFind(_T('\\')) + 1);
		//	ExportMapInfoTab(m_Datainfo,&(m_layertree.m_layerlist),pathname,tolerance);
	}
	else if(ext == _T("MIF"))
	{
		pathname = pathname.Left(pathname.ReverseFind(_T('\\')) + 1);
		//			ExportMapInfoMif(m_Datainfo,&(m_layertree.m_layerlist),pathname,tolerance);
	}
	else if(ext == _T("AI"))
	{
		ExportAI(m_layertree, m_Datainfo, pathname, tolerance);
	}
	else if(ext == _T("EPS"))
	{
		ExportEps(m_layertree, m_Datainfo, pathname, tolerance);
	}
	else if(ext == _T("PDF"))
	{
		ExportPdf(m_layertree, viewinfo, pathname, tolerance);
	}
	else if(ext == _T("DXF"))
	{
		//	ExportDxf(pathname);
	}
	/*else if(ext == _T("SWF"))
	{
	ExportSwf(pathname);
	}*/
	else if(ext == _T("PNG"))
	{
		Gdiplus::Bitmap* preview = this->CreateThumbnail(1000, 1000);
		if(preview != nullptr)
		{
			CLSID pngClsid;
			CLSIDFromString(L"{557cf406-1a04-11d3-9a73-0000f81ef32e}", &pngClsid);
			preview->Save(pathname, &pngClsid, NULL);
			::delete preview;
		}
	}
	else if(ext == _T("TXT"))
	{
		Gdiplus::Matrix matrix(0.0001, 0, 0, 0.0001, 0, 0);
		ExportTxt(pathname, m_layertree, matrix);
	}
	viewinfo.m_pGeocentric = nullptr;
	AfxGetApp()->EndWaitCursor();

	pathname = pathname.Left(pathname.ReverseFind(_T('\\')) + 1);
	AfxGetApp()->WriteProfileString(_T(""), _T("Export Path"), pathname);
}
void CGrfDoc::OnProcessWipeout()
{
	if(AfxMessageBox(IDS_LOSINGUNDOS, MB_YESNO) == IDYES)
	{
		m_actionStack.Clear();
		m_layertree.Wipeout();

		m_dwMaxGeomId = 0;
		SetModifiedFlag(TRUE);
		this->UpdateAllViews(nullptr);
	}
}
void CGrfDoc::OnProcessNormalize()
{
	if(AfxMessageBox(IDS_LOSINGUNDOS, MB_YESNO) == IDYES)
	{
		m_actionStack.Clear();
		m_layertree.Normalize();

		m_dwMaxGeomId = 0;
		SetModifiedFlag(TRUE);
		this->UpdateAllViews(nullptr);
	}
}
void CGrfDoc::OnPathReverse()
{
	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);

	m_actionStack.Start();
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->PathReverse(pView, pView->m_viewMonitor, m_actionStack);
	}
	m_actionStack.Stop();

	SetModifiedFlag(TRUE);
}

void CGrfDoc::OnPathDensify()
{
	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);
	const unsigned int tolerance = m_Datainfo.m_zoomPointToDoc * a_fDensity;

	AfxGetApp()->BeginWaitCursor();
	m_actionStack.Start();
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->Densify(pView, pView->m_viewMonitor, tolerance, m_actionStack);
	}
	m_actionStack.Stop();
	AfxGetApp()->EndWaitCursor();
	SetModifiedFlag(TRUE);
	//	CCommand* command = new CCommand("None");
	//	m_actionStack.Submit(command);	
}

void CGrfDoc::OnPathSimplify()
{
	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);
	const unsigned int& tolerance = m_Datainfo.m_zoomPointToDoc * a_fJoinTolerance;

	AfxGetApp()->BeginWaitCursor();
	m_actionStack.Start();
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->Simplify(pView, pView->m_viewMonitor, tolerance, m_actionStack);
	}
	m_actionStack.Stop();
	AfxGetApp()->EndWaitCursor();
	SetModifiedFlag(TRUE);
	//	CCommand* command = new CCommand("None");
	//	m_actionStack.Submit(command);	
}

void CGrfDoc::OnPathBuffer()
{
	CPathBufferDlg dlg(nullptr);
	if(dlg.DoModal() == IDOK)
	{
		POSITION pos = GetFirstViewPosition();
		CGrfView* pView = (CGrfView*)GetNextView(pos);
		const unsigned int distance = m_Datainfo.m_zoomPointToDoc * dlg.distance;
		const unsigned int tolerance = m_Datainfo.m_zoomPointToDoc;

		AfxGetApp()->BeginWaitCursor();
		for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
		{
			CLayer* layer = *it;
			if(layer->m_nActiveCount == 0)
				continue;

			layer->PathBuffer(pView, pView->m_viewMonitor, distance, tolerance, dlg.jointype, dlg.endtype, m_actionStack);
		}
		AfxGetApp()->EndWaitCursor();
		SetModifiedFlag(TRUE);
		//	CCommand* command = new CCommand("None");
		//	m_actionStack.Submit(command);	
	}
}

void CGrfDoc::OnPathClose()
{
	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);
	const unsigned int& tolerence = m_Datainfo.m_zoomPointToDoc * a_fJoinTolerance;

	AfxGetApp()->BeginWaitCursor();
	m_actionStack.Start();
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->Close(pView, pView->m_viewMonitor, tolerence, m_actionStack);
	}
	m_actionStack.Stop();
	AfxGetApp()->EndWaitCursor();
	SetModifiedFlag(TRUE);
	//	CCommand* command = new CCommand("None");
	//	m_actionStack.Submit(command);	
}

void CGrfDoc::OnFileMapinfo()
{
	HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Dataview.dll"));
	AfxSetResourceHandle(hInst);
	float oldMinimum = m_Datainfo.m_scaleMinimum;
	float oldMaximum = m_Datainfo.m_scaleMaximum;

	CDatainfo pOld(m_Datainfo);
	CDatainfoDlg dlg(nullptr, m_Datainfo);
	if(dlg.DoModal() == IDOK)
	{
		CDatainfo pNew(m_Datainfo);
		Undoredo::CModify<CWnd*, CDatainfo&, CDatainfo>* pModify = new Undoredo::CModify<CWnd*, CDatainfo&, CDatainfo>(nullptr, m_Datainfo, pOld, pNew);
		pModify->undo = Undoredo::Document::Datainfo::Undo_Modify;
		pModify->redo = Undoredo::Document::Datainfo::Redo_Modify;
		m_actionStack.Record(pModify);

		bool bPointToDocChanged = false;
		if(m_Datainfo.m_scaleSource != pOld.m_scaleSource)
			bPointToDocChanged = true;
		else if(m_Datainfo.m_pProjection == nullptr && pOld.m_pProjection == nullptr)
		{
		}
		else if(m_Datainfo.m_pProjection == nullptr && pOld.m_pProjection != nullptr)
			bPointToDocChanged = true;
		else if(m_Datainfo.m_pProjection != nullptr && pOld.m_pProjection == nullptr)
			bPointToDocChanged = true;
		else if(*(m_Datainfo.m_pProjection) != *(pOld.m_pProjection))
			bPointToDocChanged = true;

		if(m_Datainfo.m_scaleMinimum != oldMinimum || m_Datainfo.m_scaleMaximum != oldMaximum)
		{
			for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
			{
				CLayer* layer = *it;
				layer->ExtremizeScale(oldMinimum, oldMaximum, m_Datainfo.m_scaleMinimum, m_Datainfo.m_scaleMaximum);
			}
		}
		if(bPointToDocChanged == true)
		{
			AfxGetApp()->BeginWaitCursor();
			for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
			{
				CLayer* layer = *it;
				layer->RecalRect(m_Datainfo);
			}
			AfxGetApp()->EndWaitCursor();
		}
		POSITION pos = GetFirstViewPosition();
		if(pos != nullptr)
		{
			CGrfView* pView = (CGrfView*)GetNextView(pos);
			pView->m_viewMonitor.ResetProjection(pView->m_viewMonitor.m_pGeocentric, false);
			pView->m_viewPrinter.ResetProjection(pView->m_viewPrinter.m_pGeocentric, false);
		}
		SetModifiedFlag(TRUE);
		this->UpdateAllViews(nullptr);
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

BOOL CGrfDoc::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo == nullptr)
	{
		if(nCode == CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return COleDocument::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			POSITION pos = GetFirstViewPosition();
			CGrfView* pView = (CGrfView*)GetNextView(pos);
			unsigned long nActiveCount = 0;
			for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
			{
				CLayer* layer = *it;
				nActiveCount += layer->m_nActiveCount;
			}

			switch(nId)
			{
				case ID_FILE_SAVE:
					pCmdUI->Enable(IsModified());
					return true;
					break;
				case ID_GEOM_LOCK:
					{
						for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount == 0)
								continue;

							POSITION pos2 = layer->m_geomlist.GetTailPosition();
							while(pos2 != nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetPrev(pos2);
								if(pGeom->m_bActive == false)
									continue;

								if(pGeom->m_bLocked == true)
									continue;

								pCmdUI->Enable(TRUE);
								return true;
							}
						}
						pCmdUI->Enable(FALSE);
					}
					return true;
					break;
				case ID_GEOM_UNLOCK:
					{
						for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount == 0)
								continue;

							POSITION pos2 = layer->m_geomlist.GetTailPosition();
							while(pos2 != nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetPrev(pos2);
								if(pGeom->m_bActive == false)
									continue;
								if(pGeom->m_bLocked == false)
									continue;

								pCmdUI->Enable(TRUE);
								return true;
							}
						}
						pCmdUI->Enable(FALSE);
					}
					return true;
					break;
				case ID_EDIT_PASTE:
					{
						if(m_ClipboardFormat == (CLIPFORMAT)::RegisterClipboardFormat(_T("Diwatu Mapper Grf")) || m_ClipboardFormat == (CLIPFORMAT)::RegisterClipboardFormat(_T("Diwatu Mapper Gis")))
						{
							COleDataObject dataObject;
							BOOL bEnable = dataObject.AttachClipboard() && (dataObject.IsDataAvailable(m_ClipboardFormat) || COleClientItem::CanCreateFromData(&dataObject));
							pCmdUI->Enable(bEnable);
						}
						else
							pCmdUI->Enable(FALSE);
					}
					return true;
					break;
				case ID_EDIT_UNDO:
					if(AfxGetAuthorizationKey() == 0X00000000)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->Enable(m_actionStack.Undoable());
					return true;
					break;
				case ID_EDIT_REDO:
					if(AfxGetAuthorizationKey() == 0X00000000)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->Enable(m_actionStack.Redoable());
					return true;
					break;
				case ID_EDIT_CLIPIN:
				case ID_EDIT_CLIPOUT:
					pCmdUI->Enable(GetUniqueClosedPath() != nullptr);
					return true;
				case ID_EDIT_CLIPRELEASE:
					for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
					{
						CLayer* layer = *it;
						if(layer->m_nActiveCount == 0)
							continue;

						POSITION pos2 = layer->m_geomlist.GetTailPosition();
						while(pos2 != nullptr)
						{
							CGeom* pGeom = layer->m_geomlist.GetPrev(pos2);
							if(pGeom->m_bActive == false)
								continue;
							else if(pGeom->m_bLocked == true)
								continue;
							else if(pGeom->m_bGroup == false)
								continue;
							else if(pGeom->IsKindOf(RUNTIME_CLASS(CClip)) == TRUE)
							{
								pCmdUI->Enable(TRUE);
								return true;
							}
						}
					}
					pCmdUI->Enable(FALSE);
					return true;
				case ID_EDIT_MASKRELEASE:
					for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
					{
						CLayer* layer = *it;
						if(layer->m_nActiveCount == 0)
							continue;

						POSITION pos2 = layer->m_geomlist.GetTailPosition();
						while(pos2 != nullptr)
						{
							CGeom* pGeom = layer->m_geomlist.GetPrev(pos2);
							if(pGeom->m_bActive == false)
								continue;
							else if(pGeom->m_bLocked == true)
								continue;
							else if(pGeom->m_bGroup == false)
								continue;
							else if(pGeom->IsKindOf(RUNTIME_CLASS(CMask)) == true)
							{
								pCmdUI->Enable(TRUE);
								return true;
							}
						}
					}
					pCmdUI->Enable(FALSE);
					return true;
				case ID_TAG_SETUP:
				case ID_TAG_DELETE:
					{
						for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount == 0)
								continue;

							POSITION pos2 = layer->m_geomlist.GetTailPosition();
							while(pos2 != nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetPrev(pos2);
								if(pGeom->m_bActive == false)
									continue;
								else if(pGeom->m_bLocked == TRUE)
									continue;
								else if(pGeom->Gettype() == 9)
									continue;
								else if(pGeom->Gettype() == 10)
									continue;
								else if(pGeom->HasTag())
								{
									pCmdUI->Enable(TRUE);
									return true;
								}
							}
						}
						pCmdUI->Enable(FALSE);
					}
					return true;
					break;
				case ID_TAG_CREATE:
					{
						for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount == 0)
								continue;

							POSITION pos2 = layer->m_geomlist.GetTailPosition();
							while(pos2 != nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetPrev(pos2);
								if(pGeom->m_bActive == false)
									continue;
								else if(pGeom->HasTag())
									continue;
								else if(pGeom->Tagable())
								{
									pCmdUI->Enable(TRUE);
									return true;
								}
							}
						}
						pCmdUI->Enable(FALSE);
					}
					return true;
					break;
				case ID_EDIT_CUT:
				case ID_EDIT_CLEAR:
				case ID_EDIT_MOVE:
				case ID_ARRANGE_FORWARD:
				case ID_ARRANGE_BACKWARD:
				case ID_ARRANGE_TOFRONT:
				case ID_ARRANGE_TOBACK:
					pCmdUI->Enable(nActiveCount < 1 ? FALSE : TRUE);
					return true;
					break;
				case ID_EDIT_COPY:
				case ID_EDIT_DUPLICATE:
				case ID_EDIT_CLONE:
				case ID_EDIT_TRANSFORM:
				case ID_ANNO_SETUP:
				case ID_MAGNIFY_FITACTIVE:
					pCmdUI->Enable(nActiveCount < 1 ? FALSE : TRUE);
					return true;
					break;
				case ID_ALIGN_LEFT:
				case ID_ALIGN_HCENTER:
				case ID_ALIGN_RIGHT:
				case ID_ALIGN_BOTTOM:
				case ID_ALIGN_VCENTER:
				case ID_ALIGN_TOP:
				case ID_ALIGN_CENTER:
					{
						if(nActiveCount < 2)
							pCmdUI->Enable(FALSE);
						else
							pCmdUI->Enable(TRUE);
					}
					return true;
					break;
				case ID_MAP_COPY:
					{
						if(nActiveCount < 1)
							pCmdUI->Enable(FALSE);
						else if(m_Datainfo.m_pProjection == nullptr)
							pCmdUI->Enable(FALSE);
						else
							pCmdUI->Enable(TRUE);
					}
					return true;
					break;
				case ID_GEOM_GROUP:
					{
						for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount <= 1)
								continue;
							else
							{
								pCmdUI->Enable(TRUE);
								return true;
							}
						}
						pCmdUI->Enable(FALSE);
					}
					pCmdUI->Enable(FALSE);
					return true;
					break;
				case ID_GEOM_UNGROUP:
					{
						for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount == 0)
								continue;

							POSITION pos2 = layer->m_geomlist.GetTailPosition();
							while(pos2 != nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetPrev(pos2);
								if(pGeom->m_bActive == false)
									continue;
								else if(pGeom->m_bLocked == true)
									continue;
								else if(pGeom->m_bGroup == false)
									continue;
								else if(pGeom->IsKindOf(RUNTIME_CLASS(CDoughnut)) == TRUE)
									continue;
								else
								{
									pCmdUI->Enable(TRUE);
									return true;
								}
							}
						}
						pCmdUI->Enable(FALSE);
					}
					pCmdUI->Enable(FALSE);
					return true;
					break;
				case ID_POLYGON_SCULPT1:
				case ID_POLYGON_SCULPT2:
				case ID_POLYGON_SCULPT3:
				case ID_POLYGON_SCULPT4:
				case ID_POLYGON_SCULPT5:
				case ID_GEOM_COMBINE:
					{
						long nArea = 0;
						for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(nArea > 2)
								break;
							if(layer->m_nActiveCount == 0)
								continue;

							POSITION pos = layer->m_geomlist.GetHeadPosition();
							while(pos != nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetNext(pos);
								if(pGeom->m_bActive == false)
									continue;
								else if(pGeom->m_bLocked == TRUE)
									continue;
								else if(pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly)) == TRUE)
									continue;
								else if(pGeom->m_bClosed == TRUE)
								{
									nArea++;
								}
							}
						}
						if(nArea >= 2)
						{
							pCmdUI->Enable(TRUE);
							return TRUE;
						}
					}
					pCmdUI->Enable(FALSE);
					return TRUE;
					break;
				case ID_GEOM_DISBAND:
					{
						for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount == 0)
								continue;

							POSITION pos = layer->m_geomlist.GetHeadPosition();
							while(pos != nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetNext(pos);
								if(pGeom->m_bActive == false)
									continue;
								if(pGeom->m_bLocked == TRUE)
									continue;

								if(pGeom->IsKindOf(RUNTIME_CLASS(CDoughnut)) == TRUE)
								{
									pCmdUI->Enable(TRUE);
									return TRUE;
								}
							}
						}
						pCmdUI->Enable(FALSE);
					}
					return TRUE;
					break;
				case ID_ANNO_CLEAR:
				case ID_ANNO_REPLACE:
				case ID_ANNO_TOTEXT:
					{
						for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount == 0)
								continue;

							POSITION pos = layer->m_geomlist.GetHeadPosition();
							while(pos != nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetNext(pos);
								if(pGeom->m_bActive == false)
									continue;
								if(pGeom->m_bLocked == true)
									continue;
								if(pGeom->Gettype() == 9)
									continue;
								if(pGeom->Gettype() == 10)
									continue;
								if(pGeom->m_strName.GetLength() == 0)
									continue;

								pCmdUI->Enable(TRUE);
								return true;
							}
						}
						pCmdUI->Enable(FALSE);
					}
					return true;
					break;
				case ID_POINT_REMOVE:
				case ID_POINT_CORNER:
				case ID_POINT_CONNECT:
					{
						POSITION pos = GetFirstViewPosition();
						if(pos != nullptr)
						{
							CGrfView* pView = (CGrfView*)GetNextView(pos);
							if(pView->m_editgeom.m_pGeom != nullptr)
							{
								pCmdUI->Enable(TRUE);
								return true;
							}
						}
						pCmdUI->Enable(FALSE);
						return true;
					}
					break;

				case ID_PATH_BUFFER:
				case ID_PATH_DENSIFY:
				case ID_PATH_SIMPLIFY:
				case ID_PATH_REVERSE:
					{
						for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount == 0)
								continue;

							POSITION pos = layer->m_geomlist.GetHeadPosition();
							while(pos != nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetNext(pos);
								if(pGeom->m_bActive == false)
									continue;
								if(pGeom->m_bLocked == true)
									continue;
								if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == true)
								{
									pCmdUI->Enable(TRUE);
									return true;
								}
								if(pGeom->IsKindOf(RUNTIME_CLASS(CDoughnut)) == true)
								{
									pCmdUI->Enable(TRUE);
									return true;
								}
							}
						}
						pCmdUI->Enable(FALSE);
					}
					return true;
					break;
				case ID_PATH_CLOSE:
					{
						for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount == 0)
								continue;

							POSITION pos = layer->m_geomlist.GetHeadPosition();
							while(pos != nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetNext(pos);
								if(pGeom->m_bActive == false)
									continue;
								if(pGeom->m_bLocked == true)
									continue;
								if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == true && pGeom->m_bClosed == false)
								{
									pCmdUI->Enable(TRUE);
									return true;
								}
							}
						}
						pCmdUI->Enable(FALSE);
					}
					return true;
					break;
				case ID_COVERT_POLYTOBEZIER:
					{
						for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount == 0)
								continue;

							POSITION pos = layer->m_geomlist.GetHeadPosition();
							while(pos != nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetNext(pos);
								if(pGeom->m_bActive == false)
									continue;
								if(pGeom->m_bLocked == true)
									continue;
								if(pGeom->IsKindOf(RUNTIME_CLASS(CBezier)))
									continue;

								if(pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly)) == TRUE)
									continue;
								if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == TRUE)
								{
									pCmdUI->Enable(TRUE);
									return true;
								}
							}
						}
						pCmdUI->Enable(FALSE);
					}
					return true;
					break;
				case ID_COVERT_BEZIERTOPOLY:
					{
						for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount == 0)
								continue;

							POSITION pos = layer->m_geomlist.GetHeadPosition();
							while(pos != nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetNext(pos);
								if(pGeom->m_bActive == false)
									continue;
								if(pGeom->m_bLocked == true)
									continue;

								if(pGeom->IsKindOf(RUNTIME_CLASS(CBezier)) == TRUE)
								{
									pCmdUI->Enable(TRUE);
									return true;
								}
							}
						}
						pCmdUI->Enable(FALSE);
					}
					return true;
					break;
				case ID_TOPOEDGE_CREATE:
					{
						if(m_topolist.size() == 0)
						{
							pCmdUI->Enable(FALSE);
							return TRUE;
						}
						else if(m_pTopoCtrl == nullptr)
						{
							pCmdUI->Enable(FALSE);
							return TRUE;
						}
						else if(m_pTopoCtrl->GetSelTopo() == nullptr)
						{
							pCmdUI->Enable(FALSE);
							return TRUE;
						}
						else
						{
							pCmdUI->Enable(FALSE);

							for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
							{
								CLayer* layer = *it;
								if(layer->m_nActiveCount == 0)
									continue;

								POSITION pos = layer->m_geomlist.GetHeadPosition();
								while(pos != nullptr)
								{
									CGeom* pGeom = layer->m_geomlist.GetNext(pos);
									if(pGeom->m_bActive == false)
										continue;
									else if(pGeom->m_bClosed == TRUE)
										continue;
									else if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == TRUE)
									{
										pCmdUI->Enable(TRUE);
										return TRUE;
									}
								}
							}
						}
					}
					return TRUE;
					break;
				case ID_TOPOEDGE_REMOVE:
				case ID_TOPOEDGE_JOIN:
					{
						for(std::list<CTopology*>::reverse_iterator it = m_topolist.rbegin(); it != m_topolist.rend(); it++)
						{
							CTopology* pTopology = *it;

							for(CTopology::CEdgeMap::iterator it = pTopology->m_mapIdEdge.begin(); it != pTopology->m_mapIdEdge.end(); it++)
							{
								CEdge* pEdge = (CEdge*)it->second;
								if(pEdge->m_bActive == true)
								{
									pCmdUI->Enable(TRUE);
									return TRUE;
								}
							}
						}
					}
					pCmdUI->Enable(FALSE);
					return TRUE;
					break;
				case ID_TOOL_TOPOPICK:
					pCmdUI->Enable(m_topolist.size() > 0);
					pCmdUI->SetCheck(m_pTool == &topoPickTool);
					return TRUE;
					break;
				case ID_TOOL_TOPOEROSE:
					pCmdUI->Enable(m_topolist.size() > 0);
					pCmdUI->SetCheck(m_pTool == &eroseTool);
					return TRUE;
					break;
				case ID_TOOL_TOPOCROSS:
					pCmdUI->Enable(m_topolist.size() > 0);
					pCmdUI->SetCheck(m_pTool == &topoCrossTool);
					return TRUE;
					break;
				case ID_TOOL_TOPOSPLIT:
					pCmdUI->Enable(FALSE);
					//			pCmdUI->Enable(m_topolist.size() > 0);
					//			pCmdUI->SetCheck(m_pTool == &topoSplitTool);
					return TRUE;
					break;
				case ID_TOOL_RECTIFYDATUM:
					pCmdUI->SetCheck(m_pTool == &m_datumTool);
					return TRUE;
					break;
				case ID_RECTIFY_SAVEDATUM:
					pCmdUI->Enable(m_Tin.m_datumlist.GetCount() != 0);
					return true;
					break;
				case ID_RECTIFY:
					{
						if(nActiveCount < 1)
							pCmdUI->Enable(FALSE);
						else if(m_Tin.m_trianglelist.GetCount() > 0)
							pCmdUI->Enable(TRUE);
						else
							pCmdUI->Enable(FALSE);
						return true;
						break;
					}
					return true;
					break;
				default:
					break;
			}
			pCmdUI->m_bContinueRouting = FALSE;
		}
		else if(nCode == CN_COMMAND)
		{
		}
	}

	return COleDocument::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}

void CGrfDoc::OnAnnoSetup()
{
	POSITION pos1 = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos1);

	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->SetupAnno(pView, pView->m_viewMonitor);
	}
}

void CGrfDoc::OnAnnoClear()
{
	POSITION pos1 = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos1);

	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->ClearAnno(pView, pView->m_viewMonitor);
	}
}

void CGrfDoc::OnAnnoReplace()
{
	CReplaceDlg dlg(nullptr, this, m_layertree);
	if(dlg.DoModal() == IDOK)
	{
	}
}

void CGrfDoc::OnAnnoToText()
{
	POSITION pos1 = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos1);
	pView->m_editgeom.UnFocusAll(pView);

	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->GeomName(pView, m_Datainfo, pView->m_viewMonitor);
	}

	SetModifiedFlag(TRUE);
}

DWORD CGrfDoc::ApplyGeomId()
{
	return ++m_dwMaxGeomId;
}

void CGrfDoc::FreeGeomId(CGeom* pGeom)
{
	if(pGeom->m_geoId == m_dwMaxGeomId)
		m_dwMaxGeomId--;
}

void CGrfDoc::OnCovertBezierToPoly()
{
	const unsigned int& tolerance = m_Datainfo.m_zoomPointToDoc * a_fJoinTolerance;

	POSITION pos1 = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos1);
	pView->m_editgeom.UnFocusAll(pView);

	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->BezierToPoly(pView, pView->m_viewMonitor, tolerance, m_actionStack);
	}
	SetModifiedFlag(TRUE);
}

void CGrfDoc::OnCovertPolyToBezier()
{
	POSITION pos1 = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos1);
	pView->m_editgeom.UnFocusAll(pView);

	m_actionStack.Start();
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->PolyToBezier(pView, pView->m_viewMonitor, 0.5, m_actionStack);
	}
	m_actionStack.Stop();
	SetModifiedFlag(TRUE);
}

void CGrfDoc::OnPolygonSculpt(UINT nId)
{
	CLayer* layer1 = nullptr;
	CLayer* layer2 = nullptr;
	CGeom* geom1 = nullptr;
	CGeom* geom2 = nullptr;
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		POSITION pos2 = layer->m_geomlist.GetTailPosition();
		while(pos2 != nullptr)
		{
			CGeom* geom = layer->m_geomlist.GetPrev(pos2);
			if(geom->m_bActive == false)
				continue;
			else if(geom2 != nullptr)
			{
				AfxMessageBox(_T("Actived too more geometries!"));
				return;
			}
			else if(geom->m_bClosed == true && (geom->IsKindOf(RUNTIME_CLASS(CPoly)) || geom->IsKindOf(RUNTIME_CLASS(CCluster))))
			{
				if(geom1 == nullptr)
				{
					layer1 = layer;
					geom1 = geom;
				}
				else if(geom2 == nullptr)
				{
					layer2 = layer;
					geom2 = geom;
				}
				else
					break;
			}
		}
		if(geom1 != nullptr && geom2 != nullptr)
			break;
	}

	if(geom1 == nullptr || geom2 == nullptr)
	{
		AfxMessageBox(_T("No actived closed geometry!"));
		return;
	}
	else
	{
		Clipper2Lib::Paths64 paths641;
		Clipper2Lib::Paths64 paths642;
		geom1->GetPaths64(paths641, m_Datainfo.m_zoomPointToDoc);
		geom2->GetPaths64(paths642, m_Datainfo.m_zoomPointToDoc);
		if(paths641.size() > 0 && paths642.size() > 0)
		{
			Clipper2Lib::ClipType cliptype = Clipper2Lib::ClipType::Intersection;
			switch(nId)
			{
				case ID_POLYGON_SCULPT1:
					cliptype = Clipper2Lib::ClipType::Intersection;
					break;
				case ID_POLYGON_SCULPT2:
					cliptype = Clipper2Lib::ClipType::Union;
					break;
				case ID_POLYGON_SCULPT3:
					cliptype = Clipper2Lib::ClipType::Xor;
					break;
				case ID_POLYGON_SCULPT4:
					cliptype = Clipper2Lib::ClipType::Difference;
					break;
				case ID_POLYGON_SCULPT5:
					cliptype = Clipper2Lib::ClipType::Difference;
					break;
			}
			Clipper2Lib::Paths64 paths64 = Clipper2Lib::BooleanOp(cliptype, FillRule::NonZero, paths641, paths642);
			if(paths64.size() > 0)
			{
				CGeom* pGeom = CPath::Convert(paths64, true);
				geom2->CopyTo(pGeom, true);

				m_actionStack.Start();

				POSITION pos = GetFirstViewPosition();
				CGrfView* pView = (CGrfView*)GetNextView(pos);
				if(AfxMessageBox(_T("Remove the two original geometriesΔ"), MB_YESNO) == IDYES)
				{
					layer1->Clear(pView, pView->m_viewMonitor, m_actionStack);
					layer2->Clear(pView, pView->m_viewMonitor, m_actionStack);
				}
				else
				{
					CClientDC dc(pView);
					geom1->m_bActive = false;
					geom2->m_bActive = false;
					geom1->DrawAnchors(&dc, pView->m_viewMonitor);
					geom2->DrawAnchors(&dc, pView->m_viewMonitor);
					layer1->m_nActiveCount--;
					layer2->m_nActiveCount--;
				}

				layer2->NewGeom(pView, m_Datainfo, pGeom, m_actionStack);
				layer2->Invalidate(pView, pView->m_viewMonitor, pGeom);
				m_actionStack.Stop();
			}
		}
	}
}

void CGrfDoc::OnPointRemove()
{
	POSITION pos = GetFirstViewPosition();
	if(pos == nullptr)
		return;

	CGrfView* pView = (CGrfView*)GetNextView(pos);

	pView->m_editgeom.Clear(pView);
}

void CGrfDoc::OnPointCorner()
{
	POSITION pos = GetFirstViewPosition();
	if(pos == nullptr)
		return;

	CGrfView* pView = (CGrfView*)GetNextView(pos);

	pView->m_editgeom.Corner(pView);
}

void CGrfDoc::OnPointConnect()
{
	POSITION pos = GetFirstViewPosition();
	if(pos == nullptr)
		return;

	CGrfView* pView = (CGrfView*)GetNextView(pos);

	pView->m_editgeom.Cornnect(pView);
}
void CGrfDoc::OnTagCreateOptimized()
{
	//CRect effectiveRect;
	//POSITION pos11 = m_layertree.m_layerlist.GetTailPosition();
	//while (pos11 != nullptr)
	//{
	//	CLayer* layer = m_layertree.m_layerlist.GetPrev(pos11);
	//	if(layer == nullptr)
	//		continue;
	//	if(layer->m_bVisible == false)
	//		continue;

	//	if(layer->m_nActiveCount == 0)
	//		continue;

	//	int fontSize = layer->m_pHint != nullptr ? layer->m_pHint->m_nSize*10 : 90;
	//	POSITION pos12 = layer->m_geomlist.GetTailPosition();
	//	while (pos12 != nullptr)
	//	{
	//		CGeom* geom = layer->m_geomlist.GetPrev(pos12);
	//		if(geom == nullptr)
	//			continue;
	//		else if(geom->m_bActive == false)
	//			continue;
	//		else if(geom->m_pTag != nullptr)
	//			continue;
	//		else if(geom->m_strName.IsEmpty() == FALSE)
	//		{
	//			CRect rect = geom->m_Rect;
	//			rect.InflateRect(fontSize*geom->m_strName.GetLength()/2,fontSize*geom->m_strName.GetLength()/2);
	//			effectiveRect.UnionRect(effectiveRect,rect);
	//		}
	//	}
	//} 

	//Bitmap bitmap(1, 1);
	//Graphics g(&bitmap);
	//CArray<Rect, Gdiplus::Rect&> OccupiedRect;	
	//POSITION pos21 = m_layertree.m_layerlist.GetTailPosition();
	//while (pos21 != nullptr)
	//{
	//	CLayer* layer = m_layertree.m_layerlist.GetPrev(pos21);
	//	if(layer == nullptr)
	//		continue;
	//	if(layer->m_bVisible == false)
	//		continue;

	//	CHint* pHint = layer->m_pHint == nullptr ? layer->m_pHint : CHint::Default();
	//	POSITION pos22 = layer->m_geomlist.GetTailPosition();
	//	while (pos22 != nullptr)
	//	{
	//		CGeom* geom = layer->m_geomlist.GetPrev(pos22);
	//		if(geom == nullptr)
	//			continue;

	//		CRect rect = geom->m_Rect;
	//		if(rect.IntersectRect(rect,effectiveRect) == TRUE)
	//		{
	//			if(geom->m_bType == 8)
	//			{
	//				Rect markRect = Gdiplus::Rect(geom->m_Rect.left,geom->m_Rect.top,geom->m_Rect.Width(),geom->m_Rect.Height());
	//				OccupiedRect.Add(markRect);
	//			}

	//			if(geom->m_pTag != nullptr)
	//			{
	//				Rect rect(geom->m_Rect.left,geom->m_Rect.top,geom->m_Rect.Width(),geom->m_Rect.Height());
	//				Rect tagRect = pHint->MeasureTextRect(*g,rect,,CSize(72,72));
	//				OccupiedRect.Add(tagRect);
	//			}
	//		}
	//	}
	//}
	//::delete bitmap;
	//bitmap = nullptr;
	//delete g;

	//POSITION pos31 = m_layertree.m_layerlist.GetTailPosition();
	//while (pos31 != nullptr)
	//{
	//	CLayer* layer = m_layertree.m_layerlist.GetPrev(pos31);
	//	if(layer == nullptr)
	//		continue;

	//	layer->CreateOptimizedTag(OccupiedRect);
	//}
}

void CGrfDoc::OnTagCreate()
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Geometry.dll"));
	AfxSetResourceHandle(hInst);

	ANCHOR anchor = ANCHOR::ANCHOR_0;
	HALIGN hAlign = HALIGN::HA_LEFT;
	VALIGN vAlign = VALIGN::VA_CENTER;
	CTagDlg dlg(nullptr, anchor, hAlign, vAlign);
	if(dlg.DoModal() == IDOK)
	{
		POSITION pos1 = GetFirstViewPosition();
		CGrfView* pView = (CGrfView*)GetNextView(pos1);

		for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			if(layer->m_nActiveCount == 0)
				continue;

			layer->CreateTag(pView, pView->m_viewMonitor, anchor, hAlign, vAlign);
		}

		SetModifiedFlag(TRUE);
	}
	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CGrfDoc::OnTagSetup()
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Geometry.dll"));
	AfxSetResourceHandle(hInst);

	ANCHOR anchor = ANCHOR_6;
	HALIGN hAlign = HALIGN::HA_LEFT;
	VALIGN vAlign = VALIGN::VA_CENTER;
	CTagDlg dlg(nullptr, anchor, hAlign, vAlign);
	if(dlg.DoModal() == IDOK)
	{
		POSITION pos1 = GetFirstViewPosition();
		CGrfView* pView = (CGrfView*)GetNextView(pos1);

		for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			if(layer->m_nActiveCount == 0)
				continue;

			layer->SetupTag(pView, pView->m_viewMonitor, anchor, hAlign, vAlign);
		}
		SetModifiedFlag(TRUE);
	}
	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CGrfDoc::OnTagDelete()
{
	POSITION pos1 = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos1);

	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;

		layer->DeleteTag(pView, pView->m_viewMonitor);
	}
	SetModifiedFlag(TRUE);
}
void CGrfDoc::OnGeogroidReset()
{
	AfxGetApp()->BeginWaitCursor();

	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(!layer->m_bVisible)
			continue;
		if(!layer->m_bCentroid)
			continue;
		layer->ResetGeogroid(pView, pView->m_viewMonitor);
	}
	SetModifiedFlag(TRUE);

	AfxGetApp()->EndWaitCursor();
}

void CGrfDoc::OnLabeloidReset()
{
	AfxGetApp()->BeginWaitCursor();

	POSITION pos = GetFirstViewPosition();
	CGrfView* pView = (CGrfView*)GetNextView(pos);
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bVisible == false)
			continue;
		if(layer->m_bCentroid == false)
			continue;
		layer->ResetLabeloid(pView, pView->m_viewMonitor);
	}
	SetModifiedFlag(TRUE);

	AfxGetApp()->EndWaitCursor();
}

void CGrfDoc::OnGeogroidPlant()
{
	AfxGetApp()->BeginWaitCursor();

	std::list<CMark*> seeds;
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;
		if(layer->m_bVisible == false)
			continue;

		POSITION pos = layer->m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetNext(pos);
			if(pGeom->m_bActive == false)
				continue;
			if(pGeom->IsKindOf(RUNTIME_CLASS(CMark)) == TRUE)
			{
				seeds.push_back((CMark*)pGeom);
			}
		}
	}

	if(seeds.size() > 0)
	{
		POSITION pos = GetFirstViewPosition();
		CGrfView* pView = (CGrfView*)GetNextView(pos);
		for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			layer->PlantGeogroid(pView, pView->m_viewMonitor, seeds);
		}
		seeds.clear();
		SetModifiedFlag(TRUE);
	}

	AfxGetApp()->EndWaitCursor();
}

void CGrfDoc::OnLabeloidPlant()
{
	AfxGetApp()->BeginWaitCursor();

	std::list<CMark*> seeds;
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount == 0)
			continue;
		if(layer->m_bVisible == false)
			continue;

		POSITION pos = layer->m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetNext(pos);
			if(pGeom->m_bActive == false)
				continue;
			if(pGeom->IsKindOf(RUNTIME_CLASS(CMark)) == TRUE)
			{
				seeds.push_back((CMark*)pGeom);
			}
		}
	}
	if(seeds.size() > 0)
	{
		POSITION pos = GetFirstViewPosition();
		CGrfView* pView = (CGrfView*)GetNextView(pos);
		for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			layer->PlantLabeloid(pView, pView->m_viewMonitor, seeds);
		}
		seeds.clear();
		SetModifiedFlag(TRUE);
	}

	AfxGetApp()->EndWaitCursor();
}

void CGrfDoc::OnTopoCreateEdges()
{
	POSITION pos = GetFirstViewPosition();
	if(pos == nullptr)
		return;

	CGrfView* pView = (CGrfView*)GetNextView(pos);
	if(pView == nullptr)
		return;

	if(m_pTopoCtrl == nullptr)
		return;

	CTopology* pTopology = (CTopology*)m_pTopoCtrl->GetSelTopo();
	if(pTopology == nullptr)
		return;

	const unsigned int& tolerence = m_Datainfo.m_zoomPointToDoc * a_fTopoTolerance;
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		POSITION pos2 = layer->m_geomlist.GetTailPosition();
		while(pos2 != nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetPrev(pos2);
			if(pGeom->m_bActive == false)
				continue;
			else if(pGeom->m_bClosed == true)
				continue;
			else if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == TRUE)
			{
				pTopology->InsertEdge(pView, pView->m_viewMonitor, layer->m_wId, (CPoly*)pGeom, tolerence);
			}
		}
	}

	pView->InactivateAll();
}

void CGrfDoc::OnTopoPickTool()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_wndDrawTool.SwitchToolTo(&topoPickTool);
}

void CGrfDoc::OnTopoEroseTool()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_wndDrawTool.SwitchToolTo(&eroseTool);
}

void CGrfDoc::OnTopoSplitTool()
{
	//CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	//pMainFrame->m_wndDrawTool.SwitchToolTo(&topoSplitTool);
}
void CGrfDoc::OnTopoCrossTool()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_wndDrawTool.SwitchToolTo(&topoCrossTool);
}

void CGrfDoc::OnRectifyToolDatum()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_wndDrawTool.SwitchToolTo(&m_datumTool);
}

void CGrfDoc::OnTopoRemoveEdges()
{
	POSITION pos = GetFirstViewPosition();
	if(pos == nullptr)
		return;

	CGrfView* pView = (CGrfView*)GetNextView(pos);
	if(pView == nullptr)
		return;

	for(std::list<CTopology*>::reverse_iterator it = m_topolist.rbegin(); it != m_topolist.rend(); ++it)
	{
		CTopology* pTopology = *it;
		pTopology->OnClear(pView, m_Datainfo, pView->m_viewMonitor);
	}
}

void CGrfDoc::OnTopoEdgesJoin()
{
	POSITION pos = GetFirstViewPosition();
	if(pos == nullptr)
		return;

	CGrfView* pView = (CGrfView*)GetNextView(pos);
	if(pView == nullptr)
		return;

	for(std::list<CTopology*>::reverse_iterator it = m_topolist.rbegin(); it != m_topolist.rend(); it++)
	{
		CTopology* pTopology = *it;
		if(pTopology->m_bVisible == false)
			continue;

		//	if(pTopology->JoinEdges(pView, m_Datainfo, pView->m_viewMonitor, m_actionStack) == true)
		{
			//		break;
		}
	}
}

void CGrfDoc::OnPublishPC(CDeck* pDeck, CDaoRecordset* rs)
{
	if(pDeck == nullptr)
		return;

	for(std::list<CTopology*>::reverse_iterator it = m_topolist.rbegin(); it != m_topolist.rend(); it++)
	{
		CTopology* pTopology = *it;
		CString strFile = COleDocument::GetPathName();
		CString strTopo = strFile.Left(strFile.ReverseFind(_T('.')));

		pTopology->ExportPC(strTopo, m_Datainfo);
	}

	if(m_Datainfo.m_pProjection == nullptr)
	{
		POSITION pos = pDeck->m_polylist.GetHeadPosition();
		while(pos != nullptr)
		{
			CPolyF* poly = pDeck->m_polylist.GetNext(pos);
			delete poly;
		}
		pDeck->m_polylist.RemoveAll();
	}
	else
	{
		POSITION pos1 = pDeck->m_polylist.GetHeadPosition();
		POSITION pos2 = pos1;
		while((pos2 = pos1) != nullptr)
		{
			CPolyF* poly = pDeck->m_polylist.GetNext(pos1);
			if(poly->m_nAnchors == 5)
			{
				pDeck->m_polylist.RemoveAt(pos2);
				delete poly;
				poly = nullptr;
			}
		}

		if(pDeck->m_polylist.GetCount() == 0)
		{
			CRect rect = m_Datainfo.GetDocRect();

			CPolyF* dPoly = new CPolyF;

			CPoint docPoint1 = CPoint(rect.left, rect.bottom);
			CPointF mapPoint1 = m_Datainfo.DocToMap(docPoint1);
			CPointF geoPoint1 = m_Datainfo.m_pProjection->MapToGeo(mapPoint1);
			dPoly->AddAnchor(geoPoint1);

			CPoint docPoint2 = CPoint(rect.right, rect.bottom);
			CPointF mapPoint2 = m_Datainfo.DocToMap(docPoint2);
			CPointF geoPoint2 = m_Datainfo.m_pProjection->MapToGeo(mapPoint2);
			dPoly->AddAnchor(geoPoint2);

			CPoint docPoint3 = CPoint(rect.right, rect.top);
			CPointF mapPoint3 = m_Datainfo.DocToMap(docPoint3);
			CPointF geoPoint3 = m_Datainfo.m_pProjection->MapToGeo(mapPoint3);
			dPoly->AddAnchor(geoPoint3);

			CPoint docPoint4 = CPoint(rect.left, rect.top);
			CPointF mapPoint4 = m_Datainfo.DocToMap(docPoint4);
			CPointF geoPoint4 = m_Datainfo.m_pProjection->MapToGeo(mapPoint4);
			dPoly->AddAnchor(geoPoint4);

			CPoint docPoint5 = CPoint(rect.left, rect.bottom);
			CPointF mapPoint5 = m_Datainfo.DocToMap(docPoint5);
			CPointF geoPoint5 = m_Datainfo.m_pProjection->MapToGeo(mapPoint5);
			dPoly->AddAnchor(geoPoint5);

			dPoly->RecalRect();
			pDeck->m_polylist.AddTail(dPoly);
			pDeck->RecalRect();
		}
	}

	if(pDeck->m_bQueryble == TRUE)
	{
		for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			if(layer->m_bKeyQuery == false)
				continue;

			POSITION pos2 = layer->m_geomlist.GetTailPosition();
			while(pos2 != nullptr)
			{
				CGeom* pGeom = layer->m_geomlist.GetPrev(pos2);
				if(pGeom->m_strName.IsEmpty() == TRUE)
					continue;

				CString string = pGeom->m_strName;
				string.Replace(_T("\r\n"), _T(" "));
				if(string.GetLength() > 100)
					string = string.Left(100);

				rs->AddNew();
				COleVariant var;

				var = COleVariant(string, VT_BSTRT);
				rs->SetFieldValue(_T("Name"), var);

				var = COleVariant((short)(pDeck->m_wId), VT_I2);
				rs->SetFieldValue(_T("Map"), var);

				var = COleVariant((short)(layer->m_wId), VT_I2);;
				rs->SetFieldValue(_T("Layer"), var);

				var = COleVariant((long)(pGeom->m_geoId), VT_I4);
				rs->SetFieldValue(_T("Geom"), var);

				rs->Update();
			}
		}
	}

	CFileException fe;
	CFile* file = COleDocument::GetFile(pDeck->m_strFile, CFile::modeRead | CFile::shareDenyWrite, &fe);
	DWORD dwSize = GetFileSize((HANDLE)(file->m_hFile), nullptr);
	file->Close();
	if(dwSize > 100000)
	{
		int docDelta = m_Datainfo.m_zoomPointToDoc * 256;
		CSize docCanvas = m_Datainfo.GetDocCanvas();
		CSize docOffset = m_Datainfo.GetDocOrigin();
		CSize docGrid = CSize(docDelta, docDelta);

		CString strFile = pDeck->m_strFile;
		strFile = strFile.Left(strFile.ReverseFind(_T('.')));
		CString strGrid = strFile + _T(".Grd");
		CString strGeom = strFile + _T(".Geo");

		m_layertree.ExportPCGeom(strGeom);
		m_layertree.ExportGrid(strGrid, docGrid, docOffset, docCanvas);

		for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			POSITION pos2 = layer->m_geomlist.GetHeadPosition();
			while(pos2 != nullptr)
			{
				CGeom* pGeom = layer->m_geomlist.GetNext(pos2);
				delete pGeom;
			}
			layer->m_geomlist.RemoveAll();
		}
		COleDocument::SetModifiedFlag(TRUE);
		COleDocument::DoFileSave();
	}
}

void CGrfDoc::OnPublishCE(CDeck* pDeck)
{
	CDaoDatabase* pDB = new CDaoDatabase();

	CString strMDB = COleDocument::GetPathName();
	strMDB = strMDB.Left(strMDB.ReverseFind(_T('.')));

	if(_taccess(strMDB + _T(".Mdb"), 0) != -1)
	{
		pDB->Open(strMDB, TRUE, FALSE, nullptr);

		CString strSQL;
		strSQL.Format(_T("DELETE FROM %s"), _T("Label"));

		pDB->Execute(strSQL);
	}
	else
	{
		try
		{
			pDB->Create(strMDB);
		}
		catch(CDaoException* ex)
		{
			OutputDebugString(_T("Error when create Access file"));
			ex->ReportError();
			ex->Delete();
			delete pDB;
			return;
		}
		catch(CException* ex)
		{
			ex->Delete();
		}
		CString strDSN = _T("MS Access Database");

		if(pDB->IsOpen() == FALSE)
		{
			delete pDB;
			return;
		}

		CDaoTableDef td(pDB);
		td.Create(_T("Label"), dbAttachExclusive, _T(""), _T("DATABASE = "));

		CDaoFieldInfo fieldInfo;
		fieldInfo.m_strName = _T("Name");
		fieldInfo.m_nType = dbText;
		fieldInfo.m_lSize = 60;
		fieldInfo.m_lAttributes = dbVariableField;
		fieldInfo.m_nOrdinalPosition = 0;
		fieldInfo.m_bRequired = FALSE;
		fieldInfo.m_bAllowZeroLength = TRUE;
		fieldInfo.m_lCollatingOrder = 0;
		td.CreateField(fieldInfo);

		fieldInfo.m_strName = _T("Map");
		fieldInfo.m_lSize = 2;
		fieldInfo.m_nType = dbInteger;
		fieldInfo.m_lAttributes = dbFixedField;
		fieldInfo.m_bAllowZeroLength = FALSE;
		td.CreateField(fieldInfo);

		fieldInfo.m_strName = _T("Layer");
		td.CreateField(fieldInfo);

		fieldInfo.m_strName = _T("Code");
		fieldInfo.m_lSize = 4;
		fieldInfo.m_nType = dbLong;
		td.CreateField(fieldInfo);

		fieldInfo.m_strName = _T("Geom");
		td.CreateField(fieldInfo);

		td.Append();
		td.Close();
	}

	CDaoRecordset rs(pDB);
	try
	{
		CString strSQL;
		strSQL.Format(_T("SELECT * FROM %s"), _T("Label"));

		rs.Open(dbOpenDynaset, strSQL);
	}
	catch(CDaoException* ex)
	{
		//			OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}
	catch(CException* ex)
	{
		ex->Delete();
	}

	CSize docCanvas = m_Datainfo.GetDocCanvas();
	if(m_Datainfo.m_pProjection == nullptr)
	{
		POSITION pos = pDeck->m_polylist.GetHeadPosition();
		while(pos != nullptr)
		{
			CPolyF* poly = pDeck->m_polylist.GetNext(pos);
			delete poly;
		}
		pDeck->m_polylist.RemoveAll();
	}
	else
	{
		POSITION pos = pDeck->m_polylist.GetHeadPosition();
		POSITION curpos = pos;
		while((curpos = pos) != nullptr)
		{
			CPolyF* poly = pDeck->m_polylist.GetNext(pos);
			if(poly->m_nAnchors == 5)
			{
				pDeck->m_polylist.RemoveAt(curpos);
				delete poly;
			}
		}

		if(pDeck->m_polylist.GetCount() == 0)
		{
			CRect rect = CRect(0, 0, docCanvas.cx, docCanvas.cy);

			CPolyF* dPoly = new CPolyF;

			CPoint docPoint1 = CPoint(0, rect.bottom);
			CPointF mapPoint1 = m_Datainfo.DocToMap(docPoint1);
			CPointF geoPoint1 = m_Datainfo.m_pProjection->MapToGeo(mapPoint1);
			dPoly->AddAnchor(geoPoint1);

			CPoint docPoint2 = CPoint(rect.right, rect.bottom);
			CPointF mapPoint2 = m_Datainfo.DocToMap(docPoint2);
			CPointF geoPoint2 = m_Datainfo.m_pProjection->MapToGeo(mapPoint2);
			dPoly->AddAnchor(geoPoint2);

			CPoint docPoint3 = CPoint(rect.right, 0);
			CPointF mapPoint3 = m_Datainfo.DocToMap(docPoint3);
			CPointF geoPoint3 = m_Datainfo.m_pProjection->MapToGeo(mapPoint3);
			dPoly->AddAnchor(geoPoint3);

			CPoint docPoint4 = CPoint(rect.left, 0);
			CPointF mapPoint4 = m_Datainfo.DocToMap(docPoint4);
			CPointF geoPoint4 = m_Datainfo.m_pProjection->MapToGeo(mapPoint4);
			dPoly->AddAnchor(geoPoint4);

			CPoint docPoint5 = CPoint(0, rect.bottom);
			CPointF mapPoint5 = m_Datainfo.DocToMap(docPoint5);
			CPointF geoPoint5 = m_Datainfo.m_pProjection->MapToGeo(mapPoint5);
			dPoly->AddAnchor(geoPoint5);

			dPoly->RecalRect();
			pDeck->m_polylist.AddTail(dPoly);
		}
	}

	pDeck->m_bQueryble = FALSE;
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bKeyQuery == false)
			continue;

		POSITION pos2 = layer->m_geomlist.GetTailPosition();
		POSITION pos = pos2;
		while((pos = pos2) != nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetPrev(pos2);
			if(pGeom->m_strName.IsEmpty() == TRUE)
				continue;

			CString string = pGeom->m_strName;
			string.Replace(_T("\r\n"), _T(" "));
			//	string.Replace(_T("\r\n"),nullptr);
			//	string.Replace(_T(" "),nullptr);
			//	string.Replace(_T("¡¡"),nullptr);

			if(string.IsEmpty() == TRUE)
				continue;
			if(string.GetLength() > 30)
				continue;

			rs.AddNew();
			COleVariant var;

			var = COleVariant(string, VT_BSTRT);
			rs.SetFieldValue(_T("Name"), var);

			var = COleVariant((short)(pDeck->m_wId), VT_I2);
			rs.SetFieldValue(_T("Map"), var);

			var = COleVariant((short)(layer->m_wId), VT_I2);;
			rs.SetFieldValue(_T("Layer"), var);

			var = COleVariant((long)(pGeom->m_geoId), VT_I4);
			rs.SetFieldValue(_T("Geom"), var);

			rs.Update();
			pDeck->m_bQueryble = TRUE;

			COleDocument::SetModifiedFlag(TRUE);
		}
	}
	rs.Close();
	pDB->Close();

	CFileException fe;
	CFile* file = COleDocument::GetFile(pDeck->m_strFile, CFile::modeRead | CFile::shareDenyWrite, &fe);
	DWORD dwSize = GetFileSize((HANDLE)(file->m_hFile), nullptr);
	file->Close();
	if(dwSize > 10000)
	{
		int docDelta = m_Datainfo.m_zoomPointToDoc * 256;
		CSize docCanvas = m_Datainfo.GetDocCanvas();
		CSize docOffset = m_Datainfo.GetDocOrigin();
		CSize docGrid = CSize(docDelta, docDelta);

		CString strFile = pDeck->m_strFile;
		strFile = strFile.Left(strFile.ReverseFind(_T('.')));
		CString strGrid = strFile + _T(".Grd");
		CString strGeom = strFile + _T(".Geo");

		m_layertree.ExportGrid(strGrid, docGrid, docOffset, docCanvas);
		m_layertree.ExportCEGeom(strGeom);

		for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
		{
			CLayer* layer = *it;
			POSITION pos2 = layer->m_geomlist.GetHeadPosition();
			while(pos2 != nullptr)
			{
				CGeom* pGeom = layer->m_geomlist.GetNext(pos2);
				delete pGeom;
			}
			layer->m_geomlist.RemoveAll();
		}

		CString strCEFile = pDeck->m_strFile;
		if(strCEFile.ReverseFind(_T('.')) != -1)
		{
			strCEFile = strCEFile.Left(strCEFile.ReverseFind(_T('.')));
		}
		strCEFile += _T(".cis");

		CFile file;
		if(file.Open(strCEFile, CFile::modeCreate | CFile::modeWrite) == TRUE)
		{
			CArchive ar(&file, CArchive::store);

			this->ReleaseCE(ar);

			ar.Close();
			file.Close();
		}
	}
	else
	{
		CString strFile = COleDocument::GetPathName();
		strFile = strFile.Left(strFile.ReverseFind(_T('.')));
		strFile += _T(".cis");

		CFile file;
		if(file.Open(strFile, CFile::modeCreate | CFile::modeWrite) == TRUE)
		{
			CArchive ar(&file, CArchive::store);

			ReleaseCE(ar);

			ar.Close();
			file.Close();
		}
	}
}

void CGrfDoc::ReleaseWEBCap(UINT nId, CString strFolder, CString strName)
{
	const CString strDefault = (CString)GetExeDirectory();

	{//
		const CString strPath = AfxGetProfileString(HKEY_LOCAL_MACHINE, _T("Diwatu"), _T(""), _T(""), strDefault);
		const CStringA strSource = (CStringA)strPath + "Wamap.dll";
		const CStringA strTarget = (CStringA)strFolder + "\\Wamap.swf";
		CopyFileA(strSource, strTarget, FALSE);
	}
	{//xml file
		const CString strPath = AfxGetProfileString(HKEY_LOCAL_MACHINE, _T("Diwatu"), _T(""), _T("Assistant"), strDefault + _T("Assistant\\"));
		const CStringA xmlSource = (CStringA)strPath + "Webmap.xml";
		tinyxml2::XMLDocument xml;
		if(xml.LoadFile(xmlSource) == tinyxml2::XMLError::XML_SUCCESS)
		{
			tinyxml2::XMLElement* pElement = xml.FirstChildElement("Map");
			if(pElement != nullptr)
			{
				pElement->SetAttribute("Name", m_Datainfo.m_strName);
				switch(nId)
				{
					case ID_RELEASEWEB_VECTORPAGE:
						pElement->SetAttribute("Type", "VectorPage");
						break;
					case ID_RELEASEWEB_VECTORTILE:
						pElement->SetAttribute("Type", "VectorTile");
						break;
					case ID_RELEASEWEB_RASTERTILE:
						pElement->SetAttribute("Type", "RasterTile");
						break;
				}
			}
			const CString xmlTarget = strFolder + _T('\\') + strName + _T(".xml");
			if(xml.SaveFile(CStringA(xmlTarget)) != tinyxml2::XMLError::XML_SUCCESS)
			{
				CString strMessage;
				strMessage.Format(_T("Failed to save file %s.xml into folder %s"), strName, strFolder);
				AfxMessageBox(strMessage);
			}
		}
		else
		{
			CString strMessage;
			strMessage.Format(_T("File Webmap.xml is not found in folder %s"), strPath);
			AfxMessageBox(strMessage);
		}
	}
}
Gdiplus::Bitmap* CGrfDoc::CreateThumbnail(int width, int height)
{
	const CRectF mapRect = m_Datainfo.GetMapRect();
	const double mapWidth = mapRect.Width();
	const double mapHeight = mapRect.Height();
	const float factorMapToView = min(width / mapWidth, height / mapHeight);

	CViewMonitor viewinfo(m_Datainfo);
	viewinfo.m_bViewer = false;

	viewinfo.m_sizeView.cx = width;
	viewinfo.m_sizeView.cy = height;

	viewinfo.m_xFactorMapToView = factorMapToView;
	viewinfo.m_yFactorMapToView = factorMapToView;
	viewinfo.m_scaleCurrent = m_Datainfo.m_scaleSource;
	viewinfo.m_scaleCurrent = CDatainfo::GetScaleFromMapToViewRatio(m_Datainfo.m_pProjection, factorMapToView, mapRect.CenterPoint(), viewinfo.m_sizeDPI);
	viewinfo.m_levelCurrent = log(max(viewinfo.m_sizeView.cx, viewinfo.m_sizeView.cy) / 256.f) / log(2.f);
	viewinfo.m_ptViewPos.x = -(width - mapWidth * factorMapToView) / 2.f;
	viewinfo.m_ptViewPos.y = +(height - mapHeight * factorMapToView) / 2.f;

	Gdiplus::Bitmap* bitmap = ::new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);
	if(bitmap->GetLastStatus() != Gdiplus::Status::Ok)
		return nullptr;

	Gdiplus::Graphics g(bitmap);
	g.SetPageUnit(Gdiplus::Unit::UnitPixel);
	g.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
	g.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQualityBicubic);
	g.SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHighQuality);
	g.SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintAntiAlias);

	g.Clear(Gdiplus::Color::Transparent);
	CRect docRect = m_Datainfo.GetDocRect();
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_minLevelObj == 0)
		{
			layer->m_minLevelObj = -1;
		}
	}
	this->Draw(nullptr, viewinfo, g, docRect);
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_minLevelObj == -1)
		{
			layer->m_minLevelObj = 0;
		}
	}
	return bitmap;
}
void CGrfDoc::OnReleaseWEBVectorTile()
{
	DWORD dwKey = AfxGetAuthorizationKey();
	if((dwKey & LICENCE::MAPINTERNET) != LICENCE::MAPINTERNET)
	{
		CLicenceDlg::Display();
		return;
	}

	CString strPath = COleDocument::GetPathName();
	strPath = strPath.Left(strPath.ReverseFind('\\'));
	CString strName = COleDocument::GetPathName();
	strName = strName.Left(strName.ReverseFind(_T('.')));
	strName = strName.Mid(strName.ReverseFind('\\') + 1);
	if(strName.IsEmpty())
		strName = m_Datainfo.m_strName;
	if(strName.IsEmpty())
		strName = _T("Untitle");
	CString strFile;

	BYTE minMapLevel = 0;
	BYTE maxMapLevel = 1;
	BYTE minOutLevel = 0;
	BYTE maxOutLevel = 1;
	float srcMapLevel = 0;
	unsigned int mapUid;


	std::string cdata;
	pbf::writer writer(cdata);
	DWORD dwVersion = AfxGetCurrentArVersion();
	writer.add_fixed_uint32(dwVersion);
	m_Datainfo.ReleaseWeb(writer, (pbf::tag)1, minMapLevel, maxMapLevel, srcMapLevel);
	m_frame.ReleaseWeb(writer, (pbf::tag)2);
	m_layertree.ReleaseWvt(writer, (pbf::tag)3, m_Datainfo, minMapLevel, maxMapLevel);

	strFile = strName + _T(".wvtpbf");
	bool bToWeb = false;
	bool bFailed = false;
	CPublishToWebDlg dlg(AfxGetMainWnd(), m_Datainfo.m_strName, _T(""), minMapLevel, maxMapLevel);
	if(dlg.DoModal() == IDOK)
	{
		AfxGetApp()->BeginWaitCursor();

		minMapLevel = (dlg.m_bUpdate && dlg.m_bPartial) ? minMapLevel : max(dlg.m_minLevel, minMapLevel);
		maxMapLevel = (dlg.m_bUpdate && dlg.m_bPartial) ? maxMapLevel : min(dlg.m_maxLevel, maxMapLevel);
		minOutLevel = (dlg.m_bUpdate && dlg.m_bPartial) ? dlg.m_minLevel : minMapLevel;
		maxOutLevel = (dlg.m_bUpdate && dlg.m_bPartial) ? dlg.m_maxLevel : maxMapLevel;

		auto failed = [&bFailed](CString message)
		{
			bFailed = true;
			AfxMessageBox(message);
		};
		auto success = [&bFailed, &bToWeb, &mapUid](std::string response)
		{
			boost::json::value json = boost::json::parse(response);
			if(json.is_string()) {
				AfxMessageBox(CString(json.as_string().c_str()));
				bFailed = true;
			}
			else if(json.is_object()) {
				boost::json::object object = json.as_object();
				if(object.contains("success"))
				{
					auto success = object.at("success");
					if(success != nullptr && success.as_bool())
					{
						boost::json::object data = object.at("data").as_object();
						mapUid = data.at("UID").as_int64();
					}
					else if(object.contains("message"))
					{
						AfxMessageBox(CString(object.at("message").as_string().c_str()));
						bFailed = true;
					}
					else
					{
						bFailed = true;
					}
				}
				else if(object.contains("message"))
				{
					AfxMessageBox(CString(object.at("message").as_string().c_str()));
					bFailed = true;
				}
				else
				{
					bFailed = true;
				}
			}
			bToWeb = true;
		};
		char* blob = Base64_Encode((char*)cdata.c_str(), cdata.length());
		std::string payload = CGrfDoc::CreateDiwatuEntity(dlg.m_bUpdate, "VectorTile", dlg.m_strName, blob, minMapLevel, maxMapLevel, srcMapLevel, false, false, dlg.m_strDescription, (dlg.m_bUpdate && dlg.m_bPartial) ? minOutLevel : (std::optional<int>)std::nullopt, (dlg.m_bUpdate && dlg.m_bPartial) ? maxOutLevel : (std::optional<int>)std::nullopt);
		CWebService::CallService(std::string(CT2A(dlg.m_strAPIUrl + (dlg.m_bUpdate ? _T("Update_Basemap2") : _T("Create_Basemap")))), dlg.m_strClient, dlg.m_strUsername, dlg.m_strPassword, payload, failed, success);
		::free(blob);

		AfxGetApp()->EndWaitCursor();
	}
	else if(((CWinAppEx*)AfxGetApp())->GetShellManager()->BrowseForFolder(strPath, nullptr, strPath, _T("Choose a folder where you want to output to:")) == TRUE)
	{
		if(PathIsDirectory(strPath) == TRUE && DeleteDirectory(strPath) == FALSE)
			return;
		if(PathIsDirectory(strPath) == FALSE)
			CreateDirectory(strPath, nullptr);

		CFileException ex;
		CFile file;
		if(file.Open(strPath + _T("\\") + strFile, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone, &ex) == TRUE)
		{
			AfxGetApp()->BeginWaitCursor();

			file.Write(cdata.c_str(), cdata.length());
			file.Close();

			ReleaseWEBCap(ID_RELEASEWEB_VECTORTILE, strPath, strName);

			AfxGetApp()->EndWaitCursor();
		}
		else
			return;
	}
	else
		return;

	if(bFailed == false)
	{
		CRectF mapRect = m_Datainfo.GetMapRect();
		double mapWidth = mapRect.Width();
		double mapHeight = mapRect.Height();
		double mapMax = max(mapWidth, mapHeight);
		CRectF mapSquare = mapRect;
		mapSquare.InflateRect((mapMax - mapWidth) / 2.f, (mapMax - mapHeight) / 2.f);

		HINSTANCE hInst = ::LoadLibrary(_T("Public.dll"));
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		AfxSetResourceHandle(hInst);

		CProgressDlg progressDlg(nullptr);
		progressDlg.Create(IDD_PROGRESSBAR, nullptr);
		progressDlg.ShowWindow(SW_SHOW);
		int totalCalls = 0;
		for(int level = minOutLevel; level <= maxOutLevel; level++)
		{
			int tileCount = pow(2.0, level);
			double mapTile = mapMax / tileCount;
			for(int row = 0; row < tileCount; row++)
			{
				if(mapSquare.bottom - mapTile * row >= mapRect.bottom + mapTile)
					continue;
				if(mapSquare.bottom - mapTile * row <= mapRect.top)
					continue;
				for(int col = 0; col < tileCount; col++)
				{
					if(mapSquare.left + mapTile * col + mapTile <= mapRect.left)
						continue;
					else if(mapSquare.left + mapTile * col >= mapRect.right)
						continue;
					else
						totalCalls++;
				}
			}
		}
		progressDlg.SetRange(0, totalCalls);

		int requested = 0;
		int succeeded = 0;
		CViewMonitor viewinfo(m_Datainfo);
		viewinfo.m_bViewer = false;
		for(int level = minOutLevel; level <= maxOutLevel; level++)
		{
			CString folderLevel;
			folderLevel.Format(_T("%s\\%d"), strPath, level);

			viewinfo.m_sizeView.cx = 256 * pow(2, level) * (mapWidth / mapMax);
			viewinfo.m_sizeView.cy = 256 * pow(2, level) * (mapHeight / mapMax);

			int tileCount = pow(2.0, level);
			float factorMapToView = 256.f * pow(2, level) / mapMax;
			viewinfo.m_xFactorMapToView = factorMapToView;
			viewinfo.m_yFactorMapToView = factorMapToView;
			viewinfo.m_scaleCurrent = CDatainfo::GetScaleFromMapToViewRatio(m_Datainfo.m_pProjection, factorMapToView, mapRect.CenterPoint(), CSize(72, 72));
			viewinfo.m_levelCurrent = log(max(viewinfo.m_sizeView.cx, viewinfo.m_sizeView.cy) / 256) / log(2.0);

			int minCol = (int)((mapRect.left - mapSquare.left) * factorMapToView) / 256;
			int maxCol = (int)((mapRect.right - mapSquare.left) * factorMapToView) / 256;
			int minRow = (int)((mapSquare.bottom - mapRect.bottom) * factorMapToView) / 256;
			int maxRow = (int)((mapSquare.bottom - mapRect.top) * factorMapToView) / 256;
			minCol = max(0, min(minCol, tileCount - 1));
			maxCol = max(0, min(maxCol, tileCount - 1));
			minRow = max(0, min(minRow, tileCount - 1));
			maxRow = max(0, min(maxRow, tileCount - 1));

			int stepRows = min(25, maxRow - minRow + 1);
			int stepCols = min(25, maxCol - minCol + 1);

			HDC hdc = ::CreateCompatibleDC(nullptr);
			BITMAPINFO* pbmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD));
			pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			pbmi->bmiHeader.biWidth = stepCols * 256 + 100;
			pbmi->bmiHeader.biHeight = stepRows * 256 + 100;
			pbmi->bmiHeader.biPlanes = 1;
			pbmi->bmiHeader.biBitCount = 1;
			pbmi->bmiHeader.biCompression = BI_RGB;
			pbmi->bmiHeader.biClrUsed = 2;
			pbmi->bmiColors[0].rgbBlue = 0x00;
			pbmi->bmiColors[0].rgbGreen = 0x00;
			pbmi->bmiColors[0].rgbRed = 0x00;
			pbmi->bmiColors[1].rgbBlue = 0xFF;
			pbmi->bmiColors[1].rgbGreen = 0xFF;
			pbmi->bmiColors[1].rgbRed = 0xFF;
			//HBITMAP hBitmap = ::CreateBitmap(stepCols*256+100, stepRows*256+100, 1, 1, nullptr);// same: ::CreateCompatibleBitmap(hdc, cx, cy);		::CreateDIBitmap(hdc, &bih, CBM_INIT, nullptr, &bmi, 0); these don't work to have access of the raw byts			
			HBITMAP hBitmap = ::CreateDIBSection(hdc, pbmi, 0, nullptr, nullptr, 0);
			const HANDLE oldBmp = ::SelectObject(hdc, hBitmap);

			viewinfo.m_ptViewPos.x = -(mapRect.left - mapSquare.left) * factorMapToView;
			viewinfo.m_ptViewPos.y = -(mapSquare.bottom - mapRect.bottom) * factorMapToView;
			int staRow = minRow;
			do
			{
				int midRow = min(staRow + stepRows - 1, maxRow);
				int staCol = minCol;

				do {
					if(true)
					{
						BITMAP bitmap;
						::GetObject(hBitmap, sizeof(BITMAP), &bitmap);
						BitBlt(hdc, 0, 0, bitmap.bmWidth, 100, hdc, 0, bitmap.bmHeight - 100, SRCCOPY);
						BitBlt(hdc, 0, 0, 100, bitmap.bmHeight, hdc, bitmap.bmWidth - 100, 0, SRCCOPY);
						CViewMonitor viewinfo2(m_Datainfo);
						viewinfo2 = viewinfo;
						viewinfo2.m_ptViewPos.x = staCol * 256 + viewinfo.m_ptViewPos.x - 100;
						viewinfo2.m_ptViewPos.y = staRow * 256 + viewinfo.m_ptViewPos.y - 100;
						CRect inRect(staCol * 256, staRow * 256, min(staCol + stepCols, maxCol + 1) * 256, min(staRow + stepRows, maxRow + 1) * 256);
						inRect = viewinfo.ClientToDoc(inRect);

						Gdiplus::Graphics* g = Gdiplus::Graphics::FromHDC(hdc);
						g->SetPageUnit(Gdiplus::Unit::UnitPixel);
						g->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeNone);
						//g->Clear(Gdiplus::Color::Transparent);
						Gdiplus::SolidBrush brush(Gdiplus::Color::Black);
						g->FillRectangle(&brush, 100, 100, bitmap.bmWidth - 100, bitmap.bmHeight - 100);
						monochrome::SaveHBITMAPToFile(hBitmap, _T("c:\\2.bmp"));

						for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
						{
							CLayer* layer = *it;
							layer->Alreadyin(*g, viewinfo2, inRect);
						}
						for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
						{
							CLayer* layer = *it;
							layer->Squeezein(*g, viewinfo2, inRect);
						}
						for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
						{
							CLayer* layer = *it;
							layer->Preoccupy(*g, viewinfo2, inRect);
						}
						for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
						{
							CLayer* layer = *it;
							layer->PositeTag(*g, viewinfo2, inRect, true);
						}

						delete g;
					}
					int midCol = min(staCol + stepCols - 1, maxCol);
					CVectorTile** matrixTiles = new CVectorTile * [midCol - staCol + 1];
					for(int col = staCol; col <= midCol; col++)
					{
						matrixTiles[col - staCol] = new CVectorTile[midRow - staRow + 1];
					}
					for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
					{
						CLayer* layer = *it;
						layer->Tilize(matrixTiles, staRow, midRow, staCol, midCol, viewinfo);
					}

					for(int col = staCol; col <= midCol; col++)
					{
						for(int row = staRow; row <= midRow; row++)
						{
							CSize offset(col * 256, row * 256);
							std::string cdata = matrixTiles[col - staCol][row - staRow].Extract(offset, dwVersion);
							if(cdata.empty() == true)
								continue;

							if(bToWeb == true)
							{
								auto failed = [](CString message)
								{
									AfxMessageBox(message);
								};
								auto success = [&succeeded, &progressDlg](std::string response)
								{
									succeeded++;
									progressDlg.UpdateProgressPosition(succeeded);
								};

								char* blob = Base64_Encode((char*)cdata.c_str(), cdata.length());
								std::stringstream fmt;
								fmt << "{";
								fmt << "\"mapUid\": ";
								fmt << mapUid;
								fmt << ",";
								fmt << "\"type\": ";
								fmt << 2;
								fmt << ",";
								fmt << "\"level\": ";
								fmt << level;
								fmt << ",";
								fmt << "\"row\": ";
								fmt << row;
								fmt << ",";
								fmt << "\"col\": ";
								fmt << col;
								fmt << ",";
								fmt << "\"blob\": \"";
								fmt << blob;
								fmt << "\"";
								fmt << "}";
								std::string payload = fmt.str();
								CWebService::CallService(std::string(CT2A(dlg.m_strAPIUrl + _T("Create_Maptile"))), dlg.m_strClient, dlg.m_strUsername, dlg.m_strPassword, payload, failed, success);
								::free(blob);
							}
							else
							{
								if(PathIsDirectory(folderLevel) == FALSE)
									CreateDirectory(folderLevel, nullptr);
								CString folderRow;
								folderRow.Format(_T("%s\\%d"), folderLevel, row);
								if(PathIsDirectory(folderRow) == FALSE)
									CreateDirectory(folderRow, nullptr);

								CString strTileFile;
								strTileFile.Format(_T("%s\\%d.wvtpbf"), folderRow, col);
								CFile file;
								if(file.Open(strTileFile, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
								{
									file.Write(cdata.c_str(), cdata.length());
									file.Close();
								}
							}
						}

						delete[] matrixTiles[col - staCol];
						matrixTiles[col - staCol] = nullptr;
					}
					delete[] matrixTiles;
					matrixTiles = nullptr;

					staCol = midCol + 1;

					monochrome::SaveHBITMAPToFile(hBitmap, _T("c:\\1.bmp"));
				} while(staCol <= maxCol);

				staRow = midRow + 1;
			} while(staRow <= maxRow);

			::SelectObject(hdc, oldBmp);
			::DeleteDC(hdc);

			::DeleteObject(hBitmap);
			::free(pbmi);
		}
		progressDlg.DestroyWindow();
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);

		if(bToWeb == false)
		{
		}
		else if(requested == 0)
		{
		}
		else if(requested == succeeded)
			AfxMessageBox(_T("Successfully uploaded to web server!"));
		else
			AfxMessageBox(_T("Partial tiles failed to uploaded to web server!"));
	}
}
void CGrfDoc::OnReleaseWEBRasterTile()
{
	DWORD dwKey = AfxGetAuthorizationKey();
	if((dwKey & LICENCE::MAPINTERNET) != LICENCE::MAPINTERNET)
	{
		CLicenceDlg::Display();
		return;
	}

	CString extPng = _T("image/png");
	extPng.MakeLower();
	BSTR bstrPng = extPng.AllocSysString();
	CLSID pngClsid;
	if(CPrintTool::GetEncoderClsid(bstrPng, &pngClsid) < 0)
	{
		::SysFreeString(bstrPng);
		return;
	}
	::SysFreeString(bstrPng);

	CString strPath = COleDocument::GetPathName();
	strPath = strPath.Left(strPath.ReverseFind('\\'));
	CString strName = COleDocument::GetPathName();
	strName = strName.Left(strName.ReverseFind(_T('.')));
	strName = strName.Mid(strName.ReverseFind('\\') + 1);
	if(strName.IsEmpty())
		strName = m_Datainfo.m_strName;
	if(strName.IsEmpty())
		strName = _T("Untitle");
	CString strFile;

	BYTE minMapLevel = 0;
	BYTE maxMapLevel = 1;
	BYTE minOutLevel = 0;
	BYTE maxOutLevel = 1;
	float srcMapLevel = 1;
	unsigned int mapUid;

	std::string blob;
	pbf::writer writer(blob);
	DWORD dwVersion = AfxGetCurrentArVersion();
	writer.add_fixed_uint32(dwVersion);
	m_Datainfo.ReleaseWeb(writer, (pbf::tag)1, minMapLevel, maxMapLevel, srcMapLevel);

	strFile = strName + _T(".wrtpbf");
	bool bToWeb = false;
	bool bFailed = false;
	CPublishToWebDlg dlg(AfxGetMainWnd(), m_Datainfo.m_strName, _T(""), minMapLevel, maxMapLevel);
	if(dlg.DoModal() == IDOK)
	{
		AfxGetApp()->BeginWaitCursor();

		minMapLevel = (dlg.m_bUpdate && dlg.m_bPartial) ? minMapLevel : max(dlg.m_minLevel, minMapLevel);
		maxMapLevel = (dlg.m_bUpdate && dlg.m_bPartial) ? maxMapLevel : min(dlg.m_maxLevel, maxMapLevel);
		minOutLevel = (dlg.m_bUpdate && dlg.m_bPartial) ? dlg.m_minLevel : minMapLevel;
		maxOutLevel = (dlg.m_bUpdate && dlg.m_bPartial) ? dlg.m_maxLevel : maxMapLevel;

		auto failed = [&bFailed](CString message)
		{
			AfxMessageBox(message);
			bFailed = true;
		};
		auto success = [&bFailed, &bToWeb, &mapUid](std::string response)
		{
			boost::json::value json = boost::json::parse(response);
			if(json.is_string()) {
				AfxMessageBox(CString(json.as_string().c_str()));
				bFailed = true;
			}
			else if(json.is_object()) {
				boost::json::object object = json.as_object();
				if(object.contains("success"))
				{
					auto success = object.at("success");
					if(success != nullptr && success.as_bool())
					{
						boost::json::object data = object.at("data").as_object();
						mapUid = data.at("UID").as_int64();
					}
					else
					{
						bFailed = true;
					}
				}
				else
				{
					bFailed = true;
				}
			}
			bToWeb = true;
		};
		const std::string payload = CGrfDoc::CreateDiwatuEntity(dlg.m_bUpdate, "RasterTile", dlg.m_strName, Base64_Encode((char*)blob.c_str(), blob.length()), minMapLevel, maxMapLevel, srcMapLevel, false, false, dlg.m_strDescription, (dlg.m_bUpdate && dlg.m_bPartial) ? dlg.m_minLevel : (std::optional<int>)std::nullopt, (dlg.m_bUpdate && dlg.m_bPartial) ? dlg.m_minLevel : (std::optional<int>)std::nullopt);
		CWebService::CallService(std::string(CT2A(dlg.m_strAPIUrl + (dlg.m_bUpdate ? _T("Update_Basemap2") : _T("Create_Basemap")))), dlg.m_strClient, dlg.m_strUsername, dlg.m_strPassword, payload, failed, success);

		AfxGetApp()->EndWaitCursor();
	}
	else if(((CWinAppEx*)AfxGetApp())->GetShellManager()->BrowseForFolder(strPath, nullptr, strPath, _T("Choose a folder where you want to output to:")) == TRUE)
	{
		if(PathIsDirectory(strPath) == TRUE && DeleteDirectory(strPath) == FALSE)
			return;
		if(PathIsDirectory(strPath) == FALSE)
			CreateDirectory(strPath, nullptr);

		CFileException ex;
		CFile file;
		if(file.Open(strPath + _T("\\") + strFile, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone, &ex) == TRUE)
		{
			file.Write(blob.c_str(), blob.length());
			file.Close();

			ReleaseWEBCap(ID_RELEASEWEB_RASTERTILE, strPath, strName);
		}
		else
			return;
	}
	else
		return;

	if(bFailed == false)
	{
		AfxGetApp()->BeginWaitCursor();

		CRectF mapRect = m_Datainfo.GetMapRect();
		double mapWidth = mapRect.Width();
		double mapHeight = mapRect.Height();
		double mapMax = max(mapWidth, mapHeight);
		CRectF mapSquare = mapRect;
		mapSquare.InflateRect((mapMax - mapWidth) / 2.f, (mapMax - mapHeight) / 2.f);

		HINSTANCE hInst = ::LoadLibrary(_T("Public.dll"));
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		AfxSetResourceHandle(hInst);

		CProgressDlg progressDlg(nullptr);
		progressDlg.Create(IDD_PROGRESSBAR, nullptr);
		progressDlg.ShowWindow(SW_SHOW);
		int totalCalls = 0;
		for(int level = minOutLevel; level <= maxOutLevel; level++)
		{
			int tileCount = pow(2.0, level);
			double mapTile = mapMax / tileCount;
			for(int row = 0; row < tileCount; row++)
			{
				if(mapSquare.bottom - mapTile * row >= mapRect.bottom + mapTile)
					continue;
				if(mapSquare.bottom - mapTile * row <= mapRect.top)
					continue;
				for(int col = 0; col < tileCount; col++)
				{
					if(mapSquare.left + mapTile * col + mapTile <= mapRect.left)
						continue;
					else if(mapSquare.left + mapTile * col >= mapRect.right)
						continue;
					else
						totalCalls++;
				}
			}
		}
		progressDlg.SetRange(0, totalCalls);

		int requested = 0;
		int succeeded = 0;
		CViewMonitor viewinfo(m_Datainfo);
		viewinfo.m_bViewer = false;
		for(int level = minOutLevel; level <= maxOutLevel; level++)
		{
			CString folderLevel;
			folderLevel.Format(_T("%s\\%d"), strPath, level);

			viewinfo.m_sizeView.cx = 256 * pow(2, level);
			viewinfo.m_sizeView.cy = 256 * pow(2, level);

			int tileCount = pow(2.0, level);
			double mapTile = mapMax / tileCount;
			float factorMapToView = 256.f * pow(2, level) / mapMax;
			viewinfo.m_xFactorMapToView = factorMapToView;
			viewinfo.m_yFactorMapToView = factorMapToView;
			viewinfo.m_scaleCurrent = CDatainfo::GetScaleFromMapToViewRatio(m_Datainfo.m_pProjection, factorMapToView, mapRect.CenterPoint(), CSize(72, 72));
			viewinfo.m_levelCurrent = log(max(viewinfo.m_sizeView.cx, viewinfo.m_sizeView.cy) / 256.f) / log(2.f);

			Gdiplus::Bitmap* bitmap = nullptr;
			for(int i = level; i >= 0; i--)
			{
				int pixels = pow(2.0, i) * 256;
				Gdiplus::Bitmap* btp = ::new Gdiplus::Bitmap(pixels, 256, PixelFormat32bppARGB);
				if(btp == nullptr)
					continue;
				else if(btp->GetLastStatus() == Gdiplus::Status::Ok)
				{
					bitmap = btp;
					break;
				}
			}
			if(bitmap == nullptr)
				continue;

			Gdiplus::Graphics g(bitmap);
			g.SetPageUnit(Gdiplus::Unit::UnitPixel);
			g.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
			g.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQualityBicubic);
			g.SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHighQuality);
			g.SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintAntiAlias);

			for(int row = 0; row < tileCount; row++)
			{
				if(mapSquare.bottom - mapTile * row >= mapRect.bottom + mapTile)
					continue;
				else if(mapSquare.bottom - mapTile * row <= mapRect.top)
					continue;

				CString folderRow;
				folderRow.Format(_T("%s\\%d"), folderLevel, row);

				double tileTop = mapTile * row;
				viewinfo.m_ptViewPos.y = tileTop * factorMapToView + (mapMax - mapHeight) * factorMapToView / 2.f;
				for(int stripX = 0; stripX < tileCount; stripX += bitmap->GetWidth() / 256)
				{
					double stripLeft = mapTile * stripX;
					viewinfo.m_ptViewPos.x = stripLeft * factorMapToView + (mapMax - mapWidth) * factorMapToView / 2.f;

					CRectF mapStripRect(CPointF(mapSquare.left + mapTile * stripX, mapSquare.bottom - mapTile * row), CSizeF(mapTile * bitmap->GetWidth() / 256, -mapTile));
					mapStripRect.NormalizeRect();

					if(mapStripRect.IntersectWith(mapRect) == false)
						continue;

					g.Clear(Gdiplus::Color::Transparent);
					CRect docStripRect = m_Datainfo.MapToDoc(mapStripRect);
					this->Draw(nullptr, viewinfo, g, docStripRect);

					for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
					{
						CLayer* layer = *it;
						layer->DrawTag(g, viewinfo, docStripRect, true, true);
					}
					for(int col = stripX; col < stripX + bitmap->GetWidth() / 256; col++)
					{
						if(mapSquare.left + mapTile * col + mapTile <= mapRect.left)
							continue;
						else if(mapSquare.left + mapTile * col >= mapRect.right)
							continue;

						Gdiplus::Bitmap* tile = bitmap->Clone((col - stripX) * 256, 0, 256, 256, PixelFormat32bppARGB);
						if(tile == nullptr)
						{

						}
						else if(bToWeb)
						{
							char* string64 = CWebService::BitmapToString64(tile);
							std::map<CString, CString> files;
							std::map<CStringA, const BYTE*> bytes;
							requested++;
							auto failed = [](CString message)
							{
							};
							auto success = [&succeeded, &progressDlg](std::string response)
							{
								succeeded++;
								progressDlg.UpdateProgressPosition(succeeded);
							};

							std::stringstream fmt;
							fmt << "{";
							fmt << "\"mapUid\": ";
							fmt << mapUid;
							fmt << ",";
							fmt << "\"type\": ";
							fmt << 3;
							fmt << ",";
							fmt << "\"level\": ";
							fmt << level;
							fmt << ",";
							fmt << "\"row\": ";
							fmt << row;
							fmt << ",";
							fmt << "\"col\": ";
							fmt << col;
							fmt << ",";
							fmt << "\"blob\": \"";
							fmt << string64;
							fmt << "\"";
							fmt << "}";
							std::string payload = fmt.str();
							CWebService::CallService(std::string(CT2A(dlg.m_strAPIUrl + _T("Create_MapTile"))), dlg.m_strClient, dlg.m_strUsername, dlg.m_strPassword, payload, failed, success);
							free(string64);
						}
						else
						{
							if(PathIsDirectory(folderLevel) == FALSE)
								CreateDirectory(folderLevel, nullptr);
							if(PathIsDirectory(folderRow) == FALSE)
								CreateDirectory(folderRow, nullptr);
							CString strFile;
							strFile.Format(_T("%s\\%d.png"), folderRow, col);

							BSTR bstrFile = strFile.AllocSysString();
							tile->Save(bstrFile, &pngClsid);
							::SysFreeString(bstrFile);
						}

						delete tile;
						tile = nullptr;
					}
				}
			}
			::delete bitmap;
			bitmap = nullptr;
		}
		progressDlg.DestroyWindow();
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);

		AfxGetApp()->BeginWaitCursor();
		if(bToWeb == false)
		{
		}
		else if(requested == 0)
		{
		}
		else if(requested == succeeded)
		{
			AfxMessageBox(_T("Successfully uploaded to web server!"));
		}
		else
		{
			AfxMessageBox(_T("Partial tiles failed to uploaded to web server!"));
		}
	}
}
void CGrfDoc::OnReleaseWEBVectorPage()
{
	DWORD dwKey = AfxGetAuthorizationKey();
	if((dwKey & LICENCE::MAPINTERNET) != LICENCE::MAPINTERNET)
	{
		CLicenceDlg::Display();
		return;
	}

	CString strPath = COleDocument::GetPathName();
	strPath = strPath.Left(strPath.ReverseFind('\\'));
	CString strName = CString(COleDocument::GetPathName());
	strName = strName.Left(strName.ReverseFind(_T('.')));
	strName = strName.Mid(strName.ReverseFind('\\') + 1);
	if(strName.IsEmpty())
		strName = m_Datainfo.m_strName;
	if(strName.IsEmpty())
		strName = _T("Untitle");
	CString strFile = strName;


	AfxGetApp()->BeginWaitCursor();
	BYTE minMapLevel = 0;
	BYTE maxMapLevel = 5;
	BYTE minTagLevel = 0;
	BYTE maxTagLevel = 5;
	//	m_layertree.MinMaxLevel(minMapLevel, maxMapLevel, minTagLevel, maxTagLevel);

	float srcMapLevel = 0;

	auto createBlob = [&](std::string& blob, bool bExportProperty)
	{
		pbf::writer writer(blob);
		DWORD dwVersion = AfxGetCurrentArVersion();
		writer.add_fixed_uint32(dwVersion);
		m_Datainfo.ReleaseWeb(writer, (pbf::tag)1, minMapLevel, maxMapLevel, srcMapLevel);
		m_frame.ReleaseWeb(writer, (pbf::tag)2);
		m_layertree.ReleaseWeb(writer, (pbf::tag)3, m_Datainfo, minMapLevel, maxMapLevel, bExportProperty);
		if(m_topolist.size() > 0)
		{
			std::string cdata;
			pbf::writer child(cdata);
			for(std::list<CTopology*>::iterator it = m_topolist.begin(); it != m_topolist.end(); it++)
			{
				CTopology* pTopology = *it;
				pTopology->ReleaseWeb(child);
			}
			writer.add_block((pbf::tag)4, cdata.c_str(), cdata.length());
		}
	};


	strFile = strName + _T(".wvppbf");
	AfxGetApp()->EndWaitCursor();
	CPublishToWebDlg dlg(AfxGetMainWnd(), m_Datainfo.m_strName, _T(""), minMapLevel, maxMapLevel, true);
	if(dlg.DoModal() == IDOK)
	{
		AfxGetApp()->BeginWaitCursor();

		minMapLevel = dlg.m_minLevel;
		maxMapLevel = dlg.m_maxLevel;
		std::string blob;
		createBlob(blob, dlg.m_bExportProperty);

		bool withLngLat = (this->m_Datainfo.m_pProjection != nullptr);
		bool withGeoCode = (this->m_layertree.m_GeoDBList.size() > 0 || this->m_layertree.m_root.SuportGeoCode());	//	frame.c_str(), 
		std::map<CString, std::variant<std::string, int>> parameters;
		std::map<CString, CString> files;
		std::string payload = CGrfDoc::CreateDiwatuEntity(dlg.m_bUpdate, "VectorPage", dlg.m_strName, Base64_Encode(blob.c_str(), blob.size()), minMapLevel, maxMapLevel, srcMapLevel, withLngLat, withGeoCode, dlg.m_strDescription, (std::optional<int>)std::nullopt, (std::optional<int>)std::nullopt);
		auto failed = [](const CString message)
		{
			AfxGetApp()->EndWaitCursor();
			AfxMessageBox(message);
		};
		auto success = [](const std::string response)
		{
			AfxGetApp()->EndWaitCursor();
			boost::json::value json = boost::json::parse(response);
			if(json.is_string()) {
				AfxMessageBox(CString(json.as_string().c_str()));
			}
			else if(json.is_object()) {
				boost::json::object object = json.as_object();
				if(object.contains("success"))
				{
					auto success = object.at("success");
					if(success != nullptr && success.as_bool()) {

					}
					else if(object.contains("message"))
					{
						AfxMessageBox(CString(object.at("message").as_string().c_str()));
					}
				}
				else if(object.contains("message"))
				{
					AfxMessageBox(CString(object.at("message").as_string().c_str()));
				}
			}
		};
		CWebService::CallService(std::string(CT2A(dlg.m_strAPIUrl + (dlg.m_bUpdate ? _T("Update_Basemap2") : _T("Create_Basemap")))), dlg.m_strClient, dlg.m_strUsername, dlg.m_strPassword, payload, failed, success);

		AfxGetApp()->EndWaitCursor();
	}
	else if(((CWinAppEx*)AfxGetApp())->GetShellManager()->BrowseForFolder(strPath, nullptr, strPath, _T("Choose a folder where you want to output to:")) == TRUE)
	{
		CFileException ex;
		CFile file;
		if(file.Open(strPath + _T("\\") + strFile, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone, &ex) == TRUE)
		{
			AfxGetApp()->BeginWaitCursor();

			std::string blob;
			createBlob(blob, dlg.m_bExportProperty);

			file.Write(blob.c_str(), blob.length());
			file.Close();
			ReleaseWEBCap(ID_RELEASEWEB_VECTORPAGE, strPath, strName);
			AfxGetApp()->EndWaitCursor();
		}
	}
	else
	{
	}
}
void CGrfDoc::ReleaseWEBVectorPage(CString strName, bool diwatu)
{
	BYTE minMapLevel = 0;
	BYTE maxMapLevel = 1;
	float srcMapLevel = 0;

	std::string blob;
	pbf::writer writer(blob);
	DWORD dwVersion = AfxGetCurrentArVersion();
	writer.add_fixed_uint32(dwVersion);
	m_Datainfo.ReleaseWeb(writer, (pbf::tag)1, minMapLevel, maxMapLevel, srcMapLevel);
	m_frame.ReleaseWeb(writer, (pbf::tag)2);
	m_layertree.ReleaseWeb(writer, (pbf::tag)3, m_Datainfo, minMapLevel, maxMapLevel, true);
	if(m_topolist.size() > 0)
	{
		std::string cdata;
		pbf::writer child(cdata);
		for(std::list<CTopology*>::iterator it = m_topolist.begin(); it != m_topolist.end(); it++)
		{
			CTopology* pTopology = *it;
			pTopology->ReleaseWeb(child);
		}
		writer.add_block((pbf::tag)4, cdata.c_str(), cdata.length());
	}
	if(diwatu == true)
	{
		minMapLevel = 1;
		maxMapLevel = 5;
		const bool withLngLat = (this->m_Datainfo.m_pProjection != nullptr);
		const bool withGeoCode = (this->m_layertree.m_GeoDBList.size() > 0 || this->m_layertree.m_root.SuportGeoCode());
		const std::string payload = CGrfDoc::CreateDiwatuEntity(true, "VectorPage", strName, blob.c_str(), minMapLevel, maxMapLevel, srcMapLevel, withLngLat, withGeoCode, _T(""), (std::optional<int>)std::nullopt, (std::optional<int>)std::nullopt);
		auto failed = [](const CString message)
		{
			AfxGetApp()->EndWaitCursor();
			AfxMessageBox(message);
		};
		auto success = [](const std::string response)
		{
			AfxGetApp()->EndWaitCursor();
		};
		CWebService::CallService(std::string(CT2A(_T("https://api.diwatu.com/Map.svc/Update_Basemap2"))), _T(""), _T("Honey Plug"), _T("3"), payload, failed, success);
	}
	else
	{
		CFileException ex;
		CFile file;
		if(file.Open(strName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone, &ex) == TRUE)
		{
			AfxGetApp()->BeginWaitCursor();

			file.Write(blob.c_str(), blob.length());
			file.Close();
			AfxGetApp()->EndWaitCursor();
		}
	}
}
std::string CGrfDoc::Convert()
{
	BYTE minMapLevel;
	BYTE maxMapLevel;
	float srcMapLevel;

	boost::json::object json;
	m_Datainfo.ReleaseWeb(json, minMapLevel, maxMapLevel, srcMapLevel);
	m_layertree.ReleaseWeb(json, m_Datainfo, minMapLevel, maxMapLevel, false);
	return boost::json::serialize(json);
}

void CGrfDoc::Transform(double m11, double m21, double m31, double m12, double m22, double m32)
{
	for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it != m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		layer->Transform(m11, m21, m31, m12, m22, m32);
	}
}

std::string CGrfDoc::CreateDiwatuEntity(bool bUpdate, CStringA maptype, CString strName, const char* blob, int minMapLevel, int maxMapLevel, float srcMapLevel, bool withLngLat, bool withGeoCode, CString strDescription, std::optional<int> minPartialLevel, std::optional<int> maxPartialLevel)
{
	Gdiplus::Bitmap* preview = this->CreateThumbnail(240, 160);
	char* thumbnail = CWebService::BitmapToString64(preview);
	::delete preview;

	std::vector<std::string> codelist;
	for(CLayerTree::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_strGeoCatogery.IsEmpty())
			continue;
		codelist.push_back("\"" + std::string(layer->m_strGeoCatogery) + "\"");
	}
	const auto last = std::unique(codelist.begin(), codelist.end());
	codelist.erase(last, codelist.end());
	std::string geocodes;
	if(codelist.size() == 0)
		geocodes = "";
	else if(codelist.size() == 1)
		geocodes = codelist[0];
	else
		geocodes = std::accumulate(codelist.begin() + 1, codelist.end(), codelist[0], [](const std::string& a, std::string b) {
		return "\"" + a + "\"" + ",\"" + b + "\"";
	});

	std::stringstream fmt;
	fmt << "{";
	if(bUpdate)
	{
		fmt << "\"name\": \"";
		fmt << StringToEscapedASCII(strName);
		fmt << "\",";
		fmt << "\"type\": ";
		if(maptype == "VectorPage")
			fmt << 0;
		else if(maptype == "RasterPage")
			fmt << 1;
		else if(maptype == "VectorTile")
			fmt << 2;
		else if(maptype == "RasterTile")
			fmt << 3;
		else
			fmt << -1;
		if(minPartialLevel != nullopt)
		{
			fmt << ",";
			fmt << "\"minPartialLevel\":";
			fmt << minPartialLevel.value();
		}
		if(maxPartialLevel != nullopt)
		{
			fmt << ",";
			fmt << "\"maxPartialLevel\":";
			fmt << maxPartialLevel.value();
		}
	}
	else
	{
		fmt << "\"entity\":{";
		fmt << "\"Name\": \"";
		fmt << StringToEscapedASCII(strName);
		fmt << "\",";
		fmt << "\"Title\": \"";
		fmt << StringToEscapedASCII(strName);
		fmt << "\",";
		fmt << "\"Type\": ";
		if(maptype == "VectorPage")
			fmt << 0;
		else if(maptype == "RasterPage")
			fmt << 1;
		else if(maptype == "VectorTile")
			fmt << 2;
		else if(maptype == "RasterTile")
			fmt << 3;
		else
			fmt << -1;
		fmt << ",";
		fmt << "\"Description\": \"";
		fmt << StringToEscapedASCII(strDescription);
		fmt << "\",";
		fmt << "\"MinLevel\":";
		fmt << (int)minMapLevel;
		fmt << ",";
		fmt << "\"MaxLevel\":";
		fmt << (int)maxMapLevel;
		fmt << ",";
		fmt << "\"DftLevel\":";
		fmt << srcMapLevel;
		fmt << ",";
		fmt << "\"WithLngLat\":";
		fmt << withLngLat;
		fmt << ",";
		fmt << "\"WithGeoCode\":";
		fmt << withGeoCode;
		fmt << "}";
	}
	if(blob != nullptr)
	{
		fmt << ",";
		fmt << "\"blob\":\"";
		fmt << blob;
		fmt << "\"";
	}
	if(thumbnail != nullptr)
	{
		fmt << ",";
		fmt << "\"thumbnail\":\"data:image/png;base64,";
		fmt << thumbnail;
		fmt << "\",";
		::free(thumbnail);
	}
	if(true)
	{
		fmt << "\"mapperFile\": \"\",";
		fmt << "\"mapperData\": null";
	}
	fmt << "}";
	return fmt.str();
}

void CGrfDoc::PreRemoveGeom(CGeom* pGeom)
{
	POSITION pos = GetFirstViewPosition();
	if(pos == nullptr)
		return;

	CGrfView* pView = (CGrfView*)GetNextView(pos);
	if(pView == nullptr)
		return;

	if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)))
	{
		for(std::list<CTopology*>::iterator it = m_topolist.begin(); it != m_topolist.end(); it++)
		{
			CTopology* pTopology = *it;
			pTopology->PreRemovePoly(pView, m_Datainfo, pView->m_viewMonitor, pGeom->m_geoId);
		}
	}
}
void CGrfDoc::Replace(CTypedPtrList<CObList, CGeom*>& geomlist)
{
	POSITION pos1 = geomlist.GetHeadPosition();
	POSITION pos2 = pos1;
	while((pos2 = pos1) != nullptr)
	{
		CGeom* pGeom = geomlist.GetNext(pos1);
		switch(pGeom->m_bType)
		{
			case 5:
				CGrfDoc::Replace(((CGroup*)pGeom)->m_geomlist);
				break;
			case 41:
				{
					CSubmap* pSubmap = new CSubmap((Holder::Submap*)pGeom);
					geomlist.SetAt(pos2, pSubmap);
					delete pGeom;
				}
				break;
			default:
				break;
		}
	}
}
void CGrfDoc::OnProcessAutoTagging()
{
	CRectF mapRect = m_Datainfo.GetMapRect();
	double mapWidth = mapRect.Width();
	double mapHeight = mapRect.Height();
	double mapMax = max(mapWidth, mapHeight);
	CRectF mapSquare = mapRect;
	mapSquare.InflateRect((mapMax - mapWidth) / 2.f, (mapMax - mapHeight) / 2.f);

	CViewMonitor viewinfo(m_Datainfo);
	viewinfo.m_bViewer = false;
	for(int level = viewinfo.m_levelMinimum; level <= viewinfo.m_levelMaximum; level++)
	{
		viewinfo.m_sizeView.cx = 256 * pow(2, level) * (mapWidth / mapMax);
		viewinfo.m_sizeView.cy = 256 * pow(2, level) * (mapHeight / mapMax);

		viewinfo.m_sizeView.cx = 256 * pow(2, level) * (mapWidth / mapMax);
		viewinfo.m_sizeView.cy = 256 * pow(2, level) * (mapHeight / mapMax);

		int tileCount = pow(2.0, level);
		float factorMapToView = 256.f * pow(2, level) / mapMax;
		viewinfo.m_xFactorMapToView = factorMapToView;
		viewinfo.m_yFactorMapToView = factorMapToView;
		viewinfo.m_scaleCurrent = CDatainfo::GetScaleFromMapToViewRatio(m_Datainfo.m_pProjection, factorMapToView, mapRect.CenterPoint(), CSize(72, 72));
		viewinfo.m_levelCurrent = log(max(viewinfo.m_sizeView.cx, viewinfo.m_sizeView.cy) / 256) / log(2.0);

		int minCol = (int)((mapRect.left - mapSquare.left) * factorMapToView) / 256;
		int maxCol = (int)((mapRect.right - mapSquare.left) * factorMapToView) / 256;
		int minRow = (int)((mapSquare.bottom - mapRect.bottom) * factorMapToView) / 256;
		int maxRow = (int)((mapSquare.bottom - mapRect.top) * factorMapToView) / 256;
		minCol = max(0, min(minCol, tileCount - 1));
		maxCol = max(0, min(maxCol, tileCount - 1));
		minRow = max(0, min(minRow, tileCount - 1));
		maxRow = max(0, min(maxRow, tileCount - 1));

		int stepRows = min(25, maxRow - minRow + 1);
		int stepCols = min(25, maxCol - minCol + 1);

		HDC hdc = ::CreateCompatibleDC(nullptr);
		BITMAPINFO* pbmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD));
		pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pbmi->bmiHeader.biWidth = stepCols * 256 + 100;
		pbmi->bmiHeader.biHeight = stepRows * 256 + 100;
		pbmi->bmiHeader.biPlanes = 1;
		pbmi->bmiHeader.biBitCount = 1;
		pbmi->bmiHeader.biCompression = BI_RGB;
		pbmi->bmiHeader.biClrUsed = 2;
		pbmi->bmiColors[0].rgbBlue = 0x00;
		pbmi->bmiColors[0].rgbGreen = 0x00;
		pbmi->bmiColors[0].rgbRed = 0x00;
		pbmi->bmiColors[1].rgbBlue = 0xFF;
		pbmi->bmiColors[1].rgbGreen = 0xFF;
		pbmi->bmiColors[1].rgbRed = 0xFF;
		HBITMAP hBitmap = ::CreateDIBSection(hdc, pbmi, 0, nullptr, nullptr, 0);
		const HANDLE oldBmp = ::SelectObject(hdc, hBitmap);

		viewinfo.m_ptViewPos.x = -(mapRect.left - mapSquare.left) * factorMapToView;
		viewinfo.m_ptViewPos.y = -(mapSquare.bottom - mapRect.bottom) * factorMapToView;
		int staRow = minRow;
		do
		{
			int midRow = min(staRow + stepRows - 1, maxRow);
			int staCol = minCol;

			do {
				BITMAP bitmap;
				::GetObject(hBitmap, sizeof(BITMAP), &bitmap);
				BitBlt(hdc, 0, 0, bitmap.bmWidth, 100, hdc, 0, bitmap.bmHeight - 100, SRCCOPY);
				BitBlt(hdc, 0, 0, 100, bitmap.bmHeight, hdc, bitmap.bmWidth - 100, 0, SRCCOPY);
				CViewMonitor viewinfo2(m_Datainfo);
				viewinfo2 = viewinfo;
				viewinfo2.m_ptViewPos.x = staCol * 256 + viewinfo.m_ptViewPos.x - 100;
				viewinfo2.m_ptViewPos.y = staRow * 256 + viewinfo.m_ptViewPos.y - 100;
				CRect inRect(staCol * 256, staRow * 256, min(staCol + stepCols, maxCol + 1) * 256, min(staRow + stepRows, maxRow + 1) * 256);
				inRect = viewinfo.ClientToDoc(inRect);

				Gdiplus::Graphics* g = Gdiplus::Graphics::FromHDC(hdc);
				g->SetPageUnit(Gdiplus::Unit::UnitPixel);
				g->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeNone);
				//g->Clear(Gdiplus::Color::Transparent);
				Gdiplus::SolidBrush brush(Gdiplus::Color::Black);
				g->FillRectangle(&brush, 100, 100, bitmap.bmWidth - 100, bitmap.bmHeight - 100);
				monochrome::SaveHBITMAPToFile(hBitmap, _T("c:\\2.bmp"));

				for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
				{
					CLayer* layer = *it;
					layer->Alreadyin(*g, viewinfo2, inRect);
				}
				for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
				{
					CLayer* layer = *it;
					layer->Squeezein(*g, viewinfo2, inRect);
				}
				for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
				{
					CLayer* layer = *it;
					layer->Preoccupy(*g, viewinfo2, inRect);
				}
				for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
				{
					CLayer* layer = *it;
					layer->PositeTag(*g, viewinfo2, inRect, true);
				}

				delete g;

				staCol = min(staCol + stepCols - 1, maxCol) + 1;

				//	monochrome::SaveHBITMAPToFile(hBitmap, _T("c:\\1.bmp"));
			} while(staCol <= maxCol);

			staRow = midRow + 1;
		} while(staRow <= maxRow);

		::SelectObject(hdc, oldBmp);
		::DeleteDC(hdc);

		::DeleteObject(hBitmap);
		::free(pbmi);
	}
}
void CGrfDoc::OnPolygonAutoColoring()
{
	AfxGetApp()->BeginWaitCursor();
	for(CLayerTree::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bVisible == false)
			continue;

		layer->AutoColoring(m_Datainfo.m_zoomPointToDoc);
	}
	AfxGetApp()->EndWaitCursor();
}
