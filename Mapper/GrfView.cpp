#include "stdafx.h"
#include <afxpriv.h>

#include  <cstdio>
#include  <gdiplus.h>

#include "Global.h"

#include "../Viewer/Global.h"

#include "GrfView.h"
#include "GrfDoc.h"
#include "GisDoc.h"
#include "MainFrm.h"
#include "ChildFrm.h"

#include "../Tool/Global.h"
#include "../Tool/Tool.h"
#include "../Tool/SelectTool.h"
#include "../Tool/PanTool.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Text.h"
#include "../Geometry/TextPoly.h"
#include "../Geometry/Mark.h"
#include "../Geometry/Doughnut.h"
#include "../Geometry/OleObj.h"
#include "../Geometry/OleItem.h"
#include "../Geometry/Global.h"
#include "../Geometry/TextEditDlg.h"

#include "../Style/Line.h"
#include "../Style/Type.h"
#include "../Style/Spot.h"
#include "../Style/SpotCtrl.h"
#include "../Layer/Global.h"
#include "../Layer/LayerTreeCtrl.h"
#include "../Layer/LayerPane.h"
#include "../Style/TypeDlg.h"
#include "../Style/StyleDlg.h"

#include "../Information/Global.h"
#include "../Information/TDBGridCtrl.h"
#include "../Information/POUDBGridCtrl.h"
#include "../Information/POUTDBGridCtrl.h"
#include "../Information/ATTDBGridCtrl.h"
#include "../Information/ATTTDBGridCtrl.h"
#include "../Information/TablePane.h"

#include "../Atlas/Deck.h"

#include "../Projection/Projection1.h"

#include "../Dataview/Global.h"
#include "../Dataview/Background.h"

#include "../Topology/TopoCtrl.h"
#include "../Topology/TopoPane.h"
#include "../Topology/Node.h"
#include "../Topology/Edge.h"

#include "../Public/PropertyPane.h"
#include "../Public/Enumeration.h"
#include "../Public/ValueCounter.h"
#include "../Public/ODBCDatabase.h"

#include "../Action/Append.h"
#include "../Action/Remove.h"
#include "../Action/Document/LayerTree/Layerlist/Layer.h"
#include "../Utility/Rect.hpp"
#include "../Utility/array.hpp"

#define IDC_MAINBAR_TAB 100

extern __declspec(dllimport) CSelectTool selectTool;
extern __declspec(dllimport) CPanTool panTool;
extern __declspec(dllimport) CPanTool datumTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UNIT a_UnitArray[3];
extern BYTE a_nUnitIndex;
extern float a_fJoinTolerance;
extern float a_fTopoTolerance;
extern float a_fDensity;
extern CString a_strUnitType;

extern __declspec(dllimport) CArray<Gdiplus::RectF, Gdiplus::RectF&> d_oRectArray;

IMPLEMENT_GWDYNCREATE(CGrfView, CView)

BEGIN_MESSAGE_MAP(CGrfView, CView)
	//{{AFX_MSG_MAP(CGrfView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()

	ON_COMMAND_RANGE(ID_ARRANGE_TOFRONT, ID_ARRANGE_BACKWARD, OnArrange)
	ON_COMMAND_RANGE(ID_SELECTALL_ALL, ID_SELECTALL_REBEL, OnSelectAllByType)
	ON_COMMAND(ID_SELECTALL_SAMESTYLE, OnSelectAllSameStyle)
	ON_COMMAND(ID_SELECTALL_IDENTICAL, OnSelectAllIdentical)
	ON_COMMAND_RANGE(ID_VIEW_LABELOID, ID_VIEW_GEOGROID, OnShowCentroid)
	ON_COMMAND_RANGE(ID_VIEW_CREATEDTAGS, ID_VIEW_DYNAMICTAGS, OnShowTags)
	ON_COMMAND(ID_GEOM_JOIN, OnGeomJoin)

	ON_COMMAND(ID_GEOM_SETSTYLE, OnGeomSetStyle)
	ON_COMMAND(ID_GEOM_SETSPOT, OnGeomSetSpot)
	ON_COMMAND(ID_GEOM_SETTYPE, OnGeomSetType)
	ON_COMMAND(ID_GEOM_SETHINT, OnGeomSetHint)
	ON_COMMAND(ID_GEOM_REGRESS, OnGeomRegress)
	ON_COMMAND(ID_GEOM_PROPERTY, OnGeomProperty)
	ON_COMMAND(ID_VIEW_GRID, OnViewGrid)
	ON_COMMAND(ID_MAGNIFY_ZOOMIN, OnMagnifyZoomin)
	ON_COMMAND(ID_MAGNIFY_ZOOMOUT, OnMagnifyZoomout)
	ON_COMMAND(ID_MAGNIFY_FITINPAGE, OnMagnifyFitinPage)
	ON_COMMAND(ID_MAGNIFY_FITACTIVE, OnMagnifyFitActive)
	ON_COMMAND(ID_MAGNIFY_PAPER1TO1, OnMagnifyPaper1to1)
	ON_COMMAND(ID_MAGNIFY_SCREEN1TO1, OnMagnifyScreen1to1)
	ON_COMMAND(ID_SELECTALL_DISCARD, OnSelectAllDiscard)
	ON_COMMAND(ID_EDIT_INSERTOLE, OnEditInsertOle)
	ON_COMMAND(ID_SELECTALL_INVERSE, OnSelectAllInverse)
	ON_COMMAND(ID_SELECTALL_WITHNAME, OnSelectAllByName)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)

	ON_MESSAGE(WM_REMOVEGEOM, OnRemovGeomtry)
	ON_MESSAGE(WM_LAYERACTIVATED, OnLayerActivated)
	ON_MESSAGE(WM_GETPAPERSIZE, OnGetPaperSize)
	ON_MESSAGE(WM_GETACTIVERECT, OnGetActiveRect)
	ON_MESSAGE(WM_GETACTIVEAREA, OnGetActiveArea)
	ON_MESSAGE(WM_CAPTUREIMAGE, OnCaptureImage)
	ON_MESSAGE(WM_GETVIEWINFO, OnGetViewinfo)
	ON_MESSAGE(WM_ACTIVEGEOM, OnActiveGeom)
	ON_MESSAGE(WM_SETPRINTARECT, OnSetPrintRect)
	ON_MESSAGE(WM_DOCMODIFIED, OnDocModified)
	ON_MESSAGE(WM_ZOOMIN, OnZoomIn)
	ON_MESSAGE(WM_ZOOMOUT, OnZoomOut)
	ON_MESSAGE(WM_TRANSFORM, OnTransform)
	ON_MESSAGE(WM_TRANSFORMED, OnTransformed)
	ON_MESSAGE(WM_MOVEGEOM, OnMoveGeomtry)
	ON_MESSAGE(WM_PICKACTIVEGEOM, OnPickActiveGeom)
	ON_MESSAGE(WM_PICKANCHOR, OnPickAnchor)
	ON_MESSAGE(WM_PICKGEOM, OnPickGeom)
	ON_MESSAGE(WM_PICKBYRECT, OnPickByRect)
	ON_MESSAGE(WM_PICKBYCIRCLE, OnPickByCircle)
	ON_MESSAGE(WM_PICKBYPOLY, OnPickByPoly)
	ON_MESSAGE(WM_SPLITGEOM, OnSplitGeom)
	ON_MESSAGE(WM_LOOKUPGEOM, OnLookupGeom)

	ON_MESSAGE(WM_FIXATEVIEW, OnFixateView)

	ON_MESSAGE(WM_NEWGEOMDREW, OnNewGeomDrew)
	ON_MESSAGE(WM_NEWGEOMBRED, OnNewGeomBred)
	ON_MESSAGE(WM_TOREMOVEEDGE, OnToRemoveEdge)
	ON_MESSAGE(WM_PREREMOVEGEOM, OnPreRemoveGeom)
	ON_MESSAGE(WM_PREDELETEGEOM, OnPreDeleteGeom)
	ON_MESSAGE(WM_PREREMOVELAYER, OnPreRemoveLayer)
	ON_MESSAGE(WM_PREDELETELAYER, OnPreDeleteLayer)

	ON_MESSAGE(WM_APPLYGEOMID, OnApplyGeomID)

	ON_MESSAGE(WM_KEYQUERY, OnKeyQuery)
	ON_MESSAGE(WM_FLASHGEOM, OnFlashGeom)

	ON_MESSAGE(WM_TOPO_PICKBYPOLYGON, OnTopoPickByPolygon)
	ON_MESSAGE(WM_TOPO_PICK, OnTopoPick)
	ON_MESSAGE(WM_TOPO_SPLIT, OnTopoSplit)
	ON_MESSAGE(WM_TOPO_CROSS, OnTopoCross)

	ON_MESSAGE(WM_PUBLISHPC, OnPublishPC)
	ON_MESSAGE(WM_PUBLISHCE, OnPublishCE)
END_MESSAGE_MAP()

CGrfView::CGrfView(CObject& parameter) noexcept
	: m_editgeom(m_viewMonitor, ((CGrfDoc&)parameter).m_actionStack), m_document((CGrfDoc&)parameter), m_viewMonitor(((CGrfDoc&)parameter).m_Datainfo), m_viewPrinter(((CGrfDoc&)parameter).m_Datainfo)
{
	m_hDrawOver = CreateEvent(nullptr, TRUE, FALSE, nullptr);;
	ResetEvent(m_hDrawOver);

	m_editgeom.m_pGeom = nullptr;

	m_viewMonitor.m_bViewer = false;
	m_viewPrinter.m_bViewer = false;
}

CGrfView::~CGrfView()
{
	CMainFrame* pAppFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if(pAppFrame!=nullptr)
	{
		bool bLastOne = true;
		CWnd* pMDIClientAreaWnd = pAppFrame->GetWindow(GW_CHILD);
		while(pMDIClientAreaWnd!=nullptr)
		{
			const CRuntimeClass* pRuntimeClass = pMDIClientAreaWnd->GetRuntimeClass();
			if(strcmp(pRuntimeClass->m_lpszClassName, "CMDIClientAreaWnd")==0)
				break;

			pMDIClientAreaWnd = pMDIClientAreaWnd->GetNextWindow(GW_HWNDNEXT);
		}

		if(pMDIClientAreaWnd!=nullptr)
		{
			CWnd* pChildFrame = pMDIClientAreaWnd->GetWindow(GW_CHILD);
			while(pChildFrame!=nullptr)
			{
				if(pChildFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
				{
					CView* pView = ((CMDIChildWnd*)pChildFrame)->GetActiveView();
					if(pView!=nullptr&&pView!=this)
					{
						bLastOne = false;
						break;
					}
				}
				pChildFrame = pChildFrame->GetNextWindow(GW_HWNDNEXT);
			}
		}
		if(bLastOne==true)
		{
			pAppFrame->m_wndDrawTool.SwitchSource(nullptr);
		}
	}
}

CStatusBarEx* CGrfView::GetStatusBar()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CWnd* pp = AfxGetMainWnd();
	if(pMainFrame!=nullptr)
		return &(pMainFrame->GetStatusBar());
	else
		return nullptr;
}

BOOL CGrfView::PreCreateWindow(CREATESTRUCT& cs)
{
	CView::PreCreateWindow(cs);

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

	return TRUE;
}

void CGrfView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	const CClientDC dc(this);
	m_viewMonitor.ResetProjection(nullptr, false);
	m_viewMonitor.m_sizeDPI = CSize(dc.GetDeviceCaps(LOGPIXELSX), dc.GetDeviceCaps(LOGPIXELSY));
	m_viewMonitor.m_sizeDPI = CSize(72, 72);
	const CRectF mapRect = m_viewMonitor.m_datainfo.GetMapRect();
	m_viewMonitor.ScaleTo(this, m_document.m_Datainfo.m_scaleSource, mapRect.CenterPoint());
	CChildFrame* pChildfrm = (CChildFrame*)GetParentFrame();
	pChildfrm->ResetRuler(m_viewMonitor);

	this->SendMessage(WM_LAYERACTIVATED, 0, 0);
}

BOOL CGrfView::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	this->GetClientRect(rect);
	if(m_viewMonitor.m_pBackground!=nullptr)
		pDC->FillSolidRect(rect, RGB(255, 255, 254));
	else
		pDC->FillSolidRect(rect, RGB(255, 255, 255));
	return TRUE;
}
CRect CGrfView::GetVisibleDocRect() const
{
	CRect rect1;
	this->GetClientRect(rect1);
	CRect rect2 = CRect(-m_viewMonitor.m_ptViewPos, m_viewMonitor.m_sizeView);
	CRect rect3;
	rect3.IntersectRect(rect1, rect2);
	rect3.DeflateRect(1, 1);
	return m_viewMonitor.ClientToDoc(rect3);

}
void CGrfView::OnDraw(CDC* pDC)
{
	d_oRectArray.RemoveAll();

	if(pDC->IsPrinting()==FALSE)
	{
		CRect clipBox;
		pDC->GetClipBox(clipBox);
		clipBox.NormalizeRect();
		if(clipBox.IsRectNull())
			return;

		m_viewMonitor.m_sizeDPI = CSize(pDC->GetDeviceCaps(LOGPIXELSX), pDC->GetDeviceCaps(LOGPIXELSY));
		m_viewMonitor.m_sizeDPI = CSize(72, 72);
		if(m_document.m_pTool!=nullptr)
		{
			m_document.m_pTool->Draw(this, m_viewMonitor, pDC);
		}

		Gdiplus::Bitmap bitmap(clipBox.Width(), clipBox.Height(), PixelFormat32bppARGB);
		Gdiplus::Graphics g1(&bitmap);
		g1.SetPageUnit(Gdiplus::Unit::UnitPixel);
		const HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();
		const LONG nStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
		if((nStyle&WS_EX_LAYERED)!=WS_EX_LAYERED)
		{
			g1.Clear(m_document.m_argbBackground);
			g1.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
			g1.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQualityBicubic);
			g1.SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHighQuality);
			g1.SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintAntiAlias);
		}
		else
		{
			g1.Clear(Gdiplus::Color(255, 255, 254));
		}
		g1.TranslateTransform(-clipBox.left, -clipBox.top);

		CRect inRect = CRect(-m_viewMonitor.m_ptViewPos, m_viewMonitor.m_sizeView);
		inRect.IntersectRect(inRect, clipBox);
		inRect.InflateRect(1, 1);
		inRect = m_viewMonitor.ClientToDoc(inRect);

		m_viewMonitor.DrawGrid(g1, inRect);
		m_viewMonitor.DrawCanvas1(g1);

		m_document.Draw(this, m_viewMonitor, g1, inRect);
		POSITION pos1 = m_hlGeomList.GetHeadPosition();
		while(pos1!=nullptr)
		{
			CGeom* pGeom = m_hlGeomList.GetNext(pos1);
			CLayer* pLayer = m_document.m_layertree.GetLayerByGeom(pGeom);
			const int width = pLayer==nullptr ? 10 : max(10, pLayer->GetVewInflation(m_viewMonitor, pGeom).Width);
			pGeom->DrawHatch(g1, m_viewMonitor, width);
		}
		m_document.DrawTag(this, m_viewMonitor, g1, inRect);
		m_viewMonitor.DrawCanvas2(g1);
		Gdiplus::Graphics g2(pDC->m_hDC);
		g2.DrawImage(&bitmap, clipBox.left, clipBox.top, clipBox.Width(), clipBox.Height());
		g2.ReleaseHDC(pDC->m_hDC);

		if(m_document.m_pTool!=nullptr&&(GetAsyncKeyState(VK_LBUTTON)&0X8000)==0X8000)// VK_LBUTTON is down
		{
			m_document.m_pTool->Draw(this, m_viewMonitor, pDC);
		}
		if(m_document.preference.ShowLabeloid||m_document.preference.ShowGeogroid)
		{
			for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
			{
				CLayer* layer = *it;
				if(layer->m_bVisible==false)
					continue;

				layer->DrawCentroid(pDC, m_viewMonitor, inRect, m_document.preference.ShowLabeloid, m_document.preference.ShowGeogroid);
			}
		}
		const CPoint oldOrg = pDC->SetViewportOrg(0, 0);
		for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			layer->DrawAnchors(pDC, m_viewMonitor, inRect);
		}

		m_editgeom.SwitchAnchorsFocused(pDC);

		pDC->SetViewportOrg(oldOrg);

		SetEvent(m_hDrawOver);
	}
	else
	{
		m_viewPrinter.m_sizeDPI = CSize(pDC->GetDeviceCaps(LOGPIXELSX), pDC->GetDeviceCaps(LOGPIXELSY));
		m_viewPrinter.ResetProjection(m_viewMonitor.m_pGeocentric, true);

		CRect clipBox;
		pDC->GetClipBox(&clipBox);
		CRect inRect = m_viewPrinter.ClientToDoc(clipBox);
		if(inRect.IsRectEmpty()==TRUE)
			return;

		Gdiplus::Graphics g(pDC->m_hDC);
		g.SetPageUnit(Gdiplus::Unit::UnitPixel);
		g.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
		g.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQualityBicubic);
		g.SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHighQuality);
		g.SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintAntiAlias);

		m_document.Draw(this, m_viewPrinter, g, inRect);
		m_document.DrawTag(this, m_viewPrinter, g, inRect);
		m_viewPrinter.m_pGeocentric = nullptr;
		g.ReleaseHDC(pDC->m_hDC);
	}
	d_oRectArray.RemoveAll();
}

CLayer* CGrfView::GetActiveLayer() const
{
	return m_document.m_LayerTreeCtrl.GetSelLayer();
}
/////////////////////////////////////////////////////////////////////////////
// CGrfView message handlers
void CGrfView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CView::OnLButtonDown(nFlags, point);

	if(m_document.m_pTool!=nullptr)
	{
		const CPoint docPoint = m_viewMonitor.ClientToDoc(point, false);
		m_document.m_pTool->OnLButtonDown(this, m_viewMonitor, nFlags, point, docPoint);
	}
}

void CGrfView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CView::OnLButtonUp(nFlags, point);

	if(CWnd::GetCapture()!=this)
	{
	}
	else if(m_document.m_pTool!=nullptr)
	{
		const CPoint docPoint = m_viewMonitor.ClientToDoc(point, true);
		m_document.m_pTool->OnLButtonUp(this, m_viewMonitor, nFlags, point, docPoint);
	}
	if(GetKeyState(VK_CONTROL)>=0&&m_document.m_pTool==&selectTool&&m_document.m_oldTool!=nullptr)
	{
		m_document.m_pTool = m_document.m_oldTool;
		m_document.m_oldTool = nullptr;
		m_document.m_pTool->SetCursor(this, m_viewMonitor);
	}
}

void CGrfView::OnMouseMove(UINT nFlags, CPoint point)
{
	CView::OnMouseMove(nFlags, point);

	CChildFrame* pChildfrm = (CChildFrame*)GetParentFrame();
	pChildfrm->m_ruler.OnMouseMove(pChildfrm->m_wndSplitter.m_hWnd==nullptr ? (CWnd*)pChildfrm : &pChildfrm->m_wndSplitter, point);
	const CPoint docPoint = m_viewMonitor.ClientToDoc(point, false);

	CStatusBarEx* pStatusBar = this->GetStatusBar();
	if(pStatusBar!=nullptr)
	{
		CString strX;
		CString stry;
		if(a_strUnitType==_T("Map"))
		{
			const CPointF mapPoint = m_document.m_Datainfo.DocToMap(docPoint);
			strX.Format(_T("X: %g"), mapPoint.x);
			stry.Format(_T("Y: %g"), mapPoint.y);
		}
		else if(a_strUnitType==_T("Geography")&&m_document.m_Datainfo.m_pProjection!=nullptr)
		{
			const CPointF geoPoint = m_document.m_Datainfo.DocToWGS84(docPoint);

			strX.Format(_T("X: %g"), geoPoint.x*constants::radianTodegree);
			stry.Format(_T("Y: %g"), geoPoint.y*constants::radianTodegree);
		}
		pStatusBar->SetPaneText(indexCOORDINATE, strX+" "+stry);
	}
	if(GetKeyState(VK_SPACE)<0&&m_document.m_pTool!=&panTool&&GetKeyState(VK_LBUTTON)>=0)
	{
		SetCapture();

		m_document.m_oldTool = m_document.m_pTool;
		m_document.m_pTool = &panTool;
		m_document.m_pTool->SetCursor(this, m_viewMonitor);
	}
	else if(m_document.m_pTool!=nullptr)
	{
		m_document.m_pTool->OnMouseMove(this, m_viewMonitor, nFlags, point, docPoint);
	}
}

void CGrfView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CView::OnLButtonDblClk(nFlags, point);

	if(m_document.m_pTool!=nullptr)
	{
		const CPoint docPoint = m_viewMonitor.ClientToDoc(point, false);
		m_document.m_pTool->OnLButtonDblClk(this, m_viewMonitor, nFlags, docPoint);
	}
}

void CGrfView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CView::OnRButtonDown(nFlags, point);

	if((nFlags&MK_LBUTTON)==MK_LBUTTON)
		return;

	if(m_document.m_pTool!=nullptr&&m_document.m_pTool==&m_document.m_datumTool)
	{
		const CPoint docPoint = m_viewMonitor.ClientToDoc(point, false);
		m_document.m_pTool->OnRButtonDown(this, m_viewMonitor, nFlags, docPoint);
	}
	else
	{
		CLayer* pLayer = nullptr;
		CGeom* pActivated = m_document.GetTheActivatedGeom(pLayer);
		if(pActivated!=nullptr&&pActivated->m_bClosed==TRUE)
		{
			CMenu Menu;
			if(Menu.LoadMenu(IDR_POPUP))
			{
				CMenu* pSubMenu = Menu.GetSubMenu(0);
				if(pSubMenu!=nullptr)
				{
					CPoint scrPoint(point.x, point.y);
					this->ClientToScreen(&scrPoint);
					const UINT selection = pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD, scrPoint.x, scrPoint.y, this);
					pSubMenu->DestroyMenu();

					const CPoint docPoint = m_viewMonitor.ClientToDoc(point, false);
					switch(selection)
					{
						case ID_TOP_SETLABELOID:
							pActivated->SetLabeloid(docPoint);
							pActivated->Invalid(m_document);
							break;
						case ID_TOP_SETGEOGROID:
							pActivated->SetGeogroid(docPoint);
							pActivated->Invalid(m_document);
							break;
					}
				}
				Menu.DestroyMenu();
			}
		}
	}
}

void CGrfView::OnRButtonUp(UINT nFlags, CPoint point)
{
	CView::OnRButtonUp(nFlags, point);

	if(m_document.m_pTool!=nullptr)
	{
		const CPoint docPoint = m_viewMonitor.ClientToDoc(point, false);
		m_document.m_pTool->OnRButtonUp(this, m_viewMonitor, nFlags, docPoint);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGrfView diagnostics
#ifdef _DEBUG
void CGrfView::AssertValid() const
{
	CView::AssertValid();
}

void CGrfView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

BOOL CGrfView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if(pWnd==this&&nHitTest==HTCLIENT&&m_document.m_pTool!=nullptr)
	{
		if(m_viewMonitor.m_pBackground!=nullptr&&m_document.m_pTool==&panTool)
			return TRUE;
		else
			return m_document.m_pTool->SetCursor(this, m_viewMonitor);
	}
	else
		return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CGrfView::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType)
{
	const CChildFrame* pChildfrm = (CChildFrame*)GetParentFrame();
	if(pChildfrm->m_ruler.m_bVisible)
	{
		lpClientRect->left = 10;
		lpClientRect->top = 10;
	}

	CView::CalcWindowRect(lpClientRect, nAdjustType);
}

void CGrfView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if(m_hWnd!=nullptr)
	{
		m_viewMonitor.OnSized(this, cx, cy);
	}
}

BOOL CGrfView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
	CView::OnScrollBy(sizeScroll, bDoScroll);

	switch(bDoScroll)
	{
		case 0:
			m_viewMonitor.MoveBy(this, sizeScroll.cx, sizeScroll.cy);
			break;
		case 1:
			m_viewMonitor.MoveBy(this, sizeScroll.cx, sizeScroll.cy);
			break;
		case 2:
			m_viewMonitor.SpinBy(this, sizeScroll.cx*constants::radianTodegree/100000000.f, sizeScroll.cy*constants::radianTodegree/100000000.f);
			RefreshPreviewCenter();
			break;
	}
	CChildFrame* pChildfrm = (CChildFrame*)GetParentFrame();
	if(sizeScroll.cy!=0)
	{
		const Gdiplus::Point start = m_viewMonitor.DataMapToClient<Gdiplus::Point>(CPointF(0, 0));
		pChildfrm->m_ruler.DrawV(this, &pChildfrm->m_wndSplitter, start);
	}
	if(sizeScroll.cx!=0)
	{
		const Gdiplus::Point start = m_viewMonitor.DataMapToClient<Gdiplus::Point>(CPointF(0, 0));
		pChildfrm->m_ruler.DrawH(this, &pChildfrm->m_wndSplitter, start);
	}

	if(bDoScroll==TRUE)
	{
		CClientDC dc(this);
		OnPrepareDC(&dc, nullptr);

		//	OnEraseBkgnd(&dc);
		//	OnDraw(&dc);
	}

	return TRUE;
}

void CGrfView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(m_document.m_pTool!=nullptr)
	{
		m_document.m_pTool->OnKeyDown(this, m_viewMonitor, nChar, nRepCnt, nFlags, m_document.m_pTool, m_document.m_oldTool);
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGrfView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(m_document.m_pTool!=nullptr)
	{
		m_document.m_pTool->OnKeyUp(this, m_viewMonitor, nChar, nRepCnt, nFlags, m_document.m_pTool, m_document.m_oldTool);
	}

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CGrfView::OnViewGrid()
{
	m_viewMonitor.m_bShowGrid = 1-m_viewMonitor.m_bShowGrid;
}
void CGrfView::OnMagnifyZoomin()
{
	CRect cliRect;
	this->GetClientRect(cliRect);
	m_viewMonitor.ZoomBy(this, 2, cliRect.CenterPoint());
}
void CGrfView::OnMagnifyZoomout()
{
	CRect cliRect;
	this->GetClientRect(cliRect);
	m_viewMonitor.ZoomBy(this, 0.5, cliRect.CenterPoint());
}
void CGrfView::OnMagnifyFitinPage()
{
	CRect cliRect;
	this->GetClientRect(cliRect);
	const float xZoom = (float)cliRect.Width()/m_viewMonitor.m_mapCanvas.cx;
	const float yZoom = (float)cliRect.Height()/m_viewMonitor.m_mapCanvas.cy;
	const float fZoom = min(xZoom, yZoom);
	m_viewMonitor.RatioTo(this, fZoom, cliRect.CenterPoint());
	CStatusBarEx* pStatusBar = this->GetStatusBar();
	if(pStatusBar!=nullptr)
	{
		pStatusBar->m_LevelBar.SetLevel(m_viewMonitor.m_levelCurrent);
		pStatusBar->m_ScaleBar.SetScale(m_viewMonitor.m_scaleCurrent);
	}

	CChildFrame* pChildfrm = (CChildFrame*)GetParentFrame();
	pChildfrm->ResetRuler(m_viewMonitor);
}

void CGrfView::OnGeomJoin()
{
	std::function<bool(WORD wLayer, DWORD dwPoly1, DWORD dwPoly2, const BYTE& mode)> joinable = [&](WORD wLayer, DWORD dwPoly1, DWORD dwPoly2, const BYTE& mode)
	{
		if(m_document.m_topolist.size()==0)
			return true;

		bool fine = true;
		for(std::list<CTopology*>::iterator it = m_document.m_topolist.begin(); it!=m_document.m_topolist.end(); ++it)
		{
			CTopology* pTopology = *it;
			if(pTopology->EdgeJoinable(wLayer, dwPoly1, dwPoly2, mode)==false)
				return false;
		}
		return true;
	};
	std::function<void(WORD wLayer, DWORD dwPoly1, DWORD dwPoly2, const BYTE& mode)> joined = [&](WORD wLayer, DWORD dwPoly1, DWORD dwPoly2, const BYTE& mode)
	{
		if(m_document.m_topolist.size()==0)
			return;

		bool fine = true;
		for(std::list<CTopology*>::iterator it = m_document.m_topolist.begin(); it!=m_document.m_topolist.end(); ++it)
		{
			CTopology* pTopology = *it;
			DWORD dwEdgeId1 = -1;
			DWORD dwEdgeId2 = -1;
			pTopology->PolyJoined(this, this->m_document.m_Datainfo, m_viewMonitor, wLayer, dwPoly1, mode, dwPoly2, dwEdgeId1, dwEdgeId2);
		}
	};


	m_editgeom.UnFocusAll(this);

	AfxGetApp()->BeginWaitCursor();

	m_document.m_actionStack.Start();
	for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount==0)
			continue;
		const unsigned int& tolerance = m_document.m_Datainfo.m_zoomPointToDoc*a_fJoinTolerance;
		if(true) //Èç¹û¸Ã²ãÉÏ½¨Á¢ÁËÍØÆË¹ØÏµ£¬²»ÄÜÁ¬½Ó
		{
			layer->Join(this, m_viewMonitor, tolerance, m_document.m_actionStack, joinable, joined);
		}
	}
	m_document.m_actionStack.Stop();

	AfxGetApp()->EndWaitCursor();
	SendMessage(WM_DOCMODIFIED, 0, TRUE);
}

void CGrfView::OnArrange(UINT dwId)
{
	ARRANGE arrange;
	switch(dwId)
	{
		case ID_ARRANGE_TOFRONT:
			arrange = ARRANGE::Tofront;
			break;
		case ID_ARRANGE_TOBACK:
			arrange = ARRANGE::Toback;
			break;
		case ID_ARRANGE_FORWARD:
			arrange = ARRANGE::Forward;
			break;
		case ID_ARRANGE_BACKWARD:
			arrange = ARRANGE::Backward;
			break;
	}

	AfxGetApp()->BeginWaitCursor();
	m_document.m_actionStack.Start();
	for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount==0)
			continue;

		layer->Arrange(this, m_viewMonitor, m_document.m_actionStack, arrange);
	}
	m_document.m_actionStack.Stop();
	AfxGetApp()->EndWaitCursor();
	SendMessage(WM_DOCMODIFIED, 0, TRUE);
}

void CGrfView::OnSetFocus(CWnd* pOldWnd)
{
	COleClientItem* pActiveItem = m_document.GetInPlaceActiveItem(this);
	if(pActiveItem!=nullptr&&pActiveItem->GetItemState()==COleClientItem::activeUIState)
	{
		// need to set focus to this item if it is in the same view
		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
		if(pWnd!=nullptr)
		{
			pWnd->SetFocus(); // don't call the base class
			return;
		}
	}

	CView::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
}

void CGrfView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CGrfView::OnPreparePrinting(CPrintInfo* pInfo)
{
	//	CView::DoPreparePrinting(pInfo);

	return m_viewPrinter.DoPreparePrinting(this, pInfo);
}

void CGrfView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CView::OnBeginPrinting(pDC, pInfo);

	m_viewPrinter.BeginPrinting(pDC, pInfo);
}

void CGrfView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	AfxGetApp()->BeginWaitCursor();
	m_viewPrinter.Print(this, pDC, pInfo);
	AfxGetApp()->EndWaitCursor();

	CView::OnPrint(pDC, pInfo);
}

void CGrfView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	//	if(pInfo->m_bPreview == FALSE)
	{
		m_viewPrinter.m_docPrint.SetRectEmpty();
	}
	CView::OnEndPrinting(pDC, pInfo);
}

void CGrfView::OnSelectAllByType(UINT dwId)
{
	CRect inRect = GetVisibleDocRect();

	ACTIVEALL activeall = ACTIVEALL::All;
	switch(dwId)
	{
		case ID_SELECTALL_INVERSE:
			activeall = ACTIVEALL::Inverse;
			break;
		case ID_SELECTALL_ALL:
			activeall = ACTIVEALL::All;
			break;
		case ID_SELECTALL_AREA:
			activeall = ACTIVEALL::Area;
			break;
		case ID_SELECTALL_TEXT:
			activeall = ACTIVEALL::Text;
			break;
		case ID_SELECTALL_MARK:
			activeall = ACTIVEALL::Mark;
			break;
		case ID_SELECTALL_LINE:
			activeall = ACTIVEALL::Line;
			break;
		case ID_SELECTALL_NONAME:
			activeall = ACTIVEALL::NoName;
			break;
		case ID_SELECTALL_GROUP:
			activeall = ACTIVEALL::Group;
			break;
		case ID_SELECTALL_DONUT:
			activeall = ACTIVEALL::Doughnut;
			break;
		case ID_SELECTALL_IMAGE:
			activeall = ACTIVEALL::Image;
			break;
		case ID_SELECTALL_BEZIER:
			activeall = ACTIVEALL::Bezier;
			break;
		case ID_SELECTALL_HASNAME:
			activeall = ACTIVEALL::HasName;
			break;
		case ID_SELECTALL_WITHNAME:
			activeall = ACTIVEALL::WithName;
			break;
		case ID_SELECTALL_POLYTEXT:
			activeall = ACTIVEALL::PolyText;
			break;
		case ID_SELECTALL_CLIP:
			activeall = ACTIVEALL::Clip;
			break;
		case ID_SELECTALL_MASK:
			activeall = ACTIVEALL::Mask;
			break;
		case ID_SELECTALL_REBEL:
			activeall = ACTIVEALL::Rebel;
			break;
	}

	AfxGetApp()->BeginWaitCursor();
	if(m_document.m_LayerTreeCtrl.m_bMulti==false)
	{
		CLayer* layer = GetActiveLayer();
		if(layer!=nullptr&&layer->m_bVisible)
		{
			layer->ActivateAll(this, activeall, m_document.m_Datainfo, m_viewMonitor, inRect);
		}
	}
	else
	{
		for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			layer->ActivateAll(this, activeall, m_document.m_Datainfo, m_viewMonitor, inRect);
		}
	}
	m_document.m_LayerTreeCtrl.UpdateSelect();
	AfxGetApp()->EndWaitCursor();
}

void CGrfView::InactivateAll()
{
	CRect inRect = GetVisibleDocRect();

	m_editgeom.UnFocusAll(this);

	AfxGetApp()->BeginWaitCursor();
	for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		layer->InactivateAll(this, m_document.m_Datainfo, m_viewMonitor, inRect);
	}
	AfxGetApp()->EndWaitCursor();
}

void CGrfView::UnhighligtAll()
{
	POSITION pos = m_hlGeomList.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_hlGeomList.GetNext(pos);
		CLayer* pLayer = m_document.m_layertree.GetLayerByGeom(pGeom);
		if(pLayer==nullptr)
			continue;
		pLayer->Invalidate(this, m_viewMonitor, pGeom);
	}
	m_hlGeomList.RemoveAll();
}

LONG CGrfView::OnTransform(UINT WPARAM, LONG LPARAM)
{
	const double* a = (double*)WPARAM;
	double m11, m21, m31, m12, m22, m32;
	m11 = a[0], m21 = a[1], m31 = a[2], m12 = a[3], m22 = a[4], m32 = a[5];
	Gdiplus::Matrix matrixTransform(m11, m12, m21, m22, m31, m32);

	CClientDC dc(this);

	AfxGetApp()->BeginWaitCursor();
	for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		layer->Transform(&dc, m_viewMonitor, m11, m21, m31, m12, m22, m32);
	}
	AfxGetApp()->EndWaitCursor();

	return 0L;
}

LONG CGrfView::OnTransformed(UINT WPARAM, LONG LPARAM)
{
	const double* a = (double*)WPARAM;
	double m11, m21, m31, m12, m22, m32;
	m11 = a[0], m21 = a[1], m31 = a[2], m12 = a[3], m22 = a[4], m32 = a[5];

	AfxGetApp()->BeginWaitCursor();
	m_document.m_actionStack.Start();
	for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		layer->Transform(this, m_viewMonitor, m_document.m_actionStack, m11, m21, m31, m12, m22, m32);
	}
	m_document.m_actionStack.Stop();
	AfxGetApp()->EndWaitCursor();

	SendMessage(WM_DOCMODIFIED, 0, TRUE);
	return 0L;
}

LONG CGrfView::OnLookupGeom(UINT WPARAM, LONG LPARAM)
{
	const CPoint point = *(CPoint*)WPARAM;

	for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it!=m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(LPARAM==1)
		{
			CATTDatasource* pDatasource = layer->GetAttDatasource();
			if(pDatasource==nullptr)
				continue;
			CDatabase* pDatabase = m_document.m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
			if(pDatabase==nullptr)
				continue;
		}

		CGeom* pGeom = nullptr;
		if(LPARAM==3)
		{
			const bool oldVisible = layer->m_bVisible;
			layer->m_bVisible = true;
			pGeom = layer->Look(m_viewMonitor, point);
			layer->m_bVisible = oldVisible;
		}
		else
		{
			if(layer->m_bVisible==false)
				continue;

			pGeom = layer->Look(m_viewMonitor, point);
		}

		if(pGeom!=nullptr)
			return (LONG)pGeom;
	}

	return 0;
}

LONG CGrfView::OnPickGeom(UINT WPARAM, LONG LPARAM)
{
	const CPoint point = *(CPoint*)LPARAM;
	CClientDC dc(this);

	CLayer* slayer = GetActiveLayer();
	for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it!=m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(m_document.m_LayerTreeCtrl.m_bMulti==false&&slayer!=layer)
			continue;
		if(layer->m_bVisible==false)
			continue;

		CGeom* pGeom = layer->Pick(this, m_viewMonitor, point, true);
		if(pGeom!=nullptr)
		{
			if(pGeom->m_bActive==true)
				return (LONG)pGeom;

			if(::GetKeyState(VK_SHIFT)>=0)
				InactivateAll();

			pGeom->m_bActive = true;
			pGeom->DrawAnchors(&dc, m_viewMonitor);
			layer->m_nActiveCount++;

			m_document.m_LayerTreeCtrl.Select(layer);

			//casual			
			CStatusBarEx* pStatusBar = this->GetStatusBar();
			if(pStatusBar!=nullptr)
			{
				CString str1;
				str1.Format(_T("Id: %d"), pGeom->m_geoId);
				CString str2;
				str2.Format(_T("Points: %d"), pGeom->GetAnchors());
				CStringA str3;
				str3.Format("Geo Code: %s", pGeom->m_geoCode);
				pStatusBar->SetPaneText(indexGEOMID, str1);
				pStatusBar->SetPaneText(indexPOINTCOUNT, str2);
				pStatusBar->SetPaneText(indexGEOMNAME, pGeom->m_strName);
				pStatusBar->SetPaneText(indexGEOCODE, CA2T(str3));
			}

			this->SendMessage(WM_GEOMACTIVATED, (UINT)layer, (LONG)pGeom);
			return (LONG)pGeom;
		}
	}

	if(::GetKeyState(VK_SHIFT)>=0)
	{
		InactivateAll();
		m_document.m_LayerTreeCtrl.SelectFixed();
	}

	return 0;
}

LONG CGrfView::OnPickByRect(UINT WPARAM, LONG LPARAM)
{
	const CRect rect = *(CRect*)LPARAM;
	if(m_document.m_LayerTreeCtrl.m_bMulti==false)
	{
		CLayer* layer = GetActiveLayer();
		if(layer!=nullptr&&layer->m_bVisible)
		{
			layer->Pick(this, m_viewMonitor, rect, true);
		}
	}
	else
	{
		for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it!=m_document.m_layertree.forwend(); ++it)
		{
			CLayer* layer = *it;
			if(layer->m_bVisible==false)
				continue;

			layer->Pick(this, m_viewMonitor, rect, true);
		}
		m_document.m_LayerTreeCtrl.UpdateSelect();
	}

	return 0L;
}

LONG CGrfView::OnPickByCircle(UINT WPARAM, LONG LPARAM)
{
	const CPoint center = *(CPoint*)WPARAM;
	const DWORD radius = LPARAM;

	if(m_document.m_LayerTreeCtrl.m_bMulti==false)
	{
		CLayer* layer = GetActiveLayer();
		if(layer!=nullptr&&layer->m_bVisible)
		{
			layer->Pick(this, m_viewMonitor, center, radius, true);
		}
	}
	else
	{
		for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it!=m_document.m_layertree.forwend(); ++it)
		{
			CLayer* layer = *it;
			if(layer->m_bVisible==false)
				continue;
			if(m_viewMonitor.m_levelCurrent<layer->m_minLevelObj)
				continue;
			if(m_viewMonitor.m_levelCurrent>=layer->m_maxLevelObj)
				continue;

			layer->Pick(this, m_viewMonitor, center, radius, true);
		}

		m_document.m_LayerTreeCtrl.UpdateSelect();
	}

	return 0L;
}

LONG CGrfView::OnPickByPoly(UINT WPARAM, LONG LPARAM)
{
	const CPoly& poly = *(CPoly*)WPARAM;

	if(m_document.m_LayerTreeCtrl.m_bMulti==false)
	{
		CLayer* layer = GetActiveLayer();
		if(layer!=nullptr&&layer->m_bVisible)
		{
			layer->Pick(this, m_viewMonitor, poly, true);
		}
	}
	else
	{
		for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it!=m_document.m_layertree.forwend(); ++it)
		{
			CLayer* layer = *it;
			if(layer->m_bVisible==false)
				continue;

			layer->Pick(this, m_viewMonitor, poly, true);
		}

		m_document.m_LayerTreeCtrl.UpdateSelect();
	}

	return 0L;
}

LONG CGrfView::OnLayerActivated(UINT WPARAM, LONG LPARAM)
{
	CLayer* pNewLayer = (CLayer*)WPARAM;
	CLayer* pOldLayer = (CLayer*)LPARAM;

	if(pOldLayer==nullptr)
		return 0;
	if(pNewLayer==nullptr)
		return 0;

	if(pOldLayer==m_editgeom.m_pLayer&&m_editgeom.m_pGeom!=nullptr)
	{
		const POSITION pos = pNewLayer->m_geomlist.Find(m_editgeom.m_pGeom);
		if(pos!=nullptr)
			m_editgeom.m_pLayer = pNewLayer;
	}

	return 0;
}

LONG CGrfView::OnNewGeomDrew(UINT WPARAM, LONG LPARAM)
{
	CGeom* pGeom = (CGeom*)WPARAM;
	if(pGeom==nullptr)
		return FALSE;

	if(CLayer* layer = GetActiveLayer(); layer!=nullptr)
	{
		this->InactivateAll();

		if(layer->NewGeom(this, m_document.m_Datainfo, pGeom, m_document.m_actionStack)==true)
		{
			m_editgeom.m_pLayer = layer;
			pGeom->m_bActive = true;
			pGeom->m_geoId = m_document.ApplyGeomId();
			SendMessage(WM_DOCMODIFIED, 0, TRUE);
			layer->Invalidate(this, m_viewMonitor, pGeom);
			return TRUE;
		}
		else
		{
			delete pGeom;
			pGeom = nullptr;
			return FALSE;
		}
	}
	else
	{
		delete pGeom;
		pGeom = nullptr;
		ReleaseCapture();
		AfxMessageBox(_T("No layer selected yet!"));
		return FALSE;
	}
}

LONG CGrfView::OnNewGeomBred(UINT WPARAM, LONG LPARAM)
{
	CGeom* pGeom = (CGeom*)LPARAM;
	if(pGeom!=nullptr)
	{
		pGeom->m_geoId = m_document.ApplyGeomId();
	}

	return 0;
}

LONG CGrfView::OnSplitGeom(UINT WPARAM, LONG LPARAM)
{
	std::function<void(CLayer* pLayer, DWORD dwPoly, const CPoint& point, DWORD dwBred)> splitted = [&](CLayer* pLayer, DWORD dwPoly, const CPoint& point, DWORD dwBred) {
		DWORD dwOldEdgeId = -1;
		DWORD dwNewEdgeId = -1;
		for(std::list<CTopology*>::reverse_iterator it = m_document.m_topolist.rbegin(); it!=m_document.m_topolist.rend(); it++)
		{
			CTopology* pTopology = *it;
			DWORD dwOldEdgeId = -1;
			DWORD dwNewEdgeId = -1;
			if(pTopology->PolySplitted(this, m_viewMonitor, pLayer->m_wId, dwPoly, point, dwBred, dwOldEdgeId, dwNewEdgeId))//to do: open the last action and add the edge changes in
			{
				CRect rect;
				GetClientRect(rect);
				const CRect inRect = m_viewMonitor.ClientToDoc(rect);
				pLayer->InactivateAll(this, m_document.m_Datainfo, m_viewMonitor, rect);
			}
		}
	};
	const CPoint local = *(CPoint*)LPARAM;
	CPoint point;
	m_editgeom.UnFocusAll(this);

	for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it!=m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount==0)
			continue;
		DWORD dwPoly = -1;
		DWORD dwBred = -1;
		if(layer->Split(this, m_viewMonitor, local, m_document.m_actionStack, splitted)==true)
		{
			SendMessage(WM_DOCMODIFIED, 0, TRUE);
			if(dwPoly!=-1&&dwBred!=-1)
			{
				for(std::list<CTopology*>::reverse_iterator it = m_document.m_topolist.rbegin(); it!=m_document.m_topolist.rend(); it++)
				{
					CTopology* pTopology = *it;
					DWORD dwOldEdgeId = -1;
					DWORD dwNewEdgeId = -1;
					if(pTopology->PolySplitted(this, m_viewMonitor, layer->m_wId, dwPoly, point, dwBred, dwOldEdgeId, dwNewEdgeId))//to do: open the last action and add the edge changes in
					{
						const CRect inRect = GetVisibleDocRect();
						layer->InactivateAll(this, m_document.m_Datainfo, m_viewMonitor, inRect);
					}
				}
			}
			break;
		}
	}
	return 0L;
}

LONG CGrfView::OnRemovGeomtry(UINT WPARAM, LONG LPARAM)
{
	CGeom* pGeom = (CGeom*)LPARAM;

	for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		const POSITION pos = layer->m_geomlist.Find(pGeom);
		if(pos!=nullptr)
		{
			int index = GetIndexFromOBList(layer->m_geomlist, pGeom);
			std::list<std::pair<CGeom*, unsigned int>> geom;
			geom.push_back(std::pair<CGeom*, unsigned int>(pGeom, index));
			Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pRemove = new Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(this, layer, geom);
			pRemove->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
			pRemove->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
			pRemove->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_AppendRemove_Geoms;
			m_document.m_actionStack.Record(pRemove);

			layer->Invalidate(m_document, pGeom);
			this->SendMessage(WM_PREREMOVEGEOM, (LONG)layer, (UINT)pGeom);
			layer->m_geomlist.RemoveAt(pos);
			return TRUE;
		}
	}

	return FALSE;
}

LONG CGrfView::OnZoomIn(UINT WPARAM, LONG LPARAM)
{
	switch(LPARAM)
	{
		case 0:
			{
				Gdiplus::Rect cliRect = *(Gdiplus::Rect*)WPARAM;
				const CPoint cliPoint(cliRect.X+cliRect.Width/2, cliRect.Y+cliRect.Height/2);
				if(cliRect.Width<5||cliRect.Height<5)
				{
					m_viewMonitor.RatioBy(this, 2, cliPoint);
				}
				else
				{
					CRect client;
					GetClientRect(&client);
					CPoint cliCenter = client.CenterPoint();

					const float xRrato = (float)client.Width()/cliRect.Width;
					const float yRatio = (float)client.Height()/cliRect.Height;
					const float fRatio = min(xRrato, yRatio);

					m_viewMonitor.RatioBy(this, fRatio, cliPoint);
				}
			}
			break;
		case 1:
			{
				CRect docRect = *(CRect*)WPARAM;
				const Gdiplus::Rect cliRect = m_viewMonitor.DocToClient<Gdiplus::Rect>(docRect);
				const CPoint cliPoint = CPoint(cliRect.X+cliRect.Width/2, cliRect.Y+cliRect.Height/2);
				if(cliRect.Width<5||cliRect.Height<5)
				{
					m_viewMonitor.ZoomBy(this, 2, cliPoint);
				}
				else
				{
					CRect client;
					GetClientRect(&client);
					CPoint cliCenter = client.CenterPoint();

					const float xRrato = (float)client.Width()/cliRect.Width;
					const float yRatio = (float)client.Height()/cliRect.Height;
					const float fRatio = min(xRrato, yRatio);

					m_viewMonitor.ZoomBy(this, fRatio, cliPoint);
				}
			}
			break;
	}

	CStatusBarEx* pStatusBar = this->GetStatusBar();
	if(pStatusBar!=nullptr)
	{
		pStatusBar->m_LevelBar.SetLevel(m_viewMonitor.m_levelCurrent);
		pStatusBar->m_ScaleBar.SetScale(m_viewMonitor.m_scaleCurrent);
	}
	CChildFrame* pChildfrm = (CChildFrame*)GetParentFrame();
	pChildfrm->ResetRuler(m_viewMonitor);
	return TRUE;
}

LONG CGrfView::OnZoomOut(UINT WPARAM, LONG LPARAM)
{
	switch(LPARAM)
	{
		case 0:
			{
				Gdiplus::Rect cliRect = *(Gdiplus::Rect*)WPARAM;
				const CPoint cliPoint(cliRect.X+cliRect.Width/2, cliRect.Y+cliRect.Height/2);
				if(cliRect.Width<5||cliRect.Height<5)
				{
					m_viewMonitor.RatioBy(this, 0.5, cliPoint);
				}
				else
				{
					CRect client;
					GetClientRect(&client);
					CPoint cliCenter = client.CenterPoint();

					const float xRrato = (float)client.Width()/cliRect.Width;
					const float yRatio = (float)client.Height()/cliRect.Height;
					const float fRatio = max(xRrato, yRatio);

					m_viewMonitor.RatioBy(this, fRatio, cliPoint);
				}
			}
			break;
		case 1:
			{
				CRect docRect = *(CRect*)WPARAM;
				const Gdiplus::Rect cliRect = m_viewMonitor.DocToClient<Gdiplus::Rect>(docRect);
				const CPoint cliPoint = CPoint(cliRect.X+cliRect.Width/2, cliRect.Y+cliRect.Height/2);
				if(cliRect.Width<5||cliRect.Height<5)
				{
					m_viewMonitor.ZoomBy(this, 0.5, cliPoint);
				}
				else
				{
					CRect client;
					GetClientRect(&client);
					CPoint cliCenter = client.CenterPoint();

					const float xRrato = (float)client.Width()/cliRect.Width;
					const float yRatio = (float)client.Height()/cliRect.Height;
					const float fRatio = max(xRrato, yRatio);

					m_viewMonitor.ZoomBy(this, fRatio, cliPoint);
				}
			}
			break;
	}

	CStatusBarEx* pStatusBar = this->GetStatusBar();
	if(pStatusBar!=nullptr)
	{
		pStatusBar->m_LevelBar.SetLevel(m_viewMonitor.m_levelCurrent);
		pStatusBar->m_ScaleBar.SetScale(m_viewMonitor.m_scaleCurrent);
	}
	CChildFrame* pChildfrm = (CChildFrame*)GetParentFrame();
	pChildfrm->ResetRuler(m_viewMonitor);
	return TRUE;
}

LONG CGrfView::OnFixateView(UINT WPARAM, LONG LPARAM)
{
	const CPoint docPoint = *(CPoint*)WPARAM;
	const CPoint cliPoint = *(CPoint*)LPARAM;
	const CPointF mapPoint = m_document.m_Datainfo.DocToMap(docPoint);
	m_viewMonitor.FixateAt(this, mapPoint, cliPoint);
	CStatusBarEx* pStatusBar = this->GetStatusBar();
	if(pStatusBar!=nullptr)
	{
		pStatusBar->m_LevelBar.SetLevel(m_viewMonitor.m_levelCurrent);
		pStatusBar->m_ScaleBar.SetScale(m_viewMonitor.m_scaleCurrent);
	}

	CChildFrame* pChildfrm = (CChildFrame*)GetParentFrame();
	pChildfrm->ResetRuler(m_viewMonitor);
	return 1L;
}

LONG CGrfView::OnPickActiveGeom(UINT WPARAM, LONG LPARAM)
{
	const CPoint point = *((CPoint*)LPARAM);
	for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it!=m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount==0)
			continue;

		POSITION pos1 = layer->m_geomlist.GetTailPosition();
		while(pos1!=nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetPrev(pos1);
			if(pGeom->m_bActive==false)
				continue;
			const CSize gap = layer->GetDocInflation(m_viewMonitor, pGeom);
			if(pGeom->PickOn(point, gap.cx)==true)
			{
				m_editgeom.m_pLayer = layer;
				return (DWORD)pGeom;
			}
		}
	}

	return (DWORD)nullptr;
}

LONG CGrfView::OnPickAnchor(UINT WPARAM, LONG LPARAM)
{
	const UINT nFlags = WPARAM;
	CPoint& docPoint = *((CPoint*)LPARAM);
	const Gdiplus::Point cliPoint = m_viewMonitor.DocToClient<Gdiplus::Point>(docPoint);
	if((nFlags&MK_SHIFT)==0)
		m_editgeom.UnFocusAll(this);

	CLayer* sellayer = GetActiveLayer();
	for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it!=m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount==0)
			continue;

		if(m_document.m_LayerTreeCtrl.m_bMulti==false&&layer!=sellayer)
			continue;

		CTypedPtrList<CObList, CGeom*>& geomlist = layer->GetGeomList();
		POSITION pos1 = geomlist.GetTailPosition();
		while(pos1!=nullptr)
		{
			CGeom* pGeom = geomlist.GetPrev(pos1);
			if(pGeom->m_bActive==false)
				continue;
			const long nAnchor = pGeom->PickAnchor(m_document.m_Datainfo, m_viewMonitor, cliPoint);
			if(nAnchor!=0)
			{
				if(m_editgeom.m_pGeom!=pGeom)
				{
					m_editgeom.m_pLayer = layer;
					m_editgeom.NewGeom(this, pGeom);
					m_editgeom.FocusAnchor(this, nAnchor, FALSE);
				}
				else
				{
#ifndef _MAC
					if((nFlags&MK_CONTROL)!=0)
#else
					if((nFlags&MK_OPTION)!=0)
#endif
						m_editgeom.FocusAnchor(this, nAnchor, TRUE);
					else
						m_editgeom.FocusAnchor(this, nAnchor, FALSE);
				}

				CStatusBarEx* pStatusBar = this->GetStatusBar();
				if(pStatusBar!=nullptr)
				{
					const CPoint& coor = pGeom->GetAnchor(nAnchor);
					CString text(_T(""));
					text.Format(_T("Idx: %d X: %d Y: %d"), nAnchor, coor.x, coor.y);
					pStatusBar->SetPaneText(indexHANDLEXY, text);
				}
				docPoint = pGeom->GetAnchor(nAnchor);
				return TRUE;
			}
		}
	}

	return FALSE;
}

LONG CGrfView::OnMoveGeomtry(UINT WPARAM, LONG LPARAM)
{
	const CSize Δ = *(CSize*)LPARAM;

	AfxGetApp()->BeginWaitCursor();
	m_document.m_actionStack.Start();

	unsigned long nCount = 0;
	for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount==0)
			continue;

		layer->Move(this, m_viewMonitor, m_document.m_actionStack, Δ);
		nCount += layer->m_nActiveCount;
	}
	m_document.m_actionStack.Stop();
	AfxGetApp()->EndWaitCursor();

	if(nCount>0)
	{
		SendMessage(WM_DOCMODIFIED, 0, TRUE);
		return 1L;
	}
	else
		return 0L;
}

LONG CGrfView::OnSetPrintRect(UINT WPARAM, LONG LPARAM)
{
	m_viewPrinter.m_docPrint = (CRect*)LPARAM;
	return 0L;
}

LONG CGrfView::OnGetPaperSize(UINT WPARAM, LONG LPARAM)
{
	static CSize size = m_document.m_Datainfo.GetDocCanvas();
	return (LONG)(CSize*)&size;
}

LONG CGrfView::OnGetActiveRect(UINT WPARAM, LONG LPARAM)
{
	static CRect rect;
	rect.SetRectEmpty();

	for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		rect = UnionRect(rect, layer->GetActiveRect());
	}

	return (LONG)(&rect);
}

LONG CGrfView::OnGetActiveArea(UINT WPARAM, LONG LPARAM)
{
	static Gdiplus::Rect rect;
	rect.X = rect.Y = rect.Width = rect.Height = 0;

	for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		rect = UnionRect(rect, layer->GetActiveArea(m_viewMonitor));
	}

	return (LONG)(&rect);
}

LONG CGrfView::OnCaptureImage(UINT WPARAM, LONG LPARAM)
{
	const Gdiplus::Rect clipBox = *(Gdiplus::Rect*)WPARAM;
	if(clipBox.IsEmptyArea()==TRUE)
		return 0;
	const CClientDC dc(this);
	m_viewMonitor.m_sizeDPI = CSize(dc.GetDeviceCaps(LOGPIXELSX), dc.GetDeviceCaps(LOGPIXELSY));
	m_viewMonitor.m_sizeDPI = CSize(72, 72);
	Gdiplus::Bitmap* bitmap = ::new Gdiplus::Bitmap(clipBox.Width, clipBox.Height, PixelFormat32bppARGB);
	if(bitmap!=nullptr)
	{
		Gdiplus::Graphics g(bitmap);
		g.SetPageUnit(Gdiplus::Unit::UnitPixel);
		g.Clear(Gdiplus::Color(254, 254, 254));
		g.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
		g.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQualityBicubic);
		g.SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHighQuality);
		g.SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintAntiAlias);
		g.TranslateTransform(-clipBox.GetLeft(), -clipBox.GetTop());

		CRect inRect = CRect(-m_viewMonitor.m_ptViewPos, m_viewMonitor.m_sizeView);
		inRect.IntersectRect(inRect, CRect(clipBox.GetLeft(), clipBox.GetTop(), clipBox.GetRight(), clipBox.GetBottom()));
		inRect = m_viewMonitor.ClientToDoc(inRect);
		m_document.Capture(this, m_viewMonitor, g, inRect);
		return (LONG)bitmap;
	}
	else
		return (LONG)nullptr;
}

LONG CGrfView::OnActiveGeom(UINT WPARAM, LONG LPARAM)
{
	const DWORD dwGeom = LPARAM;

	for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		POSITION pos2 = layer->m_geomlist.GetTailPosition();
		while(pos2!=nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetPrev(pos2);
			if(pGeom->m_geoId==dwGeom)
			{
				if(pGeom->m_bActive==false)
				{
					CClientDC dc(this);
					pGeom->m_bActive = true;
					pGeom->DrawAnchors(&dc, m_viewMonitor);
					layer->m_nActiveCount++;
				}

				return 0;
			}
		}
	}

	return 0;
}

LONG CGrfView::OnGetGeomInflate(UINT WPARAM, LONG LPARAM)
{
	CGeom* geom = (CGeom*)LPARAM;

	for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		const POSITION pos = layer->m_geomlist.Find(geom);
		if(pos!=nullptr)
		{
			return layer->GetVewInflation(m_viewMonitor, geom).Width;
		}
	}

	return 0;
}

void CGrfView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);

	if(bActivate==TRUE&&pActivateView==this)
	{
		CStatusBarEx* pStatusBar = this->GetStatusBar();
		if(pStatusBar!=nullptr)
		{
			pStatusBar->ViewChanged(m_document.m_Datainfo);
			pStatusBar->m_LevelBar.SetLevel(m_viewMonitor.m_levelCurrent);
			pStatusBar->m_ScaleBar.SetScale(m_viewMonitor.m_scaleCurrent);
			pStatusBar->m_LevelBar.SetRange(m_viewMonitor.m_levelMinimum, m_viewMonitor.m_levelMaximum);
			pStatusBar->m_ScaleBar.SetRange(m_document.m_Datainfo.m_scaleMinimum, m_document.m_Datainfo.m_scaleMaximum);
		}

		CTopoPane* pTopoPane = (CTopoPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'T', 0);
		if(pTopoPane!=nullptr)
		{
			pTopoPane->SetCtrl(m_document.m_pTopoCtrl);
		}

		CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
		if(pPropertyPane!=nullptr)
		{
			if(m_document.m_topolist.size()>0)
			{
				pPropertyPane->ValidateTab('E');
				pPropertyPane->ValidateTab('N');
			}
			else
			{
				pPropertyPane->InvalidateTab('E');
				pPropertyPane->InvalidateTab('N');
			}
			const CStringA strClassName = m_document.GetRuntimeClass()->m_lpszClassName;
			if(strClassName=="CGrfDoc")
			{
				pPropertyPane->InvalidateTab('R');
				pPropertyPane->InvalidateTab('A');
			}
			else if(strClassName=="CGidDoc")
			{
				pPropertyPane->InvalidateTab('R');
			}
			else if(strClassName=="CGeoDoc")
			{
				pPropertyPane->InvalidateTab('A');
			}
		}
		CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'O', 0);
		if(pTablePane!=nullptr)
		{
			const CStringA strClassName = m_document.GetRuntimeClass()->m_lpszClassName;
			if(strClassName=="CGrfDoc")
			{
				if(pTablePane->m_pPOUTDBGridCtrl!=nullptr)
				{
					pTablePane->m_pPOUTDBGridCtrl->SetDatabase(nullptr);
				}
				if(pTablePane->m_pATTTDBGridCtrl!=nullptr)
				{
					pTablePane->m_pATTTDBGridCtrl->SetDatabase(nullptr);
				}
				if(pTablePane->m_pPOUDBGridCtrl!=nullptr)
				{
					pTablePane->m_pPOUDBGridCtrl->Clear();
				}
				if(pTablePane->m_pATTDBGridCtrl!=nullptr)
				{
					pTablePane->m_pATTDBGridCtrl->Clear();
				}
			}
		}
		CLayerPane* pLayerPane = (CLayerPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'L', 0);
		if(pLayerPane!=nullptr)
		{
			pLayerPane->SetCtrl(&m_document.m_LayerTreeCtrl);
		}

		CTool::SetEditGeom(&m_editgeom);

		if(m_viewMonitor.m_pBackground!=nullptr)
		{
			m_viewMonitor.m_pBackground->SetWindowPos(AfxGetMainWnd()->GetSafeHwnd(), 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
		}

		CMainFrame* pAppFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		if(pAppFrame!=nullptr)
		{
			pAppFrame->m_LayerPane.EnableBackgroundBar(FALSE);
			pAppFrame->m_wndDrawTool.SwitchSource(&m_document.m_pTool);
		}
	}
	else if(pDeactiveView==this)//when you close one view this will be fired after that view is closed
	{
	}
}

LONG CGrfView::OnIsTopMost(UINT WPARAM, LONG LPARAM)
{
	CLayer* pLayer = (CLayer*)WPARAM;
	if(pLayer==nullptr)
		return FALSE;

	CGeom* pGeom = (CGeom*)LPARAM;
	if(pGeom==nullptr)
		return FALSE;
	const CRect docRect1 = pLayer->GetGeomDocBoundary(m_viewMonitor, pGeom);
	for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it!=m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		POSITION pos2 = layer->m_geomlist.GetTailPosition();
		while(pos2!=nullptr)
		{
			CGeom* geom = layer->m_geomlist.GetPrev(pos2);
			if(geom==pGeom)
				return TRUE;

			CRect docRect2 = layer->GetGeomDocBoundary(m_viewMonitor, geom);
			if(Util::Rect::Intersect(docRect2, docRect1)==true)
				return FALSE;
		}
		if(layer==pLayer)
			return TRUE;
	}

	return TRUE;
}

LONG CGrfView::OnPreRemoveGeom(UINT WPARAM, LONG LPARAM)
{
	CLayer* pLayer = (CLayer*)WPARAM;
	if(pLayer==nullptr)
		return 0;
	CGeom* pGeom = (CGeom*)LPARAM;
	if(pGeom==nullptr)
		return 0;

	if(m_editgeom.m_pGeom==pGeom)
	{
		CClientDC dc(this);
		m_editgeom.SwitchAnchorsNormal(&dc);

		m_editgeom.m_Anchors.clear();
		m_editgeom.m_pGeom = nullptr;
	}
	const POSITION pos = m_hlGeomList.Find(pGeom);
	if(pos!=nullptr)
		m_hlGeomList.RemoveAt(pos);

	//	m_document.PreRemoveGeom(pGeom);
	return 1;
}
LONG CGrfView::OnToRemoveEdge(UINT WPARAM, LONG LPARAM)
{
	CGeom* pGeom = (CGeom*)LPARAM;
	if(pGeom==nullptr)
		return 0;

	m_document.PreRemoveGeom(pGeom);
	return 1;
}
LONG CGrfView::OnPreDeleteGeom(UINT WPARAM, LONG LPARAM)
{
	CGeom* pGeom = (CGeom*)LPARAM;
	if(pGeom==nullptr)
		return 0;

	if(m_editgeom.m_pGeom==pGeom)
	{
		CClientDC dc(this);
		m_editgeom.SwitchAnchorsNormal(&dc);

		m_editgeom.m_Anchors.clear();
		m_editgeom.m_pGeom = nullptr;
	}
	const POSITION pos = m_hlGeomList.Find(pGeom);
	if(pos!=nullptr)
		m_hlGeomList.RemoveAt(pos);

	m_document.FreeGeomId(pGeom);
	return 1;
}

LONG CGrfView::OnPreRemoveLayer(UINT WPARAM, LONG LPARAM)
{
	CLayer* layer = (CLayer*)WPARAM;
	if(layer==nullptr)
		return 0L;

	POSITION pos = layer->m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* geom = layer->m_geomlist.GetNext(pos);
		OnPreRemoveGeom((LONG)layer, (UINT)geom);
	}

	return 1L;
}

LONG CGrfView::OnPreDeleteLayer(UINT WPARAM, LONG LPARAM)
{
	CLayer* layer = (CLayer*)WPARAM;
	if(layer==nullptr)
		return 0L;

	POSITION pos = layer->m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* geom = layer->m_geomlist.GetNext(pos);
		OnPreDeleteGeom((LONG)layer, (UINT)geom);
	}

	layer->Relieve();

	return 1L;
}

LONG CGrfView::OnDocModified(UINT WPARAM, LONG LPARAM)
{
	const BOOL bModified = (BOOL)LPARAM;

	m_document.SetModifiedFlag(bModified);
	return 0L;
}

BOOL CGrfView::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo==nullptr)
	{
		if(nCode==CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CView::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
			}
			ASSERT(!pCmdUI->m_bContinueRouting); // idle - not set

			switch(nId)
			{
				case ID_VIEW_GRID:
					pCmdUI->SetCheck(m_viewMonitor.m_bShowGrid);
					break;
				case ID_SELECTALL_BYNAME:
				case ID_SELECTALL_BYTYPE:
				case ID_SELECTALL_ALL:
					pCmdUI->Enable(TRUE);
					return TRUE;
					break;
				case ID_SELECTALL_IDENTICAL:
					{
						int totalSelected = 0;
						for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							totalSelected += layer->m_nActiveCount;
						}
						pCmdUI->Enable(totalSelected==1);
					}
					return TRUE;
					break;
				case ID_GEOM_SETSPOT:
					{
						for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount==0)
								continue;

							POSITION pos = layer->m_geomlist.GetHeadPosition();
							while(pos!=nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetNext(pos);
								if(pGeom->m_bActive==false)
									continue;

								if(pGeom->IsKindOf(RUNTIME_CLASS(CMark))==false)
									continue;

								pCmdUI->Enable(TRUE);
								return TRUE;
							}
						}
					}
					pCmdUI->Enable(FALSE);
					return TRUE;
					break;
				case ID_GEOM_SETTYPE:
					{
						for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount==0)
								continue;

							POSITION pos = layer->m_geomlist.GetHeadPosition();
							while(pos!=nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetNext(pos);
								if(pGeom->m_bActive==false)
									continue;

								if(pGeom->IsKindOf(RUNTIME_CLASS(CText))==true)
								{
									pCmdUI->Enable(TRUE);
									return TRUE;
								}

								if(pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly))==true)
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
				case ID_GEOM_SETSTYLE:
					{
						for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount==0)
								continue;

							POSITION pos = layer->m_geomlist.GetHeadPosition();
							while(pos!=nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetNext(pos);
								if(pGeom->m_bActive==false)
									continue;

								if(pGeom->IsKindOf(RUNTIME_CLASS(CText))==TRUE)
									continue;
								if(pGeom->IsKindOf(RUNTIME_CLASS(CMark))==TRUE)
									continue;
								if(pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly))==TRUE)
									continue;

								pCmdUI->Enable(TRUE);
								return TRUE;
							}
						}
					}

					pCmdUI->Enable(FALSE);
					return TRUE;
					break;
				case ID_GEOM_SETHINT:
					{
						for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount==0)
								continue;

							POSITION pos = layer->m_geomlist.GetHeadPosition();
							while(pos!=nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetNext(pos);
								if(pGeom->m_bActive==false)
									continue;

								if(pGeom->IsKindOf(RUNTIME_CLASS(CText))==TRUE)
									continue;
								if(pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly))==TRUE)
									continue;

								pCmdUI->Enable(TRUE);
								return TRUE;
							}
						}
					}

					pCmdUI->Enable(FALSE);
					return TRUE;
					break;
				case ID_SELECTALL_DISCARD:
				case ID_SELECTALL_INVERSE:
				case ID_GEOM_PROPERTY:
					{
						for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount==0)
								continue;

							POSITION pos = layer->m_geomlist.GetHeadPosition();
							while(pos!=nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetNext(pos);
								if(pGeom->m_bActive==false)
									continue;

								pCmdUI->Enable(TRUE);
								return TRUE;
							}
						}
					}
					pCmdUI->Enable(FALSE);
					return TRUE;
					break;
				case ID_GEOM_JOIN:
					{
						for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount==0)
								continue;

							POSITION pos = layer->m_geomlist.GetHeadPosition();
							while(pos!=nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetNext(pos);
								if(pGeom->m_bActive==false)
									continue;
								if(pGeom->m_bLocked==TRUE)
									continue;
								if(pGeom->m_bClosed==TRUE)
									continue;
								if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly))==false)
									continue;

								pCmdUI->Enable(TRUE);
								return TRUE;
							}
						}
						pCmdUI->Enable(FALSE);
					}
					return TRUE;
					break;
				case ID_GEOM_REGRESS:
					{
						for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
						{
							CLayer* layer = *it;
							if(layer->m_nActiveCount==0)
								continue;

							POSITION pos = layer->m_geomlist.GetHeadPosition();
							while(pos!=nullptr)
							{
								CGeom* pGeom = layer->m_geomlist.GetNext(pos);
								if(pGeom->m_bActive==false)
									continue;
								if(pGeom->m_bLocked==TRUE)
									continue;

								if(pGeom->IsKindOf(RUNTIME_CLASS(CMark))&&((CMark*)pGeom)->m_pSpot!=nullptr)
								{
									pCmdUI->Enable(TRUE);
									return TRUE;
								}
								if(pGeom->IsKindOf(RUNTIME_CLASS(CText))&&((CText*)pGeom)->m_pType!=nullptr)
								{
									pCmdUI->Enable(TRUE);
									return TRUE;
								}

								if(pGeom->m_pLine!=nullptr||pGeom->m_pFill!=nullptr)
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
				case ID_VIEW_LABELOID:
					pCmdUI->SetCheck(m_document.preference.ShowLabeloid);
					return TRUE;
					break;
				case ID_VIEW_GEOGROID:
					pCmdUI->SetCheck(m_document.preference.ShowGeogroid);
					return TRUE;
					break;
				case ID_VIEW_CREATEDTAGS:
					pCmdUI->SetCheck(m_document.preference.ShowCreatedTags);
					return TRUE;
					break;
				case ID_VIEW_DYNAMICTAGS:
					pCmdUI->SetCheck(m_document.preference.ShowDynamicTags);
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

	return CView::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}

void CGrfView::OnSelectAllDiscard()
{
	InactivateAll();

	m_document.m_LayerTreeCtrl.SelectFixed();
}

void CGrfView::OnGeomSetStyle()
{
	unsigned int target1[5] = {0};
	unsigned int target2[5] = {0};
	CLine* pSample1 = nullptr;
	CFillCompact* pSample2 = nullptr;
	std::list<CGeom*> geoms;
	boost::uuids::detail::sha1 sha1;
	for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount==0)
			continue;

		POSITION pos = layer->m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetNext(pos);
			if(pGeom->m_bActive==false)
				continue;
			if(pGeom->IsKindOf(RUNTIME_CLASS(CPath))==false)
				continue;
			if(pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly))==true)
				continue;

			unsigned int hash1[5] = {0};
			unsigned int hash2[5] = {0};
			if(pGeom->m_pLine!=nullptr)
			{
				pGeom->m_pLine->Sha1(hash1);
			}
			if(pGeom->m_pFill!=nullptr)
			{
				pGeom->m_pFill->Sha1(hash2);
			}
			if(geoms.size()==0)
			{
				memcpy(target1, hash1, sizeof(unsigned int)*5);
				memcpy(target2, hash2, sizeof(unsigned int)*5);
				pSample1 = pGeom->m_pLine;
				pSample2 = pGeom->m_pFill;
			}
			else
			{
				if(arr::equal(hash1, target1)==false)
				{
					pSample1 = nullptr;
				}
				if(arr::equal(hash2, target2)==false)
				{
					pSample2 = nullptr;
				}
			}
			geoms.push_back(pGeom);
		}
	}

	if(geoms.size()>0)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Style.dll"));
		AfxSetResourceHandle(hInst);

		CStyleDlg dlg(this, false, m_document.m_layertree.m_library, pSample1, pSample2);
		if(dlg.DoModal()==IDOK)
		{
			for(auto it = geoms.begin(); it!=geoms.end(); it++)
			{
				CGeom* pGeom = (*it);
				pGeom->Invalidate(this, m_viewMonitor, 3);

				if(pGeom->m_pLine!=nullptr)
				{
					delete pGeom->m_pLine;
					pGeom->m_pLine = nullptr;
				}
				if(pGeom->m_pFill!=nullptr)
				{
					delete pGeom->m_pFill;
					pGeom->m_pFill = nullptr;
				}

				if(dlg.d_pLine!=nullptr)
				{
					pGeom->m_pLine = dlg.d_pLine->Clone();
				}
				if(dlg.d_pFill!=nullptr)
				{
					pGeom->m_pFill = (CFillCompact*)(dlg.d_pFill->Clone());
				}
				pGeom->m_bModified = true;
				pGeom->Invalidate(this, m_viewMonitor, 3);
			}
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);

		m_document.SetModifiedFlag(TRUE);
	}
}

void CGrfView::OnGeomSetSpot()
{
	unsigned int target[5] = {0};
	CSpot* pSample = nullptr;
	std::list<std::pair<CMark*, CLayer*>> marks;
	boost::uuids::detail::sha1 sha1;
	for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount==0)
			continue;

		POSITION pos = layer->m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetNext(pos);
			if(pGeom->m_bActive==false)
				continue;
			if(pGeom->IsKindOf(RUNTIME_CLASS(CMark))==false)
				continue;

			CMark* pMark = (CMark*)pGeom;
			unsigned int hash[5] = {0};
			if(pMark->m_pSpot!=nullptr)
			{
				pMark->m_pSpot->Sha1(hash);
			}
			if(marks.size()==0)
			{
				memcpy(target, hash, sizeof(unsigned int)*5);
				pSample = pMark->m_pSpot!=nullptr ? pMark->m_pSpot : layer->m_pSpot;
			}
			else if(arr::equal(hash, target)==false)
			{
				pSample = nullptr;
			}
			marks.push_back(std::make_pair(pMark, layer));
		}
	}
	if(marks.size()>0)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Style.dll"));
		AfxSetResourceHandle(hInst);

		CSpotCtrl dlg(nullptr, true, m_document.m_layertree.m_library, pSample);
		if(dlg.DoModal()==IDOK)
		{
			for(auto it = marks.begin(); it!=marks.end(); it++)
			{
				CMark* pMark = (*it).first;
				CLayer* pLayer = (*it).second;
				pMark->Invalidate(this, m_viewMonitor, 3);
				if(pMark->m_pSpot!=nullptr)
				{
					pMark->m_pSpot->Decrease(m_document.m_layertree.m_library);
					delete pMark->m_pSpot;
					pMark->m_pSpot = nullptr;
				}

				if(dlg.d_pSpot!=nullptr)
				{
					pMark->m_pSpot = dlg.d_pSpot->Clone();
					pMark->CalCorner(pMark->m_pSpot, m_viewMonitor.m_datainfo, m_document.m_layertree.m_library);
				}
				else
				{
					pMark->CalCorner(pLayer->m_pSpot, m_viewMonitor.m_datainfo, m_document.m_layertree.m_library);
				}
				pMark->Invalidate(this, m_viewMonitor, 3);
			}
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
}

void CGrfView::OnGeomSetType()
{
	unsigned int target[5] = {0};
	CType* pSample = nullptr;
	std::list<CGeom*> texts;
	boost::uuids::detail::sha1 sha1;
	for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount==0)
			continue;

		POSITION pos = layer->m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetNext(pos);
			if(pGeom->m_bActive==false)
				continue;
			if(pGeom->IsKindOf(RUNTIME_CLASS(CText))==false&&pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly))==false)
				continue;

			unsigned int hash[5] = {0};
			if(pGeom->m_pType!=nullptr)
			{
				pGeom->m_pType->Sha1(hash);
			}
			if(texts.size()==0)
			{
				memcpy(target, hash, sizeof(unsigned int)*5);
				pSample = pGeom->m_pType!=nullptr ? pGeom->m_pType : layer->m_pType;
			}
			else if(arr::equal(hash, target)==false)
			{
				pSample = nullptr;
			}
			texts.push_back(pGeom);
		}
	}
	if(texts.size()>0)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Style.dll"));
		AfxSetResourceHandle(hInst);

		CType* pNew = pSample==nullptr ? new CType() : pSample->Clone();
		CTypeDlg dlg(nullptr, *pNew);
		if(dlg.DoModal()==IDOK)
		{
			for(auto it = texts.begin(); it!=texts.end(); it++)
			{
				CGeom* pGeom = (*it);
				pGeom->Invalidate(this, m_viewMonitor, 3);

				if(pGeom->m_pType!=nullptr)
				{
					delete pGeom->m_pType;
					pGeom->m_pType = nullptr;
				}
				pGeom->m_pType = pNew->Clone();
				if(pGeom->IsKindOf(RUNTIME_CLASS(CText)))
					((CText*)pGeom)->CalCorner(nullptr, m_viewMonitor.m_datainfo.m_zoomPointToDoc, 1.f);
				else if(pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly)))
				{

				}

				pGeom->Invalidate(this, m_viewMonitor, 3);
			}
		}
		delete pNew;

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
}

void CGrfView::OnGeomSetHint()
{
	unsigned int target[5] = {0};
	CHint* pSample = nullptr;
	std::list<CGeom*> geoms;
	boost::uuids::detail::sha1 sha1;
	for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount==0)
			continue;

		POSITION pos = layer->m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetNext(pos);
			if(pGeom->m_bActive==false)
				continue;
			if(pGeom->IsKindOf(RUNTIME_CLASS(CText))==true)
				continue;
			else if(pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly))==true)
				continue;

			unsigned int hash[5] = {0};
			if(pGeom->m_pHint!=nullptr)
			{
				pGeom->m_pHint->Sha1(hash);
			}
			if(geoms.size()==0)
			{
				memcpy(target, hash, sizeof(unsigned int)*5);
				pSample = pGeom->m_pHint!=nullptr ? pGeom->m_pHint : layer->m_pHint;
			}
			else if(arr::equal(hash, target)==false)
			{
				pSample = nullptr;
			}
			geoms.push_back(pGeom);
		}
	}
	if(geoms.size()>0)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Style.dll"));
		AfxSetResourceHandle(hInst);

		CHint* pNew = pSample==nullptr ? new CHint() : pSample->Clone();
		CHintDlg dlg(nullptr, *pNew);
		if(dlg.DoModal()==IDOK)
		{
			for(auto it = geoms.begin(); it!=geoms.end(); it++)
			{
				CGeom* pGeom = (*it);
				pGeom->Invalidate(this, m_viewMonitor, 3);

				if(pGeom->m_pHint!=nullptr)
				{
					delete pGeom->m_pHint;
					pGeom->m_pHint = nullptr;
				}
				pGeom->m_pHint = pNew->Clone();

				pGeom->Invalidate(this, m_viewMonitor, 3);
			}
		}
		delete pNew;

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
}

void CGrfView::OnGeomRegress()
{
	for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount==0)
			continue;

		layer->Regress(this, m_viewMonitor);
	}
}

void CGrfView::OnMagnifyFitActive()
{
	const Gdiplus::Rect cliRect = *(Gdiplus::Rect*)(SendMessage(WM_GETACTIVEAREA, 0, 0));
	const CPoint cliPoint = CPoint(cliRect.X+cliRect.Width/2, cliRect.Y+cliRect.Height/2);
	const CPoint docPoint = m_viewMonitor.ClientToDoc(cliPoint, true);

	CRect client;
	this->GetClientRect(&client);
	const float xZoom = (float)client.Width()/cliRect.Width;
	const float yZoom = (float)client.Height()/cliRect.Height;
	const float fZoom = min(xZoom, yZoom);
	m_viewMonitor.ZoomBy(this, fZoom, cliPoint);
	const Gdiplus::Point cliFrom = m_viewMonitor.DocToClient<Gdiplus::Point>(docPoint);
	const CPoint cliTo = client.CenterPoint();
	m_viewMonitor.MoveBy(this, cliFrom.X-cliTo.x, cliFrom.Y-cliTo.y);

	CStatusBarEx* pStatusBar = this->GetStatusBar();
	if(pStatusBar!=nullptr)
	{
		pStatusBar->m_LevelBar.SetLevel(m_viewMonitor.m_levelCurrent);
		pStatusBar->m_ScaleBar.SetScale(m_viewMonitor.m_scaleCurrent);
	}
	CChildFrame* pChildfrm = (CChildFrame*)GetParentFrame();
	pChildfrm->ResetRuler(m_viewMonitor);
}

void CGrfView::OnMagnifyPaper1to1()
{
	CRect client;
	this->GetClientRect(&client);
	const CPoint cliPoint = client.CenterPoint();
	const CPointF mapPoint = m_viewMonitor.ClientToDataMap(cliPoint);
	m_viewMonitor.ScaleTo(this, m_viewMonitor.m_datainfo.m_scaleSource, mapPoint);

	CStatusBarEx* pStatusBar = this->GetStatusBar();
	if(pStatusBar!=nullptr)
	{
		pStatusBar->m_LevelBar.SetLevel(m_viewMonitor.m_levelCurrent);
		pStatusBar->m_ScaleBar.SetScale(m_viewMonitor.m_scaleCurrent);
	}
	CChildFrame* pChildfrm = (CChildFrame*)GetParentFrame();
	pChildfrm->ResetRuler(m_viewMonitor);
}

void CGrfView::OnMagnifyScreen1to1()
{
	const CClientDC dc(this);

	CRect client;
	this->GetClientRect(&client);
	const CPoint cliPoint = client.CenterPoint();
	const CPointF mapPoint = m_viewMonitor.ClientToDataMap(cliPoint);
	m_viewMonitor.ScaleTo(this, m_viewMonitor.m_datainfo.m_scaleSource, mapPoint);

	CStatusBarEx* pStatusBar = this->GetStatusBar();
	if(pStatusBar!=nullptr)
	{
		pStatusBar->m_LevelBar.SetLevel(m_viewMonitor.m_levelCurrent);
		pStatusBar->m_ScaleBar.SetScale(m_viewMonitor.m_scaleCurrent);
	}
	CChildFrame* pChildfrm = (CChildFrame*)GetParentFrame();
	pChildfrm->ResetRuler(m_viewMonitor);
}

LONG CGrfView::OnApplyGeomID(UINT WPARAM, LONG LPARAM)
{
	return m_document.ApplyGeomId();
}

void CGrfView::OnGeomProperty()
{
	for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it!=m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_nActiveCount==0)
			continue;

		layer->GeomAttribute(this, m_viewMonitor);
	}
}

void CGrfView::OnEditInsertOle()
{
	COleInsertDialog dlg;
	if(dlg.DoModal()!=IDOK)
	{
		return;
	}

	AfxGetApp()->BeginWaitCursor();

	// First create the C++ object
	CRect rect = m_viewMonitor.ClientToDoc(CRect(100, 100, 100, 100));
	rect.NormalizeRect();

	COleObj* pGeom = new COleObj(rect);
	ASSERT_VALID(pGeom);
	COleItem* pItem = new COleItem(&m_document, pGeom);
	ASSERT_VALID(pItem);
	pGeom->m_pOleItem = pItem;

	try
	{
		if(!dlg.CreateItem(pGeom->m_pOleItem))
			AfxThrowMemoryException();

		// try to get initial presentation data
		pItem->UpdateLink();
		pItem->UpdateExtent(this);

		// if insert new object -- initially show the object
		if(dlg.GetSelectionType()==COleInsertDialog::createNewItem)
			pItem->DoVerb(OLEIVERB_SHOW, this);
	}
	catch(CException* ex)
	{
		// clean up item
		pItem->Delete();
		pGeom->m_pOleItem = nullptr;
		delete pGeom;
		pGeom = nullptr;
		//			OutputDebugString(ex->m_strError);
		AfxMessageBox(IDP_FAILED_TO_CREATE);
		ex->Delete();
	}

	if(pGeom!=nullptr)
	{
		SendMessage(WM_NEWGEOMDREW, (UINT)pGeom, 0);
	}
	AfxGetApp()->EndWaitCursor();
}

LONG CGrfView::OnKeyQuery(UINT WPARAM, LONG LPARAM)
{
	CString strKey = *(CString*)WPARAM;
	CListCtrl* listCtrl = (CListCtrl*)LPARAM;
	if(listCtrl==nullptr||strKey.IsEmpty())
		return 0;

	strKey = strKey.Mid(14);
	strKey.MakeUpper();

	bool idQyery = false;
	DWORD dwId;
	if(strKey.GetLength()>4&&strKey.Left(3)==_T("ID="))
	{
		idQyery = true;
		strKey = strKey.Mid(3);
		dwId = _tcstoul(strKey, nullptr, 10);
	}
	const CString strMap = m_document.m_Datainfo.m_strName;
	CString strFile = m_document.GetPathName();

	long count = 0;
	for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it!=m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bKeyQuery==false)
			continue;;

		POSITION pos = layer->m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetNext(pos);
			if(idQyery==true)
			{
				if(pGeom->m_geoId==dwId)
				{
					CString string = pGeom->m_strName;
					string.Replace(_T("\r\n"), _T(" "));
					//	string.Replace(_T("\r\n"),nullptr);
					//	string.Replace(_T(" "),nullptr);
					string.Replace(_T("¡¡"), nullptr);

					PLACE* place = new PLACE;
					const long nItem = listCtrl->GetItemCount();
					const long index = listCtrl->InsertItem(nItem, nullptr);
					listCtrl->SetItemData(index, (DWORD)place);

					listCtrl->SetItemText(nItem, 0, string);
					listCtrl->SetItemText(nItem, 1, strMap);
					listCtrl->SetItemText(nItem, 2, layer->m_strName);

					place->wMap = 0;
					place->wLayer = layer->m_wId;
					place->dwGeom = pGeom->m_geoId;

					count++;
				}
			}
			else if(pGeom->m_strName.IsEmpty()==FALSE)
			{
				if(pGeom->Query(strKey)==true)
				{
					CString string = pGeom->m_strName;
					string.Replace(_T("\r\n"), _T(" "));
					//	string.Replace(_T("\r\n"),nullptr);
					//	string.Replace(_T(" "),nullptr);
					//	string.Replace(_T("¡¡"),nullptr);

					PLACE* place = new PLACE;
					const long nItem = listCtrl->GetItemCount();
					const long index = listCtrl->InsertItem(nItem, nullptr);
					listCtrl->SetItemData(index, (DWORD)place);

					listCtrl->SetItemText(nItem, 0, string);
					listCtrl->SetItemText(nItem, 1, strMap);
					listCtrl->SetItemText(nItem, 2, layer->m_strName);

					place->wMap = 0;
					place->wLayer = layer->m_wId;
					place->dwGeom = pGeom->m_geoId;

					count++;
				}
			}
		}
	}

	return count;
}

void CGrfView::OnSelectAllInverse()
{
	CRect inRect = GetVisibleDocRect();

	AfxGetApp()->BeginWaitCursor();
	if(m_document.m_LayerTreeCtrl.m_bMulti==false)
	{
		CLayer* layer = GetActiveLayer();
		if(layer!=nullptr)
		{
			layer->ActivateInverse(this, m_document.m_Datainfo, m_viewMonitor, inRect);
		}
	}
	else
	{
		for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			layer->ActivateInverse(this, m_document.m_Datainfo, m_viewMonitor, inRect);
		}
	}
	AfxGetApp()->EndWaitCursor();
}

void CGrfView::OnSelectAllByName()
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Geometry.dll"));
	AfxSetResourceHandle(hInst);

	CTextEditDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		CString strKey = dlg.m_String;

		CRect inRect = GetVisibleDocRect();

		AfxGetApp()->BeginWaitCursor();
		if(m_document.m_LayerTreeCtrl.m_bMulti==false)
		{
			CLayer* layer = GetActiveLayer();
			if(layer!=nullptr&&layer->m_bVisible)
			{
				layer->ActivateAll(this, m_document.m_Datainfo, m_viewMonitor, inRect, strKey);
			}
		}
		else
		{
			for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
			{
				CLayer* layer = *it;
				if(layer->m_bVisible==false)
					continue;

				layer->ActivateAll(this, m_document.m_Datainfo, m_viewMonitor, inRect, strKey);
			}

			m_document.m_LayerTreeCtrl.UpdateSelect();
		}
		AfxGetApp()->EndWaitCursor();
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CGrfView::OnSelectAllSameStyle()
{
	CRect inRect = GetVisibleDocRect();

	CLayer* pActiveLayer;
	CGeom* pActiveGeom = m_document.GetTheActivatedGeom(pActiveLayer);
	if(pActiveGeom==nullptr||pActiveLayer==nullptr)
		return;

	CLine* pLine = pActiveLayer->GetLine();
	CFill* pFill = pActiveLayer->GetFill();
	CSpot* pSpot = pActiveLayer->GetSpot();
	CType* pType = pActiveLayer->GetType();
	boost::uuids::detail::sha1 sha1;
	pActiveGeom->Sha1(sha1, pLine, pFill, pSpot, pType);
	unsigned int target[5] = {0};
	sha1.get_digest(target);

	AfxGetApp()->BeginWaitCursor();
	if(m_document.m_LayerTreeCtrl.m_bMulti==false)
	{
		CLayer* layer = GetActiveLayer();
		if(layer!=nullptr&&layer->m_bVisible)
		{
			layer->ActivateSameStyle(this, m_document.m_Datainfo, m_viewMonitor, inRect, target);
		}
	}
	else
	{
		for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			if(layer->m_bVisible==false)
				continue;

			layer->ActivateSameStyle(this, m_document.m_Datainfo, m_viewMonitor, inRect, target);
		}
		m_document.m_LayerTreeCtrl.UpdateSelect();
	}

	AfxGetApp()->EndWaitCursor();
}
void CGrfView::OnSelectAllIdentical()
{
	CRect inRect = GetVisibleDocRect();

	CLayer* pActiveLayer;
	CGeom* pActiveGeom = m_document.GetTheActivatedGeom(pActiveLayer);
	if(pActiveGeom==nullptr||pActiveLayer==nullptr)
		return;

	CLine* pLine = pActiveLayer->GetLine();
	CFill* pFill = pActiveLayer->GetFill();
	CSpot* pSpot = pActiveLayer->GetSpot();
	CType* pType = pActiveLayer->GetType();
	boost::uuids::detail::sha1 sha11;
	pActiveGeom->Sha1(sha11, pLine, pFill, pSpot, pType);
	unsigned int target1[5] = {0};
	sha11.get_digest(target1);

	CPoint origin = pActiveGeom->GetOrigin();
	boost::uuids::detail::sha1 sha12;
	pActiveGeom->Sha1(sha12, CSize(origin.x, origin.y));
	unsigned int target2[5] = {0};
	sha12.get_digest(target2);

	AfxGetApp()->BeginWaitCursor();
	if(m_document.m_LayerTreeCtrl.m_bMulti==false)
	{
		CLayer* layer = GetActiveLayer();
		if(layer!=nullptr&&layer->m_bVisible)
		{
			layer->ActivateIdentical(this, m_document.m_Datainfo, m_viewMonitor, inRect, target1, target2);
		}
	}
	else
	{
		for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			if(layer->m_bVisible==false)
				continue;

			layer->ActivateIdentical(this, m_document.m_Datainfo, m_viewMonitor, inRect, target1, target2);
		}
		m_document.m_LayerTreeCtrl.UpdateSelect();
	}

	AfxGetApp()->EndWaitCursor();
}
LONG CGrfView::OnFlashGeom(UINT WPARAM, LONG LPARAM)
{
	CGrfView::UnhighligtAll();

	CGeom* pFlash = nullptr;
	if(WPARAM==1)
	{
		pFlash = m_document.m_layertree.GetGeomById(LPARAM);
	}
	else if(WPARAM==3&&LPARAM!=0)
	{
		const PLACE* place = (PLACE*)LPARAM;
		pFlash = m_document.m_layertree.GetGeomById(place->wLayer, place->dwGeom);
	}

	if(pFlash!=nullptr)
	{
		const CRect docRect = pFlash->m_Rect;
		m_hlGeomList.AddTail(pFlash);
		const CPoint docPoint = pFlash->GetGeogroid();
		CRect client;
		GetClientRect(client);
		const CPoint cliPoint = client.CenterPoint();
		const Gdiplus::Rect cliRect = m_viewMonitor.DocToClient<Gdiplus::Rect>(docRect);
		CRect cliView;
		GetClientRect(cliView);
		if(cliRect.Width!=0&&cliRect.Height!=0&&(cliRect.Width>cliView.Width()||cliRect.Height>cliView.Height()))
		{
			const float xRatio = (float)cliView.Width()/cliRect.Width;
			const float yRatio = (float)cliView.Height()/cliRect.Height;
			const float fRatio = min(xRatio, yRatio);
			m_viewMonitor.ZoomBy(this, fRatio, cliPoint);

			//CStatusBarEx* pStatusBar = this->GetStatusBar();
			//if(pStatusBar != nullptr)
			//{
			//	pStatusBar->SetViewScale(m_viewMonitor.m_scaleCurrent);
			//}
		}
		const CPointF mapPoint = m_viewMonitor.m_datainfo.DocToMap(docPoint);
		m_viewMonitor.FixateAt(this, mapPoint, cliPoint);
	}

	return 0;
}

LONG CGrfView::OnTopoPick(UINT WPARAM, LONG LPARAM)
{
	const CPoint docPoint = CPoint(WPARAM, LPARAM);
	const CSize nRadius1 = m_viewMonitor.ClientToDoc(Gdiplus::SizeF(4, 4));
	const CSize nRadius2 = m_viewMonitor.ClientToDoc(Gdiplus::SizeF(3, 3));
	for(std::list<CTopology*>::reverse_iterator it = m_document.m_topolist.rbegin(); it!=m_document.m_topolist.rend(); ++it)
	{
		CTopology* pTopology = *it;
		if(pTopology->m_bVisible==false)
			continue;

		CNode* pNode = pTopology->PickNode(m_viewMonitor, docPoint, nRadius1.cx);
		if(pNode!=nullptr)
		{
			if(pNode!=pTopology->m_pActivateNode)
			{
				pTopology->InactivateAll(this, m_viewMonitor);
				pTopology->ActivateNode(this, m_viewMonitor, pNode);
			}

			return 1;
		}
	}
	for(std::list<CTopology*>::reverse_iterator it = m_document.m_topolist.rbegin(); it!=m_document.m_topolist.rend(); ++it)
	{
		CTopology* pTopology = *it;
		if(pTopology->m_bVisible==false)
			continue;


		CEdge* pEdge = pTopology->PickEdge(m_viewMonitor, docPoint, nRadius2.cx);
		if(pEdge!=nullptr)
		{
			if(::GetKeyState(VK_SHIFT)>=0)
			{
				pTopology->InactivateAll(this, m_viewMonitor);
			}

			pTopology->ActivateEdge(this, m_viewMonitor, pEdge);
			return 1;
		}
	}
	if(::GetKeyState(VK_SHIFT)>=0)
	{
		for(std::list<CTopology*>::reverse_iterator it = m_document.m_topolist.rbegin(); it!=m_document.m_topolist.rend(); ++it)
		{
			CTopology* pTopology = *it;
			if(pTopology->m_bVisible==false)
				continue;

			pTopology->InactivateAll(this, m_viewMonitor);
		}
	}
	return 0L;
}

LONG CGrfView::OnTopoPickByPolygon(UINT WPARAM, LONG LPARAM)
{
	CPoly& polygon = *(CPoly*)WPARAM;

	for(std::list<CTopology*>::reverse_iterator it = m_document.m_topolist.rbegin(); it!=m_document.m_topolist.rend(); it++)
	{
		CTopology* pTopology = *it;
		pTopology->PickEdges(this, m_viewMonitor, polygon);
	}

	return 0L;
}

LONG CGrfView::OnTopoSplit(UINT WPARAM, LONG LPARAM)
{
	//for(std::list<CTopology*>::reverse_iterator it = m_document.m_topolist.rbegin(); it != m_document.m_topolist.rend(); it++)
	//{
	//	CTopology* pTopology = *it;
	//	if(pTopology->m_bVisible == false)
	//		continue;
	//	if(pTopology->SplitEdge(this, m_viewMonitor, CPoint(WPARAM, LPARAM), 2, m_document.m_actionStack) == TRUE)
	//	{
	//		break;
	//	}
	//}
	return 0;
}
LONG CGrfView::OnTopoCross(UINT WPARAM, LONG LPARAM)
{
	const CPoint center = *(CPoint*)WPARAM;

	for(std::list<CTopology*>::iterator it = m_document.m_topolist.begin(); it!=m_document.m_topolist.end(); it++)
	{
		CTopology* pTopology = *it;
		if(pTopology->CrossEdges(this, m_viewMonitor, center, LPARAM, m_document.m_actionStack)==true)
		{
			break;
		}
	}

	return 0;
}

LONG CGrfView::OnGetViewinfo(UINT WPARAM, LONG LPARAM)
{
	return (LONG)&m_viewMonitor;
}
void CGrfView::ChangeViewLevel(float fLevel)
{
	if(fLevel==m_viewMonitor.m_levelCurrent)
	{
	}
	else if(fLevel>=m_viewMonitor.m_levelMinimum&&fLevel<=m_viewMonitor.m_levelMaximum)
	{
		CRect rect;
		this->GetClientRect(rect);
		const CPoint cliPoint = rect.CenterPoint();
		const CPointF mapPoint = m_viewMonitor.ClientToDataMap(cliPoint);

		m_viewMonitor.LevelTo(this, fLevel, mapPoint);
		CChildFrame* pChildfrm = (CChildFrame*)GetParentFrame();
		pChildfrm->ResetRuler(m_viewMonitor);
	}
}

void CGrfView::ChangeViewScale(float fScale)
{
	if(fScale==m_viewMonitor.m_scaleCurrent)
	{
	}
	else if(fScale<=m_document.m_Datainfo.m_scaleMinimum&&fScale>=m_document.m_Datainfo.m_scaleMaximum)
	{
		CRect rect;
		this->GetClientRect(rect);
		const CPoint cliPoint = rect.CenterPoint();
		const CPointF mapPoint = m_viewMonitor.ClientToDataMap(cliPoint);

		m_viewMonitor.ScaleTo(this, fScale, mapPoint);
		CChildFrame* pChildfrm = (CChildFrame*)GetParentFrame();
		pChildfrm->ResetRuler(m_viewMonitor);
	}
}

LONG CGrfView::OnPublishPC(UINT WPARAM, LONG LPARAM)
{
	CDeck* pDeck = (CDeck*)WPARAM;
	CDaoRecordset* rs = (CDaoRecordset*)LPARAM;
	m_document.OnPublishPC(pDeck, rs);
	return 0;
}

LONG CGrfView::OnPublishCE(UINT WPARAM, LONG LPARAM)
{
	CDeck* pDeck = (CDeck*)WPARAM;
	m_document.OnPublishCE(pDeck);
	return 0;
}

void CGrfView::OnShowCentroid(UINT dwId)
{
	ARRANGE arrange;
	switch(dwId)
	{
		case ID_VIEW_LABELOID:
			m_document.preference.ShowLabeloid ^= true;
			break;
		case ID_VIEW_GEOGROID:
			m_document.preference.ShowGeogroid ^= true;
			break;
	}

	this->Invalidate(TRUE);
	SendMessage(WM_DOCMODIFIED, 0, TRUE);
}

void CGrfView::OnShowTags(UINT dwId)
{
	ARRANGE arrange;
	switch(dwId)
	{
		case ID_VIEW_CREATEDTAGS:
			m_document.preference.ShowCreatedTags ^= true;
			break;
		case ID_VIEW_DYNAMICTAGS:
			m_document.preference.ShowDynamicTags ^= true;
			break;
	}

	this->Invalidate(TRUE);
	SendMessage(WM_DOCMODIFIED, 0, TRUE);
}
