#include "stdafx.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "MainFrm.h"
#include "GisView.h"
#include "GisDoc.h"
#include "Global.h"
#include "CreateTableDlg.h"
#include "../../ThirdParty/tinyxml/tinyxml2/tinyxml2.h"

#include "../Atlas/Atlas1.h"
#include "../Topology/TopoCtrl.h"
#include "../Tool/InjectTool.h"
#include "../Tool/PositionTool.h"
#include "../Tool/InfoTool.h"
#include "../Tool/SpinTool.h"

#include "../Geometry/Global.h"
#include "../Geometry/Geom.h"
#include "../Geometry/PRect.h"
#include "../Geometry/Mark.h"

#include "../Layer/LayerTreeCtrl.h"
#include "../Layer/Laypt.h"

#include "../Hyperlink/Link.h"
#include "../Hyperlink/LinkMap.h"
#include "../Hyperlink/LinkDlg.h"

#include "../Information/TDBGridCtrl.h"
#include "../Information/DBGridCtrl.h"
#include "../Information/POUListCtrl.h"
#include "../Information/POUPane.h"
#include "../Information/POUTDBGridCtrl.h"
#include "../Information/POUDBGridCtrl.h"
#include "../Information/TablePane.h"
#include "../Information/ATTDBGridCtrl.h"
#include "../Information/ATTTDBGridCtrl.h"

#include "../Public/AttributeCtrl.h"
#include "../Public/PropertyPane.h"

#include "../Port/ImportGdal.h"

#include "../Projection/Projection1.h"
#include "../Projection/Geographic.h"
#include "../Projection/RotateDlg.h"

#include "../Port/ImportJson.h"
#include "../Frame/FrameDlg.h"
#include "../Utility/Scale.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern __declspec(dllimport) CInjectTool injectTool;
extern __declspec(dllimport) CPositionTool positionTool;
extern __declspec(dllimport) CInfoTool infoTool;
extern __declspec(dllimport) CSpinTool spinTool;
extern __declspec(dllimport) CLayerTree lt;

IMPLEMENT_DYNCREATE(CGisDoc, CGrfDoc)

BEGIN_MESSAGE_MAP(CGisDoc, CGrfDoc)
	//{{AFX_MSG_MAP(CGisDoc)	
	ON_COMMAND(ID_FILE_IMPORTINTO, OnImportInto)
	ON_COMMAND(ID_MAP_PASTE, OnMapPaste)
	ON_COMMAND(ID_PROJECTION_DATA, OnDataProjection)

	ON_COMMAND(ID_LINK_CREATE, OnLinkCreate)
	ON_COMMAND(ID_LINK_EDIT, OnLinkEdit)
	ON_COMMAND(ID_LINK_DELETE, OnLinkDelete)

	ON_COMMAND(ID_TOOL_DBINJECT, OnDBInjectTool)
	ON_COMMAND(ID_TOOL_DBPOSITION, OnDBPositionTool)
	ON_COMMAND(ID_TOOL_DBPICKINFO, OnDBPickInfoTool)
	ON_COMMAND(ID_TOO_PREVIEWSPIN, OnPreviewSpinTool)
	ON_COMMAND(ID_DATABASE_UNASSIGNATT, OnUnassignATT)
	ON_COMMAND(ID_TRANSFORM_ROLL, OnRoll)
	ON_COMMAND(ID_TRANSFORM_PROJECT, OnProject)
	ON_COMMAND(ID_GEOGRAPH_FRAME, OnGeographFrame)
	ON_COMMAND(ID_PROCESS_WIPEOUT, OnProcessWipeout)
	//}}AFX_MSG_MAP)
END_MESSAGE_MAP()

CGisDoc::CGisDoc() noexcept
	: m_POUListCtrl(m_poulist)
{
	EnableCompoundFile(FALSE);
	m_ClipboardFormat = (CLIPFORMAT)::RegisterClipboardFormat(_T("Diwatu Mapper Gis"));

	m_Datainfo.m_scaleMinimum = 100000000;
	m_Datainfo.m_scaleSource = 10000;
	m_Datainfo.m_scaleMaximum = 1000;
	m_Datainfo.m_mapCanvas.cx = 2*M_PI;
	m_Datainfo.m_mapCanvas.cy = M_PI;
	m_Datainfo.m_mapOrigin.x = -M_PI;
	m_Datainfo.m_mapOrigin.y = -M_PI/2;
	m_Datainfo.m_strType = "Gis";
	CProjection* pProjection = new CGeographic();
	pProjection->SetupGCS("sphere");
	m_Datainfo.ResetProjection(pProjection, false);

	CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();
	CPOUPane* pPOUPane = (CPOUPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'P', 0);
	if(pPOUPane!=nullptr)
	{
		if(m_POUListCtrl.Create(pPOUPane, true)==TRUE)
		{
			m_POUListCtrl.m_pDocument = this;
		}
	}
}

CGisDoc::~CGisDoc()
{
	for(std::list<CLink*>::iterator it = m_linklist.begin(); it!=m_linklist.end(); ++it)
	{
		const CLink* link = *it;
		link = nullptr;
	}
	m_linklist.clear();

	CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();
	CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'O', 0);
	if(pTablePane!=nullptr)
	{
		if(pTablePane->m_pATTTDBGridCtrl!=nullptr)
		{
			pTablePane->m_pATTTDBGridCtrl->SetDatabase(nullptr);
		}
		if(pTablePane->m_pATTDBGridCtrl!=nullptr)
		{
			pTablePane->m_pATTDBGridCtrl->Clear();
		}
		if(pTablePane->m_pPOUTDBGridCtrl!=nullptr)
		{
			pTablePane->m_pPOUTDBGridCtrl->SetDatabase(nullptr);
		}
		if(pTablePane->m_pPOUDBGridCtrl!=nullptr)
		{
			pTablePane->m_pPOUDBGridCtrl->Clear();
		}
	}

	CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
	if(pPropertyPane!=nullptr)
	{
		CAttributeCtrl* pAttributeCtrl = (CAttributeCtrl*)pPropertyPane->GetTabWnd('A');
		if(pAttributeCtrl!=nullptr)
		{
			pAttributeCtrl->OnTableChanged(nullptr, _T(""));
		}
	}

	CPOUPane* pPOUPane = (CPOUPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'P', 0);
	if(pPOUPane!=nullptr&&pPOUPane->m_pMapPOUListCtrl==&m_POUListCtrl)
	{
		pPOUPane->SetMapCtrl((CPOUListCtrl*)nullptr);
	}
	if(m_POUListCtrl.m_hWnd!=nullptr)
	{
		m_POUListCtrl.PostMessage(WM_DESTROY, 0, 0);
		m_POUListCtrl.DestroyWindow();
	}
}
void CGisDoc::OnImportInto()
{
	TCHAR CurrentDir[256];
	GetCurrentDirectory(255, CurrentDir);
	CString strPath = AfxGetApp()->GetProfileString(_T(""), _T("Import Path"), CurrentDir);

	TCHAR BASED_CODE szIM_IM_Filter[] = _T("All Diwatu Files(*.adf,*.shp,*.e00,*.Tab,*.mif,*.dxf,*.dwg,*.dgn,*.RT1)|*.adf;*.shp;*.e00;*.Tab;*.mif;*.dxf;*.dwg;*.dgn;*.RT1|")
		_T("ArcInfo Coverage Files(*.adf)|*.adf|")
		_T("ArcInfo E00 Files(*.e00)|*.e00|")
		_T("ArcView Shape Files(*.shp)|*.shp|")
		_T("AutoCAD DXF Files(*.dxf)|*.dxf|")
		_T("AutoCAD DWG Files(*.dwg)|*.dwg|")
		_T("MapInfo File Files(*.Mif)|*.Mif|")
		_T("MapInfo File Files(*.Tab)|*.Tab|")
		_T("GeoJSON Files(*.geojson;*.geo.json)|*.geojson;*.geo.json|")
		_T("JSON Files(*.json)|*.json|")
		_T("Microstation DGN Files(*.Dgn)|*.Dgn|")
		_T("U.S. Census TIGER/Line Files(*.RT1)|*.RT1|");
	CFileDialog dlg(true, _T(""), strPath, OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT, szIM_IM_Filter, AfxGetMainWnd());
	TCHAR buffer[65535];
	memset(buffer, 0, 65535);
	dlg.m_ofn.lpstrFile = buffer;
	dlg.m_ofn.nMaxFile = 65535;
	dlg.m_ofn.lpstrTitle = _T("Select Files to Import");
	if(dlg.DoModal()==IDOK)
	{
		CString strPath;// = dlg.GetFolderPath();
		CStringList files;
		POSITION pos1 = dlg.GetStartPosition();
		while(pos1!=nullptr)
		{
			CString strFile = dlg.GetNextPathName(pos1);
			CString strExt = strFile.Mid(strFile.ReverseFind(_T('.'))+1);
			strExt.MakeUpper();
			strPath = strFile.Left(strFile.ReverseFind(_T('\\')));
			strFile = strFile.Mid(strPath.GetLength()+1);
			if(files.Find(strFile)==nullptr)
				files.AddTail(strFile);
		}
		if(files.GetCount()>0)
		{
			AfxGetApp()->BeginWaitCursor();
			CImportGdal importer(m_Datainfo, m_layertree, m_dwMaxGeomId);
			CString dbname;
			if(importer.Import(strPath, files, true, dbname, true)==TRUE)
			{
				m_Datainfo.m_strName = dbname;
				this->SetTitle(dbname);

				if(m_layertree.m_AttDBList.find(dbname)==m_layertree.m_AttDBList.end())
				{
					CODBCDatabase* pATTDatabase = new CODBCDatabase();
					pATTDatabase->m_dbms = DBMSTYPE::Access;
					pATTDatabase->m_strDatabase = strPath+_T("\\")+dbname+_T(".mdb");
					pATTDatabase->m_strUID.Empty();
					pATTDatabase->m_strPWD.Empty();
					pATTDatabase->Open(CDatabase::noOdbcDialog);
					m_layertree.m_AttDBList[dbname] = pATTDatabase;
				}
				if(m_LayerTreeCtrl.m_hWnd!=nullptr)
				{
					m_LayerTreeCtrl.DeleteAllItems();
				}
				m_LayerTreeCtrl.BuildTree();
				m_layertree.Rematch();
				this->SendInitialUpdate();

				AfxGetApp()->EndWaitCursor();
			}
			else
			{
				AfxGetApp()->EndWaitCursor();
			}
		}
	}
}
void CGisDoc::OnDataProjection()
{
	m_Datainfo.ChangeProjection(false);
	if(m_Datainfo.m_pProjection==nullptr)
	{
		POSITION pos = GetFirstViewPosition();
		while(pos!=nullptr)
		{
			CView* pView = GetNextView(pos);
			if(pView->IsKindOf(RUNTIME_CLASS(CGisView)))
			{
				((CGisView*)pView)->m_viewMonitor.ResetProjection(nullptr);
			}
		}
		for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it!=m_layertree.forwend(); ++it)
		{
			CLayer* layer = *it;
			layer->Purify();
		}
	}
	this->SetModifiedFlag(true);
}

void CGisDoc::OnMapPaste()
{
	if(m_ClipboardFormat!=(CLIPFORMAT)::RegisterClipboardFormat(_T("Diwatu Mapper Gis")))
		return;

	if(m_Datainfo.m_pProjection==nullptr)
	{
		AfxMessageBox(_T("This map has no projection, Map->Past is not supported!"));
		return;
	}

	POSITION pos = GetFirstViewPosition();
	if(pos==nullptr)
		return;

	CGrfView* pView = (CGrfView*)GetNextView(pos);
	if(pView==nullptr)
		return;

	COleDataObject dataObject;
	dataObject.AttachClipboard();
	if(dataObject.IsDataAvailable(m_ClipboardFormat))
	{
		CFile* pFile = dataObject.GetFileData(m_ClipboardFormat);
		if(pFile==nullptr)
			return;

		CArchive ar(pFile, CArchive::load);
		try
		{
			CPoint objcenter;
			DWORD offsetID = m_dwMaxGeomId;
			DWORD dwVersion;
			ar>>dwVersion;
			ar>>objcenter;

			CDatainfo datainfo;
			CLayerTree layertree;
			datainfo.Serialize(ar, dwVersion);
			layertree.Serialize(ar, dwVersion);

			for(CTree<CLayer>::forwiterator it = layertree.forwbegin(); it!=layertree.forwend(); ++it)
			{
				CLayer* layer = *it;
				POSITION pos2 = layer->m_geomlist.GetHeadPosition();
				while(pos2!=nullptr)
				{
					CGeom* pGeom = layer->m_geomlist.GetNext(pos2);
					pGeom->Project(datainfo, m_Datainfo, Interpolation::Nothing, Scale::PrecisionFromScale(m_Datainfo.m_scaleSource));
					pGeom->m_geoId = ++m_dwMaxGeomId;
					pGeom->m_bActive = true;
				}
				layer->m_nActiveCount = layer->m_geomlist.GetCount();
			}

			CGrfDoc::Paste(layertree);

			ar.Close();
			delete pFile;

			pView->Invalidate();
			SetModifiedFlag(TRUE);
		}
		catch(CException* ex)
		{
			//				OutputDebugString(ex->);
			ar.Close();
			delete pFile;
			THROW_LAST();
		}
	}
}

BOOL CGisDoc::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
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
			CGisView* pView = (CGisView*)GetNextView(pos);

			switch(nId)
			{
				case ID_MAP_PASTE:
					{
						if(m_ClipboardFormat==(CLIPFORMAT)::RegisterClipboardFormat(_T("Diwatu Mapper Gis")))
						{
							COleDataObject dataObject;
							const BOOL bEnable = dataObject.AttachClipboard()&&(dataObject.IsDataAvailable(m_ClipboardFormat)||COleClientItem::CanCreateFromData(&dataObject));

							pCmdUI->Enable(bEnable);
						}
						else
							pCmdUI->Enable(FALSE);
					}
					return true;
					break;
				case ID_LINK_CREATE:
				case ID_LINK_EDIT:
				case ID_LINK_DELETE:
					{
						for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it!=m_layertree.forwend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount==0)
								continue;

							POSITION pos2 = layer->m_geomlist.GetHeadPosition();
							while(pos2!=nullptr)
							{
								CGeom* geom = layer->m_geomlist.GetNext(pos2);
								if(geom->m_bActive==false)
									continue;

								for(std::list<CLink*>::iterator it = m_linklist.begin(); it!=m_linklist.end(); ++it)
								{
									const CLink* link = *it;
									if(link->m_dwGeomId==geom->m_geoId)
									{
										if(nId==ID_LINK_EDIT||nId==ID_LINK_DELETE)
											pCmdUI->Enable(TRUE);
										if(nId==ID_LINK_CREATE)
											pCmdUI->Enable(FALSE);
										return TRUE;
									}
								}

								if(nId==ID_LINK_EDIT||nId==ID_LINK_DELETE)
									pCmdUI->Enable(FALSE);
								if(nId==ID_LINK_CREATE)
									pCmdUI->Enable(TRUE);
								return TRUE;
							}
						}
					}
					pCmdUI->Enable(FALSE);
					return TRUE;
					break;
				case ID_TOOL_DBINJECT:
					{
						CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'O', 0);
						if(m_layertree.m_AttDBList.size()==0)
							pCmdUI->Enable(FALSE);
						else if(pTablePane==nullptr)
							pCmdUI->Enable(FALSE);
						else if(pTablePane->m_pATTTDBGridCtrl==nullptr)
							pCmdUI->Enable(FALSE);
						else if(pTablePane->m_pATTTDBGridCtrl->IsWindowVisible()==FALSE)
							pCmdUI->Enable(FALSE);
						else
							pCmdUI->Enable(TRUE);

						if(m_pTool==&injectTool)
							pCmdUI->SetCheck(TRUE);
						else
							pCmdUI->SetCheck(FALSE);
						return TRUE;
					}
					break;
				case ID_TOOL_DBPOSITION:
					{
						CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'O', 0);
						if(pTablePane==nullptr)
							pCmdUI->Enable(FALSE);
						else if(pTablePane->m_pPOUTDBGridCtrl==nullptr)
							pCmdUI->Enable(FALSE);
						else if(pTablePane->m_pPOUTDBGridCtrl->IsWindowVisible()==FALSE)
							pCmdUI->Enable(FALSE);
						else if(m_poulist.m_pous.size()==0)
							pCmdUI->Enable(FALSE);
						else
							pCmdUI->Enable(TRUE);

						if(m_pTool==&positionTool)
							pCmdUI->SetCheck(TRUE);
						else
							pCmdUI->SetCheck(FALSE);
						return TRUE;
					}
					break;
				case ID_TOOL_DBPICKINFO:
					if(m_layertree.m_AttDBList.size()>0)
						pCmdUI->Enable(TRUE);
					else if(m_poulist.m_pous.size()>0)
						pCmdUI->Enable(TRUE);
					else
						pCmdUI->Enable(FALSE);
					if(m_pTool==&infoTool)
						pCmdUI->SetCheck(TRUE);
					else
						pCmdUI->SetCheck(FALSE);
					return TRUE;
					break;
				case ID_DATABASE_UNASSIGNATT:
					{
						POSITION pos = GetFirstViewPosition();
						CGisView* pView = (CGisView*)GetNextView(pos);
						for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it!=m_layertree.forwend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount==0)
								continue;
							CATTDatasource* pDatasource = layer->GetAttDatasource();
							if(pDatasource==nullptr)
								continue;
							CDatabase* pDatabase = m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
							if(pDatabase==nullptr)
								continue;
							if(pDatabase->IsOpen()==FALSE)
								continue;

							POSITION pos2 = layer->m_geomlist.GetHeadPosition();
							while(pos2!=nullptr)
							{
								const CGeom* geom = layer->m_geomlist.GetNext(pos2);
								if(geom->m_bActive==false)
									continue;
								else if(geom->m_attId!=0XFFFFFFFF)
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
				case ID_TRANSFORM_ROLL:
				case ID_PREVIEW_PROJECTION:
					pCmdUI->Enable(m_Datainfo.m_pProjection!=nullptr&&m_Datainfo.IsLngLat());
					return TRUE;		
				case ID_TRANSFORM_PROJECT:
					pCmdUI->Enable(m_Datainfo.m_pProjection==nullptr ? FALSE : TRUE);
					return true;
				default:
					break;
			}
			pCmdUI->m_bContinueRouting = false;
		}
		else if(nCode==CN_COMMAND)
		{
		}
	}

	return CGrfDoc::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}

void CGisDoc::Draw(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect)
{
	CGrfDoc::Draw(pView, viewinfo, g, invalidRect);
	const CPoint docPoint1 = invalidRect.TopLeft();
	const CPointF geoPoint1 = m_Datainfo.DocToWGS84(docPoint1);
	const CPoint docPoint2 = invalidRect.BottomRight();
	const CPointF geoPoint2 = m_Datainfo.DocToWGS84(docPoint2);

	CRectF geoRect = CRectF(geoPoint1.x, geoPoint1.y, geoPoint2.x, geoPoint2.y);
	geoRect.NormalizeRect();
	for(std::list<CPOU*>::reverse_iterator it = m_poulist.m_pous.rbegin(); it!=m_poulist.m_pous.rend(); ++it)
	{
		CPOU* pPOU = *it;
		pPOU->Draw(g, viewinfo, geoRect);
	}

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if(pMainFrame!=nullptr&&pMainFrame->m_pAtlas!=nullptr)
	{
		for(std::list<CPOU*>::reverse_iterator it = pMainFrame->m_pAtlas->m_poulist.m_pous.rbegin(); it!=pMainFrame->m_pAtlas->m_poulist.m_pous.rend(); ++it)
		{
			CPOU* pPOU = *it;
			pPOU->Draw(g, viewinfo, geoRect);
		}
	}
}

void CGisDoc::DrawTag(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect)
{
	CGrfDoc::DrawTag(pView, viewinfo, g, invalidRect);
	const CPoint docPoint1 = invalidRect.TopLeft();
	const CPointF geoPoint1 = m_Datainfo.DocToWGS84(docPoint1);
	const CPoint docPoint2 = invalidRect.BottomRight();
	const CPointF geoPoint2 = m_Datainfo.DocToWGS84(docPoint2);

	CRectF geoRect = CRectF(geoPoint1.x, geoPoint1.y, geoPoint2.x, geoPoint2.y);
	geoRect.NormalizeRect();

	for(std::list<CPOU*>::reverse_iterator it = m_poulist.m_pous.rbegin(); it!=m_poulist.m_pous.rend(); ++it)
	{
		CPOU* pPOU = *it;
		pPOU->DrawTag(g, viewinfo, geoRect);
	}

	const CMainFrame* pAppFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if(pAppFrame->m_pAtlas!=nullptr)
	{
		for(std::list<CPOU*>::reverse_iterator it = pAppFrame->m_pAtlas->m_poulist.m_pous.rbegin(); it!=pAppFrame->m_pAtlas->m_poulist.m_pous.rend(); ++it)
		{
			CPOU* pPOU = *it;
			pPOU->DrawTag(g, viewinfo, geoRect);
		}
	}
}
void CGisDoc::OnProcessWipeout()
{
	if(AfxMessageBox(IDS_LOSINGUNDOS, MB_YESNO)==IDYES)
	{
		CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'O', 0);
		if(pTablePane!=nullptr)
		{
			if(pTablePane->m_pATTTDBGridCtrl!=nullptr)
			{
				pTablePane->m_pATTTDBGridCtrl->SetDatabase(nullptr);
			}
			if(pTablePane->m_pATTDBGridCtrl!=nullptr)
			{
				pTablePane->m_pATTDBGridCtrl->Clear();
			}
		}

		m_actionStack.Clear();
		m_layertree.Wipeout();

		m_dwMaxGeomId = 0;
		SetModifiedFlag(TRUE);
		this->UpdateAllViews(nullptr);
	}
}
BOOL CGisDoc::ImportGdal(CString strPath, CStringList& files, bool ignoreProjection = false)
{
	AfxGetApp()->BeginWaitCursor();
	CImportGdal importer(m_Datainfo, m_layertree, m_dwMaxGeomId);
	importer.hiddenlevel = this->hiddenlevel;
	CString dbname;
	if(importer.Import(strPath, files, ignoreProjection, dbname)==TRUE)
	{
		m_Datainfo.m_strName = dbname;
		this->SetTitle(dbname);

		if(m_layertree.m_AttDBList.find(dbname)==m_layertree.m_AttDBList.end())
		{
			CODBCDatabase* pATTDatabase = new CODBCDatabase();
			pATTDatabase->m_dbms = DBMSTYPE::Access;
			pATTDatabase->m_strDatabase = strPath+_T("\\")+dbname+_T(".mdb");
			pATTDatabase->m_strUID.Empty();
			pATTDatabase->m_strPWD.Empty();
			pATTDatabase->Open(CDatabase::noOdbcDialog);
			m_layertree.m_AttDBList[dbname] = pATTDatabase;
		}
		if(m_LayerTreeCtrl.m_hWnd!=nullptr)
		{
			m_LayerTreeCtrl.DeleteAllItems();
		}
		m_LayerTreeCtrl.BuildTree();

		this->SendInitialUpdate();

		AfxGetApp()->EndWaitCursor();
		return TRUE;
	}
	else
	{
		AfxGetApp()->EndWaitCursor();
		return FALSE;
	}
}
BOOL CGisDoc::ImportJson(CString& strFile)
{
	AfxGetApp()->BeginWaitCursor();
	CString dbname;
	CImportJson importer(m_Datainfo, m_layertree, m_topolist, m_dwMaxGeomId);
	importer.hiddenlevel = this->hiddenlevel;
	if(importer.Import(strFile, dbname)==TRUE)
	{
		this->SetTitle(m_Datainfo.m_strName);
		if(dbname.IsEmpty())
		{
		}
		else if(m_layertree.m_AttDBList.find(dbname)==m_layertree.m_AttDBList.end())
		{
			CString strPath = strFile.ReverseFind(_T('\\'))!=-1 ? strFile.Left(strFile.ReverseFind(_T('\\'))) : CString("");

			CODBCDatabase* pATTDatabase = new CODBCDatabase();
			pATTDatabase->m_dbms = DBMSTYPE::Access;
			pATTDatabase->m_strDatabase = strPath+_T("\\")+dbname+_T(".mdb");
			pATTDatabase->m_strUID.Empty();
			pATTDatabase->m_strPWD.Empty();
			pATTDatabase->Open(CDatabase::noOdbcDialog);
			m_layertree.m_AttDBList[dbname] = pATTDatabase;
		}

		m_LayerTreeCtrl.DeleteAllItems();
		m_LayerTreeCtrl.BuildTree();
		m_pTopoCtrl->DeleteAllItems();
		m_pTopoCtrl->Built();

		this->SendInitialUpdate();

		AfxGetApp()->EndWaitCursor();
		return TRUE;
	}
	else
	{
		AfxGetApp()->EndWaitCursor();
		return FALSE;
	}
	return TRUE;
}
void CGisDoc::Serialize(CArchive& ar)
{
	DWORD dwCurrent = AfxGetCurrentArVersion();
	DWORD dwVersion = dwCurrent;
	if(ar.IsStoring())
	{
		ar<<dwVersion;
	}
	else
	{
		ar>>dwVersion;
		if(dwVersion>AfxGetCurrentArVersion())
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

	CGisDoc::Serialize(ar, dwVersion);
}

void CGisDoc::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CGrfDoc::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<(int)(m_linklist.size());
		for(std::list<CLink*>::iterator it = m_linklist.begin(); it!=m_linklist.end(); ++it)
		{
			CLink* pLink = *it;
			const BYTE type = pLink->Gettype();
			ar<<type;
			pLink->Serialize(ar, dwVersion);
		}
		m_poulist.Serialize(ar, dwVersion);
	}
	else
	{
		int count1;
		ar>>count1;
		for(int index = 0; index<count1; index++)
		{
			BYTE type;
			ar>>type;
			CLink* pLink = CLink::Apply(type);
			pLink->Serialize(ar, dwVersion);
			m_linklist.push_back(pLink);
		}
		m_poulist.Serialize(ar, dwVersion);
		if(m_POUListCtrl.m_hWnd!=nullptr)
		{
			m_POUListCtrl.Build();
		}
	}
}

#ifdef _DEBUG
void CGisDoc::AssertValid() const
{
	CGrfDoc::AssertValid();
}

void CGisDoc::Dump(CDumpContext& dc) const
{
	CGrfDoc::Dump(dc);
}
#endif //_DEBUG

void CGisDoc::OnLinkCreate()
{
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it!=m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount==0)
			continue;

		POSITION pos2 = layer->m_geomlist.GetHeadPosition();
		while(pos2!=nullptr)
		{
			CGeom* geom = layer->m_geomlist.GetNext(pos2);
			if(geom->m_bActive==false)
				continue;

			CLink* link = new CLinkMap;
			link->m_dwGeomId = geom->m_geoId;
			const HINSTANCE hInstOld = AfxGetResourceHandle();
			HINSTANCE hInst = ::LoadLibrary(_T("Hyperlink.dll"));
			AfxSetResourceHandle(hInst);

			CLinkDlg dlg(nullptr, link);
			if(dlg.DoModal()==IDOK)
			{
				if(link!=nullptr)
				{
					delete link;
					link = dlg.d_pLink;
					dlg.d_pLink = nullptr;

					m_linklist.push_back(link);
				}
			}
			else
			{
				delete link;
				link = nullptr;
			}

			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
		}
	}

	SetModifiedFlag(TRUE);
	return;
}

void CGisDoc::OnLinkEdit()
{
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it!=m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount==0)
			continue;

		POSITION pos2 = layer->m_geomlist.GetHeadPosition();
		while(pos2!=nullptr)
		{
			CGeom* geom = layer->m_geomlist.GetNext(pos2);
			if(geom->m_bActive==false)
				continue;

			for(std::list<CLink*>::iterator it = m_linklist.begin(); it!=m_linklist.end(); ++it)
			{
				CLink* link = *it;
				if(link->m_dwGeomId==geom->m_geoId)
				{
					const HINSTANCE hInstOld = AfxGetResourceHandle();
					HINSTANCE hInst = ::LoadLibrary(_T("Hyperlink.dll"));
					AfxSetResourceHandle(hInst);

					CLinkDlg dlg(nullptr, link);
					if(dlg.DoModal()==IDOK)
					{
						if(link!=nullptr)
						{
							delete link;
							link = dlg.d_pLink;
							dlg.d_pLink = nullptr;

							(*it) = link;
						}
					}

					AfxSetResourceHandle(hInstOld);
					::FreeLibrary(hInst);

					SetModifiedFlag(TRUE);
					return;
				}
			}
		}
	}
}

void CGisDoc::OnLinkDelete()
{
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it!=m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount==0)
			continue;

		POSITION pos2 = layer->m_geomlist.GetHeadPosition();
		while(pos2!=nullptr)
		{
			CGeom* geom = layer->m_geomlist.GetNext(pos2);
			if(geom->m_bActive==false)
				continue;

			for(std::list<CLink*>::iterator it = m_linklist.begin(); it!=m_linklist.end(); ++it)
			{
				const CLink* link = *it;
				if(link->m_dwGeomId==geom->m_geoId)
				{
					delete link;
					link = nullptr;
					m_linklist.erase(it);
					SetModifiedFlag(TRUE);
				}
			}
		}
	}
}
void CGisDoc::OnDBInjectTool()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_wndDrawTool.SwitchToolTo(&injectTool);
}
void CGisDoc::OnDBPositionTool()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_wndDrawTool.SwitchToolTo(&positionTool);
}
void CGisDoc::OnDBPickInfoTool()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_wndDrawTool.SwitchToolTo(&infoTool);
}
void CGisDoc::OnPreviewSpinTool()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_wndDrawTool.SwitchToolTo(&spinTool);
}
void CGisDoc::OnUnassignATT()
{
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it!=m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount==0)
			continue;

		POSITION pos2 = layer->m_geomlist.GetHeadPosition();
		while(pos2!=nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetNext(pos2);
			if(pGeom->m_bActive==false)
				continue;

			pGeom->m_attId = 0XFFFFFFFF;
			this->SetModifiedFlag(TRUE);
		}
	}
}
void CGisDoc::OnRoll()
{
	if(m_Datainfo.m_pProjection==nullptr)
		return;
	else if(m_Datainfo.IsLngLat()==false)
		return;
	else if(AfxMessageBox(IDS_LOSINGUNDOS, MB_YESNO)==IDYES)
	{
		m_actionStack.Clear();

		HINSTANCE hInst = ::LoadLibrary(_T("Projection.dll"));
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		AfxSetResourceHandle(hInst);

		CRotateDlg dlg(nullptr);
		if(dlg.DoModal()==IDOK)
		{
			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);

			const float deltaLng = dlg.m_lngDelta;
			const float deltaLat = dlg.m_latDelta;
			const float angle = dlg.m_angle;
			for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it!=m_layertree.postend(); ++it)
			{
				CLayer* layer = *it;
				layer->Rotate(m_Datainfo, deltaLng, deltaLat, angle);
			}
			SetModifiedFlag(TRUE);
		}
		else
		{
			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
		}
	}
}
void CGisDoc::OnProject()
{
	if(m_Datainfo.m_pProjection==nullptr)
		return;

	CDatainfo tMapinfo;
	tMapinfo = m_Datainfo;
	bool interpolation = tMapinfo.ChangeProjection(true);
	if(tMapinfo.m_pProjection==nullptr)
		return;


	if(AfxMessageBox(IDS_LOSINGUNDOS, MB_YESNO)==IDYES)
	{
		POSITION pos = GetFirstViewPosition();
		while(pos!=nullptr)
		{
			CView* pView = GetNextView(pos);
			if(pView->IsKindOf(RUNTIME_CLASS(CGisView)))
			{
				((CGisView*)pView)->m_viewMonitor.ResetProjection(nullptr, false);
			}
		}

		m_actionStack.Clear();
		float tolerance = Scale::PrecisionFromScale(m_Datainfo.m_scaleSource);
		if(*(m_Datainfo.m_pProjection)!=*(tMapinfo.m_pProjection))
		{
			for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it!=m_layertree.postend(); ++it)
			{
				CLayer* layer = *it;
				layer->Project(m_Datainfo, tMapinfo, Interpolation::GreatCircle, tolerance);
			}

			CRect docRect;
			docRect.SetRectEmpty();
			for(CTree<CLayer>::postiterator it = m_layertree.postbegin(); it!=m_layertree.postend(); ++it)
			{
				CLayer* layer = *it;
				docRect = UnionRect(docRect, layer->GetRect());
			}
			docRect.NormalizeRect();
			const CPoint docPoint = docRect.TopLeft();
			const unsigned long cx = docRect.right-docRect.left;
			const unsigned long cy = docRect.bottom-docRect.top;
			tMapinfo.m_mapOrigin = tMapinfo.DocToMap(docPoint);
			tMapinfo.m_mapCanvas = tMapinfo.DocToMap(cx, cy);
			m_Datainfo = tMapinfo;

			SetModifiedFlag(TRUE);
			POSITION Pos3 = GetFirstViewPosition();
			while(Pos3!=nullptr)
			{
				CView* pView = GetNextView(Pos3);
				if(pView==nullptr)
					return;
				if(pView->IsKindOf(RUNTIME_CLASS(CGrfView)))
				{
					((CGrfView*)pView)->m_viewMonitor.m_scaleCurrent = 0;
					((CGrfView*)pView)->m_viewMonitor.m_levelCurrent = 25;
					pView->OnInitialUpdate();
				}
			}
		}
	}
}

void CGisDoc::ReleaseWEBCap(UINT nId, CString strFolder, CString strName)
{
	CGrfDoc::ReleaseWEBCap(nId, strFolder, strName);
	const CString strXml = strFolder+_T('\\')+strName+_T(".xml");
	tinyxml2::XMLDocument xml;
	if(xml.LoadFile(CStringA(strXml))==tinyxml2::XMLError::XML_SUCCESS)
	{
		tinyxml2::XMLNode* pNode = xml.FirstChildElement("Map");
		if(pNode!=nullptr)
		{
			bool bInfoService = false;
			if(m_Datainfo.m_pProjection==nullptr)
			{
				//	tinyxml2::XMLNode* pNode = xml.FirstChildElement("Background");
			}
			if(m_poulist.m_pous.size()>0)
			{
				const CString strFilePOU = strFolder+_T('\\')+strName+_T(".Wpp");
				m_poulist.ReleaseWEB(strFilePOU, m_Datainfo);
				bInfoService = true;
			}
			else if(pNode->FirstChildElement("Pou")!=nullptr)
			{
				pNode->DeleteChild(pNode->FirstChildElement("Pou"));
			}

			if(m_layertree.m_AttDBList.size()==0&&pNode->FirstChildElement("GeoFeature")!=nullptr)
				pNode->DeleteChild(pNode->FirstChildElement("GeoFeature"));
			else
				bInfoService = true;
			const CString strFileXML = strFolder+_T('\\')+strName+_T(".xml");
			if(xml.SaveFile(CStringA(strFileXML))!=tinyxml2::XMLError::XML_SUCCESS)
			{
				CString strMessage;
				strMessage.Format(_T("Failed to save file %s.xml into folder %s"), strName, strFolder);
				AfxMessageBox(strMessage);
			}
			else if(bInfoService==true)
			{
				CString strMessage;
				strMessage.Format(_T("You need to edit file %s.xml in order to have information service from your Web server!"), strName);
				AfxMessageBox(strMessage);
			}
		}
	}
}

void CGisDoc::OnGeographFrame()
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Frame.dll"));
	AfxSetResourceHandle(hInst);


	CFrameDlg dlg(nullptr, m_frame.m_bEnabledSphere, m_frame.m_sphere.m_pColor1, m_frame.m_sphere.m_pColor2, m_frame.m_sphere.m_fWidth, m_frame.m_bEnabledGraticule, m_frame.m_graticule.m_pColor, m_frame.m_graticule.m_fWidth, m_frame.m_graticule.m_lngInterval, m_frame.m_graticule.m_latInterval);
	if(dlg.DoModal()==IDOK)
	{
		m_frame.m_bEnabledSphere = dlg.m_bEnabledSphere;
		m_frame.m_sphere.m_fWidth = dlg.m_fSphereWidth;
		if(m_frame.m_sphere.m_pColor1!=nullptr)
		{
			delete m_frame.m_sphere.m_pColor1;
			m_frame.m_sphere.m_pColor1 = nullptr;
		}
		m_frame.m_sphere.m_pColor1 = dlg.m_pSphereColor1;
		dlg.m_pSphereColor1 = nullptr;
		if(m_frame.m_sphere.m_pColor2!=nullptr)
		{
			delete m_frame.m_sphere.m_pColor2;
			m_frame.m_sphere.m_pColor2 = nullptr;
		}
		m_frame.m_sphere.m_pColor2 = dlg.m_pSphereColor2;
		dlg.m_pSphereColor2 = nullptr;

		m_frame.m_bEnabledGraticule = dlg.m_bEnabledGraticule;
		m_frame.m_graticule.m_fWidth = dlg.m_fGraticuleWidth;
		if(m_frame.m_graticule.m_pColor!=nullptr)
		{
			delete m_frame.m_graticule.m_pColor;
			m_frame.m_graticule.m_pColor = nullptr;
		}
		m_frame.m_graticule.m_pColor = dlg.m_pGraticuleColor;
		dlg.m_pGraticuleColor = nullptr;
		m_frame.m_graticule.m_lngInterval = dlg.m_lngInterval;
		m_frame.m_graticule.m_latInterval = dlg.m_latInterval;

		this->UpdateAllViews(nullptr);
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}