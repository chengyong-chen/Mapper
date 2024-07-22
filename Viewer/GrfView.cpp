#include "stdafx.h"
#include <afxpriv.h>
#include "Global.h"

#include "GrfView.h"
#include "GrfDoc.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "resource.h"

#include "../Mapper/Global.h"

#include "../Atlas/Atlas1.h"
#include "../Atlas/Page.h"

#include "../Tool/Global.h"
#include "../Tool/Tool.h"
#include "../Tool/SelectTool.h"
#include "../Tool/ZoomTool.h"
#include "../Tool/PrintTool.h"
#include "../Tool/panTool.h"
#include "../Tool/MarqueeTool.h"
#include "../Tool/RadiusTool.h"
#include "../Tool/BoundTool.h"
#include "../Tool/RegionTool.h"
#include "../Tool/LengthTool.h"
#include "../Tool/AreaTool.h"
#include "../Tool/DispatchTool.h"
#include "../Tool/LinkTool.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Mark.h"
#include "../Geometry/Doughnut.h"
#include "../Geometry/Global.h"

#include "../Layer/Global.h"
#include "../Layer/LayerTreeCtrl.h"
#include "../Layer/LayerPane.h"
#include "../Layer/GridMapFile.h"
#include "../Layer/GeomMapFile.h"

#include "../Dataview/Global.h"
#include "../Dataview/ViewPrinter.h"
#include "../Dataview/Background.h"

#include "../information/Global.h"

#include "../Utility/Rect.hpp"



#define IDC_MAINBAR_TAB 100

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern __declspec(dllimport) CSelectTool selectTool;
extern __declspec(dllimport) CZoomTool zoominTool;
extern __declspec(dllimport) CZoomTool zoomoutTool;
extern __declspec(dllimport) CPrintTool printTool;
extern __declspec(dllimport) CLinkTool linkTool;
extern __declspec(dllimport) CPanTool panTool;
extern __declspec(dllimport) CMarqueeTool marqueeTool;
extern __declspec(dllimport) CRadiusTool radiusTool;
extern __declspec(dllimport) CBoundTool boundTool;
extern __declspec(dllimport) CRegionTool regionTool;
extern __declspec(dllimport) CLengthTool lengthTool;
extern __declspec(dllimport) CAreaTool areaTool;
extern __declspec(dllimport) CDispatchTool dispatchTool;

extern __declspec(dllimport) CArray<Gdiplus::RectF, Gdiplus::RectF&> d_oRectArray;

short CGrfView::m_nAngle = 0;

CLIPFORMAT CGrfView::m_ClipboardFormat = (CLIPFORMAT)

#ifdef _MAC
::RegisterClipboardFormat(_T("MAC Diwatu GEOM"));
#else
::RegisterClipboardFormat(_T("PC Diwatu GEOM"));
#endif

IMPLEMENT_GWDYNCREATE(CGrfView, CView)

BEGIN_MESSAGE_MAP(CGrfView, CView)
	//{{AFX_MSG_MAP(CGrfView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()

	ON_WM_SIZE()
	ON_WM_SETFOCUS()

	ON_WM_TIMER()

	ON_COMMAND(ID_TOOL_ZOOMIN, OnToolZoomIn)
	ON_COMMAND(ID_TOOL_ZOOMOUT, OnToolZoomOut)
	ON_COMMAND(ID_TOOL_PRINT, OnToolPrint)
	ON_COMMAND(ID_TOOL_PAN, OnToolPan)
	ON_COMMAND(ID_TOOL_RADIUS, OnToolRadius)
	ON_COMMAND(ID_TOOL_BOUND, OnToolBound)
	ON_COMMAND(ID_TOOL_REGION, OnToolRegion)
	ON_COMMAND(ID_TOOL_LENGTH, OnToolLength)
	ON_COMMAND(ID_TOOL_AREA, OnToolArea)
	ON_COMMAND(ID_TOOL_DISPATCH, OnGpsDispatch)
	ON_COMMAND(ID_TOOL_MARQUEE, OnToolMarquee)
	ON_COMMAND(ID_MAP_LEVELSLIDER, OnLevelSliderPosChanged)
	ON_COMMAND(ID_MAP_SCALESLIDER, OnScaleSliderPosChanged)
	ON_COMMAND(ID_MAP_LEVELGO, OnLevelGo)
	ON_COMMAND(ID_MAP_SCALEGO, OnScaleGo)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)

	ON_MESSAGE(WM_GEOPOINTINMAP, OnGeoPointInMap)
	ON_MESSAGE(WM_LOOKUPGEOM, OnLookupGeom)
	ON_MESSAGE(WM_GETPAPERSIZE, OnGetPaperSize)
	ON_MESSAGE(WM_SETPRINTARECT, OnSetPrintRect)
	ON_MESSAGE(WM_GETVIEWINFO, OnGetViewinfo)
	ON_MESSAGE(WM_ZOOMIN, OnZoomIn)
	ON_MESSAGE(WM_ZOOMOUT, OnZoomOut)
	ON_MESSAGE(WM_CANZOOMIN, OnCanZoomIn)
	ON_MESSAGE(WM_FIXATEVIEW, OnFixateView)
	ON_MESSAGE(WM_FIXATERECT, OnFixateRect)
	ON_MESSAGE(WM_RGNQUERY, OnRgnQuery)

	ON_MESSAGE(WM_FLASHGEOM, OnFlashGeomtry)
	ON_MESSAGE(WM_KEYQUERY, OnKeyQuery)
END_MESSAGE_MAP()

CGrfView::CGrfView(CObject& parameter)
	: m_document((CGrfDoc&)parameter), m_viewMonitor(((CGrfDoc&)parameter).m_Datainfo), m_viewPrinter(((CGrfDoc&)parameter).m_Datainfo)
{
	m_hDrawOver = CreateEvent(nullptr, TRUE, FALSE, nullptr);;
	ResetEvent(m_hDrawOver);

	m_nAngle = 0;

	m_pFlash = nullptr;
	m_nDelay = 0;

	m_pFixed = CPoint(0, 0);

	m_viewMonitor.m_bViewer = true;
	m_viewPrinter.m_bViewer = true;
}

CGrfView::~CGrfView()
{
}

void CGrfView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	const CRectF mapRect = m_document.m_Datainfo.GetMapRect();
	m_viewMonitor.ScaleTo(this, m_document.m_Datainfo.m_scaleSource, mapRect.CenterPoint());
	/*	if(m_nAngle != 0)
		{
			double fViewRotateSin = sin(m_nAngle*M_PI/180);
			double fViewRotateCos = cos(m_nAngle*M_PI/180);

			CPoint point1;
			point1.x = long(fViewRotateCos*cx);
			point1.y = long(fViewRotateSin*cx);

			CPoint point2;
			point2.x = long(fViewRotateCos*cx-fViewRotateSin*cy);
			point2.y = long(fViewRotateSin*cx+fViewRotateCos*cy);

			CPoint point3;
			point3.x = long(-fViewRotateSin*cy);
			point3.y = long( fViewRotateCos*cy);

			CRect rect;
			rect.left   = min(min(0,point1.x),min(point2.x,point3.x));
			rect.top    = min(min(0,point1.y),min(point2.y,point3.y));
			rect.right  = max(max(0,point1.x),max(point2.x,point3.x));
			rect.bottom = max(max(0,point1.y),max(point2.y,point3.y));

			cx = rect.Width();
			cy = rect.Height();
		}

		int cxView = cx*m_doctoviewZoomX;
		int cyView = cy*m_doctoviewZoomY;

		CRect rect;
		GetClientRect(rect);
		if(rect.Width() > cxView && rect.Height() > cyView)
		{
			float xZoom = (float)rect.Width() /cx;
			float yZoom = (float)rect.Height()/cy;

			m_doctoviewZoomX = min(xZoom,yZoom);
			m_doctoviewZoomY = min(xZoom,yZoom);
			m_viewMonitor.m_scaleCurrent  = m_document.m_Datainfo.GetScale(m_doctoviewZoomX,m_doctoviewZoomY,docCenter);
		}

		this->SetViewSize(CPoint(0,0),CPoint(0,0),FALSE);*/

	this->SendMessage(WM_LAYERACTIVATED, 0, 0);
}

BOOL CGrfView::OnEraseBkgnd(CDC* pDC)
{
	//CRect rect;
	//this->GetClientRect(rect);
	//if(m_viewMonitor.m_pBackground != nullptr)
	//	pDC->FillSolidRect(rect,RGB(255,255,254));
	//else
	//	pDC->FillSolidRect(rect,RGB(255,255,255));
	return TRUE;
}

CRect pageRect;
/////////////////////////////////////////////////////////////////////////////
// CGrfView drawing

void CGrfView::OnDraw(CDC* pDC)
{
	d_oRectArray.RemoveAll();

	CSize dpi(pDC->GetDeviceCaps(LOGPIXELSX), pDC->GetDeviceCaps(LOGPIXELSY));
	if(pDC->IsPrinting()==FALSE)
	{
		CRect clipBox;
		GetClientRect(clipBox);
		clipBox.NormalizeRect();
		if(clipBox.IsRectNull())
			return;

		AfxGetMainWnd()->SendMessage(WM_VIEWWILLDRAW, (UINT)this, 0);
		if(m_document.m_pTool!=nullptr)
			m_document.m_pTool->Draw(this, m_viewMonitor, pDC);

		Gdiplus::Bitmap bitmap(clipBox.Width(), clipBox.Height(), PixelFormat32bppARGB);
		Gdiplus::Graphics g1(&bitmap);

		g1.SetPageUnit(Gdiplus::Unit::UnitPixel);
		const HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();
		const LONG nStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
		if((nStyle&WS_EX_LAYERED)!=WS_EX_LAYERED)
		{
			g1.Clear(Gdiplus::Color(255, 255, 255));
			g1.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
			g1.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQualityBicubic);
			g1.SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHighQuality);
			g1.SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintAntiAlias);
		}
		g1.TranslateTransform(-clipBox.left, -clipBox.top);

		CRect inRect = CRect(-m_viewMonitor.m_ptViewPos, m_viewMonitor.m_sizeView);
		inRect.IntersectRect(inRect, clipBox);
		inRect = m_viewMonitor.ClientToDoc(inRect);			
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
		const CClientDC dc(this);
		Gdiplus::Graphics g2(dc.m_hDC);
		g2.DrawImage(&bitmap, 0, 0);
		g2.ReleaseHDC(dc.m_hDC);

		AfxGetMainWnd()->SendMessage(WM_VIEWDRAWOVER, (UINT)this, 0);//ÐèÒªºÏÊÊm_uZoom£¬Ô­À´ÎªdoctoviewZoom;

		if(m_document.m_pTool!=nullptr)
			m_document.m_pTool->Draw(this, m_viewMonitor, pDC);
		const CPoint oldOrg = pDC->SetViewportOrg(0, 0);

		CRect rect;
		this->GetClientRect(rect);
		const CRect docRect = m_viewMonitor.ClientToDoc(rect);

		for(CTree<CLayer>::postiterator it = m_document.m_layertree.postbegin(); it!=m_document.m_layertree.postend(); ++it)
		{
			CLayer* layer = *it;
			layer->DrawAnchors(pDC, m_viewMonitor, docRect);
		}

		if(m_pFixed!=CPoint(0, 0))
		{
			const Gdiplus::Point cliPoint = m_viewMonitor.DocToClient<Gdiplus::Point>(m_pFixed);

			CClientDC dc(this);
			CPen pen(PS_SOLID, 2, (COLORREF)RGB(255, 0, 0));
			CPen* oldPen = dc.SelectObject(&pen);
			CBrush* oldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);

			dc.Ellipse(cliPoint.X-11, cliPoint.Y-11, cliPoint.X+11, cliPoint.Y+11);

			dc.MoveTo(cliPoint.X-20, cliPoint.Y);
			dc.LineTo(cliPoint.X+20, cliPoint.Y);
			dc.MoveTo(cliPoint.X, cliPoint.Y-20);
			dc.LineTo(cliPoint.X, cliPoint.Y+20);

			dc.SelectObject(oldPen);
			dc.SelectObject(oldBrush);
			pen.DeleteObject();
		}

		pDC->SetViewportOrg(oldOrg);

		SetEvent(m_hDrawOver);
	}
	else
	{
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
		//	g.SetClip(Rect(inRect.left,inRect.top,inRect.Width(),inRect.Height()));//需要恢复

		m_document.Draw(this, m_viewPrinter, g, inRect);
		m_document.DrawTag(this, m_viewPrinter, g, inRect);

		g.ReleaseHDC(pDC->m_hDC);
		pDC->RestoreDC(-1);
	}
	d_oRectArray.RemoveAll();
}

BOOL CGrfView::KillTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
	case TIMER_FLASHGEOM:
	{
		if(m_pFlash!=nullptr)
		{
			CTag* pTag = m_pFlash->m_pTag;
			m_pFlash->m_pTag = nullptr;
			m_pFlash->Invalidate(this, m_viewMonitor, 9);
			m_pFlash->m_pTag = pTag;
			m_pFlash = nullptr;
		}
		m_nDelay = 0;
	}
	break;
	default:
		break;
	}

	return CView::KillTimer(nIDEvent);
}

CLayer* CGrfView::GetActiveLayer() const
{
	if(m_document.m_pLayerTreeCtrl!=nullptr)
	{
		CLayer* layer = m_document.m_pLayerTreeCtrl->GetSelLayer();
		return layer;
	}

	return nullptr;
}

void CGrfView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CView::OnLButtonDown(nFlags, point);

	if(m_document.m_pTool!=nullptr)
	{
		const CPoint docPoint = m_viewMonitor.ClientToDoc(point, false);
		m_document.m_pTool->OnLButtonDown(this, m_viewMonitor, nFlags, point, docPoint);
	}
}

void CGrfView::OnMouseMove(UINT nFlags, CPoint point)
{
	CView::OnMouseMove(nFlags, point);

	if(m_document.m_pTool!=nullptr)
	{
		const CPoint docPoint = m_viewMonitor.ClientToDoc(point, false);
		m_document.m_pTool->OnMouseMove(this, m_viewMonitor, nFlags, point, docPoint);
	}

	//	if(CMainFrame::m_Security.HasViewCoordinateRight() == true)
	{
		const CPointF geoPoint = m_viewMonitor.ClientToGeo(point);
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar1;
		pMainFrame->m_wndRibbonBar.GetElementsByID(ID_COORDINATE_LONGITUDE, ar1);
		if(ar1.GetSize()>=1)
		{
			CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, ar1[0]);
			if(pEdit!=nullptr)
			{
				CString string;
				string.Format(_T("%3.4f"), geoPoint.x);
				pEdit->SetEditText(string);
			}
		}

		CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar2;
		pMainFrame->m_wndRibbonBar.GetElementsByID(ID_COORDINATE_LATITUDE, ar2);
		if(ar2.GetSize()>=1)
		{
			CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, ar2[0]);
			if(pEdit!=nullptr)
			{
				CString string;
				string.Format(_T("%3.4f"), geoPoint.y);
				pEdit->SetEditText(string);
			}
		}
	}
}

BOOL CGrfView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if(::GetKeyState(VK_SHIFT)>=0)
		OnScrollBy(CSize(0, -zDelta/2), TRUE);
	else
		OnScrollBy(CSize(-zDelta/2, 0), TRUE);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CGrfView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CView::OnLButtonUp(nFlags, point);

	if(CWnd::GetCapture()!=this)
	{
	}
	else if(m_document.m_pTool!=nullptr)
	{
		const CPoint docPoint = m_viewMonitor.ClientToDoc(point, false);
		m_document.m_pTool->OnLButtonUp(this, m_viewMonitor, nFlags, point, docPoint);
	}
	if(GetKeyState(VK_CONTROL)>=0&&m_document.m_pTool==&selectTool&&m_document.m_oldTool!=nullptr)
	{
		m_document.m_pTool = m_document.m_oldTool;
		m_document.m_oldTool = nullptr;
		m_document.m_pTool->SetCursor(this, m_viewMonitor);
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

	if(m_document.m_pTool!=nullptr&&m_document.m_pTool->CanPan()==true)
	{
		m_document.m_oldTool = m_document.m_pTool;
		m_document.m_pTool = &panTool;
		m_document.m_pTool->SetCursor(this, m_viewMonitor);
		const CPoint docPoint = m_viewMonitor.ClientToDoc(point, false);
		m_document.m_pTool->OnLButtonDown(this, m_viewMonitor, nFlags, point, docPoint);
	}
}

void CGrfView::OnRButtonUp(UINT nFlags, CPoint point)
{
	CView::OnRButtonUp(nFlags, point);

	if(m_document.m_pTool==&panTool)
	{
		const CPoint docPoint = m_viewMonitor.ClientToDoc(point, false);
		m_document.m_pTool->OnLButtonUp(this, m_viewMonitor, nFlags, point, docPoint);

		m_document.m_pTool->Recover(this, m_viewMonitor, m_document.m_pTool, m_document.m_oldTool);
		m_document.m_pTool->SetCursor(this, m_viewMonitor);
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
#endif//_DEBUG

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

void CGrfView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if(m_hWnd!=nullptr)
	{
		m_viewMonitor.OnSized(this, cx, cy);
	}

	//	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);	
	//	if(pActiveItem != nullptr)
	//		pActiveItem->SetItemRects();
}

void CGrfView::OnSetFocus(CWnd* pOldWnd)
{
	COleClientItem* pActiveItem = m_document.GetInPlaceActiveItem(this);
	if(pActiveItem!=nullptr&&pActiveItem->GetItemState()==COleClientItem::activeUIState)
	{
		// need to set focus to this item ifit is in the same view
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

BOOL CGrfView::OnPreparePrinting(CPrintInfo* pInfo)
{
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

void CGrfView::UnhighligtAll()
{
	POSITION pos = m_hlGeomList.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_hlGeomList.GetNext(pos);
		CLayer* pLayer = m_document.m_layertree.GetLayerByGeom(pGeom);
		pLayer->Invalidate(this, m_viewMonitor, pGeom);
	}
	m_hlGeomList.RemoveAll();
}

LONG CGrfView::OnZoomIn(UINT WPARAM, LONG LPARAM)
{
	CChildFrame* pChildFrm = (CChildFrame*)this->GetParentFrame();
	if(m_viewMonitor.m_levelCurrent<=m_viewMonitor.m_levelMaximum)
	{
		const CRect docRect = *(CRect*)WPARAM;
		CPoint docPoint;
		docPoint.x = docRect.left+docRect.Width()/2;
		docPoint.y = docRect.top+docRect.Height()/2;
		CPointF geoPoint = m_document.m_Datainfo.DocToGeo(docPoint);

		if(m_document.m_Datainfo.m_pProjection!=nullptr)
		{
			const Gdiplus::Rect cliRect = m_viewMonitor.DocToClient <Gdiplus::Rect>(docRect);
			const CPoint cliPoint = CPoint(cliRect.X+cliRect.Width/2, cliRect.Y+cliRect.Height/2);

			CString stringX;
			CString stringY;
			stringX.Format(_T("%.5f"), geoPoint.x);
			stringY.Format(_T("%.5f"), geoPoint.y);
			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"), stringX);
			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"), stringY);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"), cliPoint.x);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"), cliPoint.y);
		}

		if(m_document.m_pDeck!=nullptr&&m_document.m_Datainfo.m_pProjection!=nullptr)
		{
			CDeck* pDeck = m_document.m_pDeck->GetChild(geoPoint);
			if(pDeck!=nullptr)
			{
				CFrameWnd* pChildFrm = this->GetParentFrame();
				return (LONG)pDeck->Open(pChildFrm, -1);
			}
		}
		AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"), _T(""));
		AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"), _T(""));
		AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"), 0);
		AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"), 0);
	}
	else if(LPARAM!=0)
	{
		CRect rect;
		this->GetClientRect(rect);
		AfxGetMainWnd()->SendMessage(WM_VIEWWILLDRAW, (UINT)this, 0);
		const CRect docRect = *(CRect*)WPARAM;
		CPoint docPoint = docRect.CenterPoint();
		docPoint.x = docRect.left+docRect.Width()/2;//µ±docRectµÄÖµ¹ý´óÊ±£¬CenterPoint£¨£©º¯ÊýÖ´ÐÐÓÐÎó
		docPoint.y = docRect.top+docRect.Height()/2;
		const CPointF mapPoint = m_document.m_Datainfo.DocToMap(docPoint);
		const float xZoom = *(float*)LPARAM;
		const float yZoom = *(float*)LPARAM;
		const float zoom = min(xZoom, yZoom);
		m_viewMonitor.ScaleTo(this, m_viewMonitor.m_scaleCurrent/zoom, mapPoint);
		
		((CMainFrame*)AfxGetMainWnd())->OnLevelChanged(m_viewMonitor.m_levelCurrent);
		((CMainFrame*)AfxGetMainWnd())->OnScaleChanged(m_viewMonitor.m_scaleCurrent);		
	}
	else
	{
		AfxGetMainWnd()->SendMessage(WM_VIEWWILLDRAW, (UINT)this, 0);
		const CRect docRect = *(CRect*)WPARAM;
		const Gdiplus::Rect cliRect = m_viewMonitor.DocToClient <Gdiplus::Rect>(docRect);
		const CPoint cliPoint = CPoint(cliRect.X+cliRect.Width/2, cliRect.Y+cliRect.Height/2);
		if(cliRect.Width<5||cliRect.Height<5)
		{
			m_viewMonitor.ZoomBy(this, 2, cliPoint);
		}
		else
		{
			CRect clientRect;
			this->GetClientRect(clientRect);

			const float xRatio = (float)clientRect.Width()/cliRect.Width;
			const float yRatio = (float)clientRect.Height()/cliRect.Height;
			const float fRatio = min(xRatio, yRatio);

			m_viewMonitor.ZoomBy(this, fRatio, cliPoint);
		}
		((CMainFrame*)AfxGetMainWnd())->OnLevelChanged(m_viewMonitor.m_levelCurrent);
		((CMainFrame*)AfxGetMainWnd())->OnScaleChanged(m_viewMonitor.m_scaleCurrent);
	}

	return 1;
}

LONG CGrfView::OnZoomOut(UINT WPARAM, LONG LPARAM)
{
	if(m_viewMonitor.m_levelCurrent > m_viewMonitor.m_levelMinimum)
	{
		if(m_document.m_pDeck!=nullptr)
		{
			CDeck* pDeck = m_document.m_pDeck->GetParent();
			if(pDeck!=nullptr)
			{
				if(m_document.m_Datainfo.m_pProjection!=nullptr)
				{
					CRect cliRect;
					this->GetClientRect(cliRect);
					const CPoint cliPoint = cliRect.CenterPoint();
					const CPointF geoPoint = m_viewMonitor.ClientToGeo(cliPoint);

					CString stringX;
					CString stringY;
					stringX.Format(_T("%.5f"), geoPoint.x);
					stringY.Format(_T("%.5f"), geoPoint.y);
					AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"), stringX);
					AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"), stringY);
					AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"), cliPoint.x);
					AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"), cliPoint.y);
				}

				CChildFrame* pChildFrm = (CChildFrame*)this->GetParentFrame();
				return (LONG)pDeck->Open(pChildFrm, 1);
			}
		}
	}
	else
	{
		const CRect docRect = *(CRect*)WPARAM;

		CPoint docPoint = docRect.CenterPoint();
		docPoint.x = docRect.left+docRect.Width()/2;//µ±docRectµÄÖµ¹ý´óÊ±£¬CenterPoint£¨£©º¯ÊýÖ´ÐÐÓÐÎó
		docPoint.y = docRect.top+docRect.Height()/2;
		const Gdiplus::Point cliPoint = m_viewMonitor.DocToClient<Gdiplus::Point>(docPoint);

		AfxGetMainWnd()->SendMessage(WM_VIEWWILLDRAW, (UINT)this, 0);

		m_viewMonitor.ZoomBy(this, 0.5, CPoint(cliPoint.X, cliPoint.Y));
		((CMainFrame*)AfxGetMainWnd())->OnLevelChanged(m_viewMonitor.m_levelCurrent);
		((CMainFrame*)AfxGetMainWnd())->OnScaleChanged(m_viewMonitor.m_scaleCurrent);
	}

	return 1L;
}

LONG CGrfView::OnCanZoomIn(UINT WPARAM, LONG LPARAM)
{
	if(m_viewMonitor.m_levelCurrent < m_viewMonitor.m_levelMaximum)
		return TRUE;
	else if(m_document.m_pDeck!=nullptr&&m_document.m_Datainfo.m_pProjection!=nullptr)
	{
		const CPoint cliPoint = *(CPoint*)LPARAM;
		CPointF geoPoint = m_viewMonitor.ClientToGeo(cliPoint);

		CDeck* pDeck = m_document.m_pDeck->GetChild(geoPoint);
		if(pDeck!=nullptr)
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}

void CGrfView::OnToolZoomIn()
{
	if(m_document.m_pTool!=&zoominTool)
	{
		m_document.m_pTool = &zoominTool;
	}
	else if(m_viewMonitor.m_levelCurrent < m_viewMonitor.m_levelMaximum)
	{
		CRect cliRect;
		GetClientRect(cliRect);
		const CPoint cliPoint = cliRect.CenterPoint();

		AfxGetMainWnd()->SendMessage(WM_VIEWWILLDRAW, (UINT)this, 0);
		m_viewMonitor.ZoomBy(this, 2, cliPoint);
		((CMainFrame*)AfxGetMainWnd())->OnLevelChanged(m_viewMonitor.m_levelCurrent);
		((CMainFrame*)AfxGetMainWnd())->OnScaleChanged(m_viewMonitor.m_scaleCurrent);
	}
}

void CGrfView::OnToolZoomOut()
{
	if(m_viewMonitor.m_levelCurrent>m_viewMonitor.m_levelMinimum)
	{
		if(m_document.m_pDeck!=nullptr)
		{
			CDeck* pDeck = m_document.m_pDeck->GetParent();
			if(pDeck!=nullptr)
			{
				if(m_document.m_Datainfo.m_pProjection!=nullptr)
				{
					CRect cliRect;
					this->GetClientRect(cliRect);
					const CPoint cliPoint = cliRect.CenterPoint();
					const CPointF geoPoint = m_viewMonitor.ClientToGeo(cliPoint);

					CString stringX;
					CString stringY;
					stringX.Format(_T("%.5f"), geoPoint.x);
					stringY.Format(_T("%.5f"), geoPoint.y);
					AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"), stringX);
					AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"), stringY);
					AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"), cliPoint.x);
					AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"), cliPoint.y);
				}

				CChildFrame* pChildFrm = (CChildFrame*)this->GetParentFrame();
				pDeck->Open(pChildFrm, 1);
				return;
			}
		}
	}
	else
	{
		CRect cliRect;
		GetClientRect(cliRect);
		const CPoint cliPoint = cliRect.CenterPoint();

		AfxGetMainWnd()->SendMessage(WM_VIEWWILLDRAW, (UINT)this, 0);
		m_viewMonitor.ZoomBy(this, 0.5, cliPoint);
		((CMainFrame*)AfxGetMainWnd())->OnLevelChanged(m_viewMonitor.m_levelCurrent);
		((CMainFrame*)AfxGetMainWnd())->OnScaleChanged(m_viewMonitor.m_scaleCurrent);
	}
}

BOOL CGrfView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
	CView::OnScrollBy(sizeScroll, bDoScroll);

	m_viewMonitor.MoveBy(this, sizeScroll.cx, sizeScroll.cy);

	if(bDoScroll==TRUE)
	{
		CClientDC dc(this);
		OnPrepareDC(&dc, nullptr);

		OnDraw(&dc);
	}
	return TRUE;
}

LONG CGrfView::OnFixateView(UINT WPARAM, LONG LPARAM)
{
	const CPoint docPoint = *(CPoint*)WPARAM;
	const CPoint cliPoint = *(CPoint*)LPARAM;
	const CPointF mapPoint = m_document.m_Datainfo.DocToMap(docPoint);
	m_viewMonitor.FixateAt(this, mapPoint, cliPoint);
	return 1;
}

LONG CGrfView::OnFixateRect(UINT WPARAM, LONG LPARAM)
{
	const CRect docRect = *(CRect*)WPARAM;
	const Gdiplus::Rect cliRect = m_viewMonitor.DocToClient <Gdiplus::Rect>(docRect);

	CRect rect;
	this->GetClientRect(rect);
	const float xZoom = (float)cliRect.Width/rect.Width();
	const float yZoom = (float)cliRect.Height/rect.Height();

	m_viewMonitor.ZoomBy(this, min(xZoom, yZoom), rect.CenterPoint());

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->OnLevelChanged(m_viewMonitor.m_levelCurrent);
	pMainFrame->OnScaleChanged(m_viewMonitor.m_scaleCurrent);

	return 1L;
}

LONG CGrfView::OnGetPaperSize(UINT WPARAM, LONG LPARAM)
{
	static CSize size=m_document.m_Datainfo.GetDocCanvas();
	return (LONG)(CSize*)&size;
}

LONG CGrfView::OnSetPrintRect(UINT WPARAM, LONG LPARAM)
{
	const CRect* rect = (CRect*)LPARAM;
	m_viewPrinter.m_docPrint = *rect;
	return 1L;
}

void CGrfView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);

	if(bActivate==TRUE&&pActivateView==this)
	{
		CLayerPane* pLayerPane = (CLayerPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'L', 0);
		if(pLayerPane!=nullptr)
		{
			pLayerPane->SetCtrl(m_document.m_pLayerTreeCtrl);
		}

		CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
		//if(pMainFrame->m_pEaglePane != nullptr)
		//{
		//	CString strPath = m_document.GetPathName();
		//	strPath = strPath.Left(strPath.GetLength() - 4);
		//	strPath += _T(".bmp");
		//	CRect docRect = m_document.m_Datainfo.GetDocRect();
		//	pMainFrame->m_pEaglePane->OpenBmp(this,strPath,docRect.CSize());
		//	SetEagleRect();
		//}

		if(pMainFrame->m_wndRibbonBar.m_hWnd!=nullptr&&pMainFrame->m_wndRibbonBar.IsVisible()==TRUE)
		{
			CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
			pMainFrame->m_wndRibbonBar.GetElementsByID(ID_MAP_SCALESLIDER, ar);
			for(int index = 0; index<ar.GetSize(); index++)
			{
				CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, ar[index]);
				if(pSlider!=nullptr)
				{
					pSlider->SetRange(m_document.m_Datainfo.m_scaleMaximum, m_document.m_Datainfo.m_scaleMinimum);
					pSlider->SetZoomIncrement((m_document.m_Datainfo.m_scaleMinimum-m_document.m_Datainfo.m_scaleMaximum)*0.01);
				}
			}
			pMainFrame->OnScaleChanged(m_viewMonitor.m_scaleCurrent);
		}
		if(pMainFrame->m_wndRibbonBar.m_hWnd != nullptr && pMainFrame->m_wndRibbonBar.IsVisible() == TRUE)
		{
			CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
			pMainFrame->m_wndRibbonBar.GetElementsByID(ID_MAP_LEVELSLIDER, ar);
			for(int index = 0; index < ar.GetSize(); index++)
			{
				CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, ar[index]);
				if(pSlider != nullptr)
				{
					pSlider->SetRange(m_viewMonitor.m_levelMinimum, m_viewMonitor.m_levelMaximum);
					pSlider->SetZoomIncrement((m_viewMonitor.m_levelMaximum - m_viewMonitor.m_levelMinimum)*0.01);
				}
			}
			pMainFrame->OnLevelChanged(m_viewMonitor.m_levelCurrent);
		}
		CTablePane* pTablePane = (CTablePane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'O', 0);
		if(pTablePane!=nullptr)
		{
			CFrameWnd* pChildFrm = (CFrameWnd*)this->GetParentFrame();
			if(pChildFrm!=nullptr)
			{
				//ÐèÒª»Ö¸´
				//CVehicle* pVehicle = (CVehicle*)AfxGetMainWnd()->SendMessage(WM_GETVEHICLE,(DWORD)pChildFrm,1);
				//if(pTablePane->m_pMonitorDlg != nullptr && pVehicle != nullptr)
				//{
				//	pTablePane->pMonitorDlg->ActivateVehicle(pVehicle->m_dwId);
				//}
			}
		}
		if((GetAsyncKeyState(VK_LBUTTON)&0X8000)==0X8000)// VK_LBUTTON is down
		{
			this->SetCapture();
		}

		if(m_viewMonitor.m_pBackground!=nullptr)
		{
			m_viewMonitor.m_pBackground->SetWindowPos(AfxGetMainWnd()->GetSafeHwnd(), 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
		}

		pMainFrame->m_LayerPane.EnableBackgroundBar(FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGrfView scrolling

void CGrfView::SetEagleRect()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	//if(pMainFrame->m_pEaglePane != nullptr)
	//{
	//	CRect cliRect;
	//	GetClientRect(cliRect);
	//	ClientToDoc(cliRect);
	//	CRect vRect = cliRect;

	//	CGrfDoc* pDoc = GetDocument();
	//	CRect docRect = m_document.m_Datainfo.GetDocRect();
	//	
	//	vRect.IntersectRect(vRect,docRect);

	//	pMainFrame->m_pEaglePane->SetRect(this,vRect);
	//}
}

LONG CGrfView::OnLookupGeom(UINT WPARAM, LONG LPARAM)
{
	const CPoint point = *(CPoint*)WPARAM;

	//	unsigned long docDecimal = m_document.m_Datainfo.GetDecimal();
	const CSize docOffset = m_document.m_Datainfo.GetDocOrigin();

	int xGrid = 0;
	int yGrid = 0;
	if(m_document.m_pGridMapFile!=nullptr&&m_document.m_pGeomMapFile!=nullptr)
	{
		if(m_document.m_pGridMapFile->m_Head.dwWidth!=0)
		{
			xGrid = (point.x-docOffset.cx)/(int)(m_document.m_pGridMapFile->m_Head.dwWidth);
			if(xGrid<0)
				xGrid = 0;
			if(xGrid>=m_document.m_pGridMapFile->m_Head.xCount)
				xGrid = m_document.m_pGridMapFile->m_Head.xCount-1;
		}
		if(m_document.m_pGridMapFile->m_Head.dwHeight!=0)
		{
			yGrid = (point.y-docOffset.cy)/(int)(m_document.m_pGridMapFile->m_Head.dwHeight);
			if(yGrid<0)
				yGrid = 0;
			if(yGrid>=m_document.m_pGridMapFile->m_Head.yCount)
				yGrid = m_document.m_pGridMapFile->m_Head.yCount-1;
		}
	}

	CGeom* pGeom = nullptr;
	for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it!=m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(m_viewMonitor.m_levelCurrent<layer->m_minLevelObj)
			continue;
		if(m_viewMonitor.m_levelCurrent>=layer->m_maxLevelObj)
			continue;
		CATTDatasource* pDatasource = layer->GetAttDatasource();
		if(pDatasource==nullptr)
			return 0;
		CDatabase* pDatabase = m_document.m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
		if(pDatabase==nullptr)
			return 0;

		CGeom* geom = nullptr;
		if(LPARAM==3)
		{
			const bool oldVisible = layer->m_bVisible;
			layer->m_bVisible = true;
			geom = layer->Look(m_viewMonitor, point);
			layer->m_bVisible = oldVisible;
		}
		else
		{
			if(layer->m_bVisible==false)
				continue;

			geom = layer->Look(m_viewMonitor, point);
		}

		if(geom!=nullptr)
		{
			pGeom = geom;
			break;
		}

		if(m_document.m_pGridMapFile==nullptr||m_document.m_pGeomMapFile==nullptr)
			continue;
		const int wLayerIndex = m_document.m_pGridMapFile->GetLayerIndex(layer->m_wId);
		if(wLayerIndex==-1)
			continue;

		CGridMapFile::GRID* pLayerGrid = m_document.m_pGridMapFile->GetGrid(wLayerIndex, yGrid, xGrid);
		DWORD* dwGeomlist = m_document.m_pGridMapFile->GetGridGeomList(pLayerGrid);
		for(WORD index = 0; index<pLayerGrid->wGeomCount; index++)
		{
			DWORD dwGeomIndex = *dwGeomlist;
			dwGeomlist++;

			CGeomMapFile::GEOMHEAD* pGeomHead = m_document.m_pGeomMapFile->GetGeomHeadByIndex(wLayerIndex, dwGeomIndex);
			CGeom* geom = m_document.m_pGeomMapFile->GetGeom(pGeomHead);
			const CSize gap = layer->GetDocInflation(m_viewMonitor, geom);
			const bool bIn = layer->GetFill()!=nullptr||geom->m_pFill;
			if(geom->PickOn(point, gap.cx)==true||(bIn==true&&geom->PickIn(point)==true))
			{
				delete m_document.m_pGeomMapFile->m_pGet;
				m_document.m_pGeomMapFile->m_pGet = geom;
				pGeom = m_document.m_pGeomMapFile->m_pGet;
				break;
			}
			else
			{
				delete geom;
				geom = nullptr;
			}
		}
	}

	return (LONG)pGeom;
}

void CGrfView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(m_document.m_pTool!=nullptr)
	{
		m_document.m_pTool->OnKeyDown(this, m_viewMonitor, nChar, nRepCnt, nFlags, m_document.m_pTool, m_document.m_oldTool);
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

LONG CGrfView::OnRgnQuery(UINT WPARAM, LONG LPARAM)
{
	AfxGetMainWnd()->SendMessage(WM_ACTIVEMANAGER, 'Q', 0);

	const CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	CListCtrl* listCtrl = (CListCtrl*)pMainFrame->SendMessage(WM_GETQUERYCTRL, 0, 0);
	if(listCtrl!=nullptr&&listCtrl->m_hWnd!=nullptr)
	{
		listCtrl->DeleteAllItems();
		const CWnd* pWnd = listCtrl->GetParent();
		pWnd->SendMessage(WM_SETCOUNT, 0, 0);
	}
	else
	{
		return 0;
	}

	if(WPARAM==0)
		return 0;

	WORD wMap = 0;
	CString strMap = m_document.m_Datainfo.m_strName;
	if(m_document.m_pDeck!=nullptr)
	{
		wMap = m_document.m_pDeck->m_wId;
		strMap = m_document.m_pDeck->m_strName;
	}

	const CPoly* poly = nullptr;
	CRect inRect;
	switch(LPARAM)
	{
	case 0:
	{
		poly = (CPoly*)WPARAM;
		inRect = poly->m_Rect;
	}
	break;
	case 1:
	case 2:
	{
		inRect = *(CRect*)WPARAM;
	}
	break;
	}

	//	unsigned long docDecimal = m_document.m_Datainfo.GetDecimal();
	const CSize docOffset = m_document.m_Datainfo.GetDocOrigin();

	AfxGetApp()->BeginWaitCursor();
	long count = 0;
	for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it!=m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bKeyQuery==false)
			continue;

		CTypedPtrList<CObList, CGeom*>& geomlist = layer->GetGeomList();
		POSITION pos = geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			const CGeom* pGeom = geomlist.GetNext(pos);
			if(pGeom->m_strName.IsEmpty()==TRUE)
				continue;

			CRect rect = pGeom->m_Rect;
			if(Util::Rect::Intersect(pGeom->m_Rect, inRect)==false)
				continue;

			bool bRet = false;
			switch(LPARAM)
			{
			case 0:
				bRet = pGeom->PickIn(*poly);
				break;
			case 1:
				bRet = pGeom->PickIn(inRect);
				break;
			case 2:
			{
				CPoint docPoint;
				docPoint.x = inRect.left+inRect.Width()/2;//µ±zoomRectµÄÖµ¹ý´óÊ±£¬CenterPoint£¨£©º¯ÊýÖ´ÐÐÓÐÎó
				docPoint.y = inRect.top+inRect.Height()/2;
				bRet = pGeom->PickIn(docPoint, inRect.Width());
			}
			break;
			}

			if(bRet==true)
			{
				CString string = pGeom->m_strName;
				string.Replace(_T("\r\n"), _T(" "));
				//	string.Replace(_T("\r\n"),nullptr);
				//	string.Replace(_T(" "),nullptr);
				//	string.Replace(_T("¡¡"),nullptr);
				if(string.IsEmpty()==TRUE)
					continue;
				const int nItem = listCtrl->GetItemCount();
				const int index = listCtrl->InsertItem(nItem, nullptr);
				listCtrl->SetItemText(nItem, 0, string);
				listCtrl->SetItemText(nItem, 1, strMap);
				listCtrl->SetItemText(nItem, 2, layer->m_strName);

				PLACE* place = new PLACE;
				place->wMap = wMap;
				place->wLayer = layer->m_wId;
				place->dwGeom = pGeom->m_geoId;

				listCtrl->SetItemData(index, (DWORD)place);
				count++;
			}
		}

		if(m_document.m_pGridMapFile!=nullptr&&m_document.m_pGeomMapFile!=nullptr)
		{
			if(m_document.m_pGridMapFile->m_Head.dwWidth==0)
				continue;
			if(m_document.m_pGridMapFile->m_Head.dwHeight==0)
				continue;
			const int wLayerIndex = m_document.m_pGridMapFile->GetLayerIndex(layer->m_wId);
			if(wLayerIndex==-1)
				continue;

			int xMin = (inRect.left-docOffset.cx)/(int)(m_document.m_pGridMapFile->m_Head.dwWidth);
			int yMin = (inRect.top-docOffset.cy)/(int)(m_document.m_pGridMapFile->m_Head.dwHeight);
			int xMax = (inRect.right-docOffset.cx)/(int)(m_document.m_pGridMapFile->m_Head.dwWidth);
			int yMax = (inRect.bottom-docOffset.cy)/(int)(m_document.m_pGridMapFile->m_Head.dwHeight);

			if(xMin<0)
				xMin = 0;
			if(yMin<0)
				yMin = 0;
			if(xMax>=m_document.m_pGridMapFile->m_Head.xCount)
				xMax = m_document.m_pGridMapFile->m_Head.xCount-1;
			if(yMax>=m_document.m_pGridMapFile->m_Head.yCount)
				yMax = m_document.m_pGridMapFile->m_Head.yCount-1;

			CMap<DWORD, DWORD&, DWORD, DWORD&> usedGeom;
			for(int y = yMin; y<=yMax; y++)
				for(int x = xMin; x<=xMax; x++)
				{
					CGridMapFile::GRID* pLayerGrid = m_document.m_pGridMapFile->GetGrid(wLayerIndex, y, x);
					DWORD* dwGeomlist = m_document.m_pGridMapFile->GetGridGeomList(pLayerGrid);
					for(WORD index = 0; index<pLayerGrid->wGeomCount; index++)
					{
						DWORD dwGeomIndex = *dwGeomlist;
						dwGeomlist++;

						CGeomMapFile::GEOMHEAD* pGeomHead = m_document.m_pGeomMapFile->GetGeomHeadByIndex(wLayerIndex, dwGeomIndex);
						if(Util::Rect::Intersect(pGeomHead->rect, inRect)==false)
							continue;

						if(pGeomHead->bType!=8)
						{
							DWORD id;
							if(usedGeom.Lookup(pGeomHead->dwId, id))
								continue;
							else
								usedGeom.SetAt(pGeomHead->dwId, pGeomHead->dwId);
						}

						CGeom* pGeom = m_document.m_pGeomMapFile->GetGeom(pGeomHead);
						if(pGeom->m_strName.IsEmpty()==TRUE)
						{
							delete pGeom;
							pGeom = nullptr;
							continue;
						}

						bool bRet = false;
						switch(LPARAM)
						{
						case 0:
							bRet = pGeom->PickIn(*poly);
							break;
						case 1:
							bRet = pGeom->PickIn(inRect);
							break;
						case 2:
						{
							CPoint docPoint;
							docPoint.x = inRect.left+inRect.Width()/2;//µ±zoomRectµÄÖµ¹ý´óÊ±£¬CenterPoint£¨£©º¯ÊýÖ´ÐÐÓÐÎó
							docPoint.y = inRect.top+inRect.Height()/2;
							bRet = pGeom->PickIn(docPoint, inRect.Width());
						}
						break;
						}

						if(bRet==true)
						{
							PLACE* place = new PLACE;
							place->wMap = wMap;
							place->wLayer = layer->m_wId;
							place->dwGeom = pGeomHead->dwId;
							const int nItem = listCtrl->GetItemCount();
							const int index = listCtrl->InsertItem(nItem, nullptr);
							listCtrl->SetItemData(index, (DWORD)place);

							CString string = pGeom->m_strName;
							string.Replace(_T("\r\n"), _T(" "));
							//	string.Replace(_T("\r\n"),nullptr);
							//	string.Replace(_T(" "),nullptr);
							//	string.Replace(_T("¡¡"),nullptr);

							listCtrl->SetItemText(nItem, 0, string);
							listCtrl->SetItemText(nItem, 1, strMap);
							listCtrl->SetItemText(nItem, 2, layer->m_strName);

							count++;
						}
						delete pGeom;
						pGeom = nullptr;
					}
				}
			usedGeom.RemoveAll();
		}
	}

	const CWnd* pWnd = listCtrl->GetParent();
	pWnd->SendMessage(WM_SETCOUNT, count, 0);

	/*CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;	Need work
	BOOL bVisible = (pMainFrame->m_pMainBar->GetStyle() & WS_VISIBLE) != 0;
	if(bVisible == FALSE)
	{
		pMainFrame->ShowControlBar(pMainFrame->m_pMainBar, 1, FALSE);
		pMainFrame->RecalcLayout();
	}*/

	AfxGetApp()->EndWaitCursor();
	return count;
}

LONG CGrfView::OnGeoPointInMap(UINT WPARAM, LONG LPARAM)
{
	const CPointF geoPoint = *(CPointF*)WPARAM;

	if(geoPoint.x<m_document.m_Datainfo.m_mapOrigin.x)
		return FALSE;
	if(geoPoint.y<m_document.m_Datainfo.m_mapOrigin.y)
		return FALSE;
	if(geoPoint.x>m_document.m_Datainfo.m_mapOrigin.x+m_document.m_Datainfo.m_mapCanvas.cx)
		return FALSE;
	if(geoPoint.y>m_document.m_Datainfo.m_mapOrigin.y+m_document.m_Datainfo.m_mapCanvas.cy)
		return FALSE;

	return TRUE;
}

LONG CGrfView::OnFlashGeomtry(UINT WPARAM, LONG LPARAM)
{
	this->KillTimer(TIMER_FLASHGEOM);

	if(WPARAM==3&&LPARAM!=0)
	{
		PLACE* place = (PLACE*)LPARAM;
		m_pFlash = m_document.m_layertree.GetGeomById(place->wLayer, place->dwGeom);
		if(m_pFlash==nullptr&&m_document.m_pGridMapFile!=nullptr&&m_document.m_pGeomMapFile!=nullptr)
		{
			const int wLayerIndex = m_document.m_pGridMapFile->GetLayerIndex(place->wLayer);
			if(wLayerIndex!=-1)
			{
				CGeomMapFile::GEOMHEAD* pGeomHead = m_document.m_pGeomMapFile->GetGeomHeadByID(wLayerIndex, place->dwGeom);
				if(pGeomHead==nullptr)
					return 0;

				if(m_document.m_pGeomMapFile->m_pFlash!=nullptr)
				{
					m_pFlash = nullptr;

					delete m_document.m_pGeomMapFile->m_pFlash;
					m_document.m_pGeomMapFile->m_pFlash = nullptr;
				}

				CGeom* pFlash = pGeomHead==nullptr ? nullptr : m_document.m_pGeomMapFile->GetGeom(pGeomHead);
				if(pFlash!=nullptr)
				{
					m_document.m_pGeomMapFile->m_pFlash = pFlash;
					m_pFlash = m_document.m_pGeomMapFile->m_pFlash;
				}
			}
			else
			{
				WORD layerCount = m_document.m_pGridMapFile->GetLayerCount();
				CGeomMapFile::GEOMHEAD* pGeomHead = m_document.m_pGeomMapFile->GetGeomHeadByID(place->dwGeom, layerCount);
				if(pGeomHead==nullptr)
					return 0;

				if(m_document.m_pGeomMapFile->m_pFlash!=nullptr)
				{
					m_pFlash = nullptr;

					delete m_document.m_pGeomMapFile->m_pFlash;
					m_document.m_pGeomMapFile->m_pFlash = nullptr;
				}

				CGeom* pFlash = pGeomHead==nullptr ? nullptr : m_document.m_pGeomMapFile->GetGeom(pGeomHead);
				if(pFlash!=nullptr)
				{
					m_document.m_pGeomMapFile->m_pFlash = pFlash;
					m_pFlash = m_document.m_pGeomMapFile->m_pFlash;
				}
			}
		}
	}
	else
		m_pFlash = nullptr;

	if(m_pFlash!=nullptr)
	{
		const CRect docRect = m_pFlash->m_Rect;
		const Gdiplus::Rect cliRect = m_viewMonitor.DocToClient <Gdiplus::Rect>(docRect);

		CRect rect;
		GetClientRect(rect);
		if(cliRect.Width!=0&&cliRect.Height!=0&&(cliRect.Width>rect.Width()||cliRect.Height>rect.Height()))
		{
			const CPoint cliPoint = rect.CenterPoint();

			const float xRatio = (float)rect.Width()/cliRect.Width;
			const float yRatio = (float)rect.Height()/cliRect.Height;
			const float fRatio = min(xRatio, yRatio);
			m_viewMonitor.ZoomBy(this, fRatio, cliPoint);//
		}
		CPoint docPoint = m_pFlash->GetGeogroid();
		const Gdiplus::Point cliPoint = m_viewMonitor.DocToClient<Gdiplus::Point>(docPoint);
		if(rect.PtInRect(CPoint(cliPoint.X, cliPoint.Y))==FALSE)
		{
			CPoint center = rect.CenterPoint();
			this->SendMessage(WM_FIXATEVIEW, (UINT)&docPoint, (LONG)&center);
		}

		this->SetTimer(TIMER_FLASHGEOM, 500, nullptr);

		return (DWORD)m_pFlash;
	}
	else
		return 0;
}

void CGrfView::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType)
{
	CChildFrame* pChildFrm = (CChildFrame*)this->GetParentFrame();
	if(pChildFrm!=nullptr&&pChildFrm->m_ruler.m_bVisible==TRUE)
	{
		lpClientRect->left = 10;
		lpClientRect->top = 10;
	}

	CView::CalcWindowRect(lpClientRect, nAdjustType);
}

LONG CGrfView::OnKeyQuery(UINT WPARAM, LONG LPARAM)
{
	CString strKey = *(CString*)WPARAM;
	CListCtrl* listCtrl = (CListCtrl*)LPARAM;
	if(listCtrl==nullptr||strKey.IsEmpty())
		return 0;

	strKey = strKey.Mid(14);
	strKey.MakeUpper();

	WORD wMap = 0;
	CString strMap = m_document.m_Datainfo.m_strName;
	if(m_document.m_pDeck!=nullptr)
	{
		wMap = m_document.m_pDeck->m_wId;
		strMap = m_document.m_pDeck->m_strName;
	}

	AfxGetApp()->BeginWaitCursor();

	int count = 0;
	for(CTree<CLayer>::forwiterator it = m_document.m_layertree.forwbegin(); it!=m_document.m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		if(layer->m_bKeyQuery==false)
			continue;

		POSITION pos = layer->m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CGeom* pGeom = layer->m_geomlist.GetNext(pos);
			if(pGeom->Query(strKey)==TRUE)
			{
				CString string = pGeom->m_strName;
				string.Replace(_T("\r\n"), _T(" "));
				//	string.Replace(_T("\r\n"),nullptr);
				//	string.Replace(_T(" "),nullptr);
				//	string.Replace(_T("¡¡"),nullptr);
				if(string.IsEmpty()==TRUE)
					continue;
				const int nItem = listCtrl->GetItemCount();
				const int index = listCtrl->InsertItem(nItem, nullptr);
				listCtrl->SetItemText(nItem, 0, string);
				listCtrl->SetItemText(nItem, 1, strMap);
				listCtrl->SetItemText(nItem, 2, layer->m_strName);

				PLACE* place = new PLACE();
				place->wMap = wMap;
				place->wLayer = layer->m_wId;
				place->dwGeom = pGeom->m_geoId;

				listCtrl->SetItemData(index, (DWORD)place);
				count++;
			}
		}
	}

	AfxGetApp()->EndWaitCursor();
	return count;
}

void CGrfView::OnToolPrint()
{
	//	CTool::m_oldTool = &zoomTool;
	AfxMessageBox(IDS_SELECTPRINTZOOM);
	m_document.m_pTool = &printTool;
}

void CGrfView::OnToolPan()
{
	//	CTool::m_oldTool = nullptr;
	m_document.m_pTool = &panTool;
}

void CGrfView::OnToolMarquee()
{
	//	CTool::m_oldTool = &zoomTool;
	m_document.m_pTool = &marqueeTool;
}

void CGrfView::OnToolRadius()
{
	//	CTool::m_oldTool = &zoomTool;
	m_document.m_pTool = &radiusTool;
}

void CGrfView::OnToolBound()
{
	//	CTool::m_oldTool = &zoomTool;
	m_document.m_pTool = &boundTool;
}

void CGrfView::OnToolRegion()
{
	//	CTool::m_oldTool = &zoomTool;
	m_document.m_pTool = &regionTool;
}

void CGrfView::OnToolLength()
{
	//	CTool::m_oldTool = &zoomTool;
	m_document.m_pTool = &lengthTool;
}

void CGrfView::OnToolArea()
{
	//	CTool::m_oldTool = &zoomTool;
	m_document.m_pTool = &areaTool;
}

void CGrfView::OnGpsDispatch()
{
	m_document.m_pTool = &dispatchTool;
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

			ASSERT(!pCmdUI->m_bContinueRouting);

			switch(nId)
			{
			case ID_TOOL_PAN:
				pCmdUI->SetCheck(m_document.m_pTool==&panTool);
				break;
			case ID_TOOL_PRINT:
				pCmdUI->SetCheck(m_document.m_pTool==&printTool);
				break;
			case ID_TOOL_ZOOMIN:
				pCmdUI->SetCheck(m_document.m_pTool==&zoominTool);
				break;
			case ID_TOOL_MARQUEE:
				pCmdUI->SetCheck(m_document.m_pTool==&marqueeTool);
				break;
			case ID_TOOL_RADIUS:
				pCmdUI->SetCheck(m_document.m_pTool==&radiusTool);
				break;
			case ID_TOOL_BOUND:
				pCmdUI->SetCheck(m_document.m_pTool==&boundTool);
				break;
			case ID_TOOL_REGION:
				pCmdUI->SetCheck(m_document.m_pTool==&regionTool);
				break;
			case ID_TOOL_LENGTH:
			{
				pCmdUI->SetCheck(m_document.m_pTool==&lengthTool);
				pCmdUI->Enable(m_document.m_Datainfo.m_pProjection==nullptr ? FALSE : TRUE);
				return TRUE;
			}
			break;
			case ID_TOOL_AREA:
			{
				pCmdUI->SetCheck(m_document.m_pTool==&areaTool);
				pCmdUI->Enable(m_document.m_Datainfo.m_pProjection==nullptr ? FALSE : TRUE);
				return TRUE;
			}
			break;
			case ID_TOOL_DISPATCH:
				pCmdUI->SetCheck(m_document.m_pTool==&dispatchTool);
				break;
			default:
				break;
			}

			switch(nId)
			{
			case ID_GPS_REPLAY:
			case ID_GPS_TRACE:
			{
				if(m_document.m_Datainfo.m_pProjection==nullptr)
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);

				return TRUE;
			}
			break;
			case ID_TOOL_ZOOMOUT:
				if(m_viewMonitor.m_levelCurrent <= m_viewMonitor.m_levelMinimum)
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
				return TRUE;
			default:
				break;
			}

			pCmdUI->m_bContinueRouting = false;
		}
	}

	return CView::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}
void CGrfView::ChangeViewLevel(float fLevel)
{
	if(fLevel == m_viewMonitor.m_levelCurrent)
	{
	}
	else if(fLevel <= m_viewMonitor.m_levelMinimum && fLevel >= m_viewMonitor.m_levelMaximum)
	{
		CRect rect;
		this->GetClientRect(rect);
		const CPoint cliPoint = rect.CenterPoint();
		const CPointF mapPoint = m_viewMonitor.ClientToDataMap(cliPoint);

		m_viewMonitor.LevelTo(this, fLevel, mapPoint);
	}
}

void CGrfView::ChangeViewScale(float nScale)
{
	if(nScale==m_viewMonitor.m_scaleCurrent)
	{
	}
	else if(nScale<=m_document.m_Datainfo.m_scaleMinimum&&nScale>=m_document.m_Datainfo.m_scaleMaximum)
	{
		CRect rect;
		this->GetClientRect(rect);
		const CPoint cliPoint = rect.CenterPoint();
		const CPointF mapPoint = m_viewMonitor.ClientToDataMap(cliPoint);

		m_viewMonitor.ScaleTo(this, nScale, mapPoint);
	}
}
void CGrfView::OnLevelSliderPosChanged()
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
	((CMainFrame*)(AfxGetMainWnd()))->m_wndRibbonBar.GetElementsByID(ID_MAP_LEVELSLIDER, ar);

	for(int i = 0; i < ar.GetSize(); i++)
	{
		CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, ar[i]);
		if(pSlider != nullptr)
		{
			const int nPos = pSlider->GetPos();
			const int nMax = pSlider->GetRangeMax();
			const int nMin = pSlider->GetRangeMin();
			const float fScale = (float)nPos/(nMax - nMin);
			this->ChangeViewLevel(fScale);
		}
	}
}


void CGrfView::OnScaleSliderPosChanged()
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
	((CMainFrame*)(AfxGetMainWnd()))->m_wndRibbonBar.GetElementsByID(ID_MAP_SCALESLIDER, ar);

	for(int i = 0; i<ar.GetSize(); i++)
	{
		CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, ar[i]);
		if(pSlider!=nullptr)
		{
			const int nPos = pSlider->GetPos();
			const int nMax = pSlider->GetRangeMax();
			const int nMin = pSlider->GetRangeMin();
			const unsigned fScale = nMax-nPos+nMin;
			this->ChangeViewScale(fScale);
		}
	}
}
void CGrfView::OnScaleGo()
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
	((CMainFrame*)(AfxGetMainWnd()))->m_wndRibbonBar.GetElementsByID(ID_MAP_SCALEGO, ar);

	for(int i = 0; i<ar.GetSize(); i++)
	{
		CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, ar[i]);
		if(pEdit!=nullptr)
		{
			CString str = pEdit->GetEditText();
			const float fScale = _ttof(str);
			this->ChangeViewScale(fScale);
		}
	}
}
void CGrfView::OnLevelGo()
{
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
	((CMainFrame*)(AfxGetMainWnd()))->m_wndRibbonBar.GetElementsByID(ID_MAP_LEVELGO, ar);

	for(int i = 0; i < ar.GetSize(); i++)
	{
		CMFCRibbonEdit* pEdit = DYNAMIC_DOWNCAST(CMFCRibbonEdit, ar[i]);
		if(pEdit != nullptr)
		{
			CString str = pEdit->GetEditText();
			const float fLevel = _ttof(str);
			this->ChangeViewLevel(fLevel);
		}
	}
}

LONG CGrfView::OnGetViewinfo(UINT WPARAM, LONG LPARAM)
{
	return (LONG)&m_viewMonitor;
}
