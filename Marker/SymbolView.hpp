#pragma once

#include "../DataView/ViewMonitor.h"

#include "../Tool/EditGeom.h"

#include "TabletView.hpp"
#include "MainFrm.h"

#include "../Framework/gfx.h"
#include "../Geometry/Geom.h"
#include "../Mapper/Global.h"
#include "../Style/FillCompact.h"
#include "../Style/Library.h"
#include "../Style/Line.h"
#include "../Tool/Global.h"

#include "../Action/Remove.h"
#include "../Layer/Layer.h"
#include "../Action/Document/LayerTree/Layerlist/Layer.h"
#include "../Layer/Global.h"
#include "../Style/StyleDlg.h"

extern UNIT  a_UnitArray[3];
extern BYTE  a_nUnitIndex;
extern float a_fJoinTolerance;
extern float a_fTopoTolerance;
extern float a_fDensity;

extern __declspec(dllimport) BYTE d_nUnitIndex;
class CLayerTree;

template<class T>
class CSymbolView : public CView
{
	DECLARE_GWDYNCREATE_T(CSymbolView, T)
protected:
	CSymbolView(CObject& parameter);
private:
	CSymbolDoc<T>& m_document;

public:
	CMFCStatusBar* GetStatusBar();

public:
	CViewMonitor m_viewMonitor;
	CLayerTree& m_layertree;
	CEditGeom m_editgeom;
	bool m_bSnapPoint;
	long m_docTick;

public:
	void DrawRuler(Gdiplus::Graphics& g);
	BOOL PickAnchor(UINT nFlags, CPoint point);
	BOOL IsTopmost(CGeom* pGeom);
	void Remove(CGeom* pGeom);
	void SnapPoint(long& x, long& y) const;

public:
	virtual void InactivateAll();
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSymbolView)
public:
	void OnDraw(CDC* pDC) override; // overridden to draw this view
	void OnInitialUpdate() override; // called first time after construct
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
protected:
	void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;
	//}}AFX_VIRTUAL

	// Implementation
public:
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CSymbolView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSelectAll();
	afx_msg void OnViewMagnificationActual();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGeomSetstyle();
	afx_msg void OnArrange(UINT dwId);
	afx_msg void OnAlign(UINT nId);
	afx_msg void OnSnapToPont();
	//}}AFX_MSG
	afx_msg LONG OnNewGeomDrew(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnRemovGeomtry(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetActiveRect(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetActiveArea(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnCaptureImage(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnGetGeomInflate(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnDocModified(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnZoomIn(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnZoomOut(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPickAnchor(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPickGeom(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPickByRect(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnTransform(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnTransformed(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnMoveGeomtry(UINT WPARAM, LONG LPARAM);
	DECLARE_MESSAGE_MAP()
};
IMPLEMENT_GWDYNCREATE_T(CSymbolView, T, CView)

BEGIN_TEMPLATE_MESSAGE_MAP(CSymbolView, T, CView)
	//{{AFX_MSG_MAP(CSymbolView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_SELECTALL_ALL, OnSelectAll)
	ON_COMMAND(ID_VIEW_MAGNIFICATION_ACTUAL, OnViewMagnificationActual)
	ON_COMMAND(ID_GEOM_SETSTYLE, OnGeomSetstyle)
	ON_COMMAND_RANGE(ID_ARRANGE_TOFRONT, ID_ARRANGE_BACKWARD, OnArrange)
	ON_COMMAND_RANGE(ID_ALIGN_LEFT, ID_ALIGN_ORIGIN, OnAlign)
	ON_COMMAND(ID_EDIT_SNAPTOPOINT, OnSnapToPont)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)

	ON_MESSAGE(WM_NEWGEOMDREW, OnNewGeomDrew)
	ON_MESSAGE(WM_REMOVEGEOM, OnRemovGeomtry)
	ON_MESSAGE(WM_GETACTIVERECT, OnGetActiveRect)
	ON_MESSAGE(WM_GETACTIVEAREA, OnGetActiveArea)
	ON_MESSAGE(WM_CAPTUREIMAGE, OnCaptureImage)
	ON_MESSAGE(WM_DOCMODIFIED, OnDocModified)
	ON_MESSAGE(WM_ZOOMIN, OnZoomIn)
	ON_MESSAGE(WM_ZOOMOUT, OnZoomOut)
	ON_MESSAGE(WM_TRANSFORM, OnTransform)
	ON_MESSAGE(WM_TRANSFORMED, OnTransformed)
	ON_MESSAGE(WM_MOVEGEOM, OnMoveGeomtry)
	ON_MESSAGE(WM_PICKANCHOR, OnPickAnchor)
	ON_MESSAGE(WM_PICKGEOM, OnPickGeom)
	ON_MESSAGE(WM_PICKBYRECT, OnPickByRect)
	END_MESSAGE_MAP()

template<class T>
CSymbolView<T>::CSymbolView(CObject& parameter)
	:m_editgeom(m_viewMonitor, ((CSymbolDoc<T>&)parameter).m_actionStack), m_document((CSymbolDoc<T>&)parameter), m_viewMonitor(((CSymbolDoc<T>&)parameter).m_Datainfo), m_layertree(((CSymbolDoc<T>&)parameter).m_layertree), m_bSnapPoint(false)
{
	m_editgeom.m_pLayer = &m_layertree.m_root;
	m_editgeom.m_pGeom = NULL;
	m_viewMonitor.m_bViewer = false;
}

template<class T>
CMFCStatusBar* CSymbolView<T>::GetStatusBar()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	return 	pMainFrame!=nullptr ? &(pMainFrame->m_wndStatusBar) : NULL;
}
template<class T>
void CSymbolView<T>::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	m_editgeom.m_pGeom = NULL;

	const CRectF mapRect = m_document.m_Datainfo.GetMapRect();
	m_viewMonitor.ScaleTo(this, m_document.m_Datainfo.m_scaleSource, mapRect.CenterPoint());

	CMainFrame* pAppFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if(pAppFrame!=nullptr)
	{
		pAppFrame->m_wndDrawTool.SwitchSource(&m_document.m_pTool);
	}
}
template<class T>
int CSymbolView<T>::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CView::OnCreate(lpCreateStruct)==-1)
		return -1;

	m_document.m_pSymbolView = this;
	return 0;
}

template<class T>
void CSymbolView<T>::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if(m_hWnd!=NULL)
	{
		m_viewMonitor.OnSized(this, cx, cy);
	}
}

template<class T>
BOOL CSymbolView<T>::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo==nullptr)
	{
		if(nCode==CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			switch(nID)
			{
				case ID_EDIT_SNAPTOPOINT:
					pCmdUI->SetCheck(m_bSnapPoint);
					break;
				default:
					break;
			}
			pCmdUI->m_bContinueRouting = false;
		}
		if(nCode==CN_COMMAND)
		{
		}
	}

	return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

template<class T>
void CSymbolView<T>::OnSnapToPont()
{
	m_bSnapPoint = !m_bSnapPoint;
}
template<class T>
void CSymbolView<T>::SnapPoint(long& x, long& y) const
{
	if(m_docTick == 0)
		return;
	else if(m_bSnapPoint)
	{
		x = (x / m_docTick)*m_docTick;
		y = (y / m_docTick)*m_docTick;
	}
}
template<class T>
void CSymbolView<T>::OnDraw(CDC* pDC)
{
	CRect clipBox;
	pDC->GetClipBox(clipBox);
	clipBox.NormalizeRect();
	if(clipBox.IsRectNull())
		return;

	Gdiplus::Graphics g(pDC->m_hDC);
	g.SetPageUnit(Gdiplus::UnitPixel);
	g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	g.Clear(Gdiplus::Color(255, 255, 255));
	//	g.SetClip(Rect(inRect.left,inRect.top,inRect.Width(),inRect.Height()),CombineModeReplace);//ÕâÑùÊÇ´íµÄ

	CRect inRect = m_viewMonitor.ClientToDoc(clipBox);
	m_viewMonitor.DrawCanvas1(g);
	m_document.Draw(m_viewMonitor, g, inRect);
	this->DrawRuler(g);
	g.ReleaseHDC(pDC->m_hDC);

	CLibrary library;
	const CPoint oldOrg = pDC->SetViewportOrg(0, 0);
	m_document.m_layertree.m_root.DrawAnchors(pDC, m_viewMonitor, inRect);
	pDC->SetViewportOrg(oldOrg);

	if(m_document.IsKindOf(RUNTIME_CLASS_T(CSymbolDoc, CSymbolLine))==TRUE)
	{
		const CListCtrl& pList = ((CTabletView<T>*)m_document.m_pTabletView)->GetListCtrl();
		const long nItem = pList.GetNextItem(-1, LVNI_SELECTED);
		if(nItem>=0)
		{
			Gdiplus::Pen* pen = ::new Gdiplus::Pen(Gdiplus::Color(100, 100, 100), 1);
			pen->SetAlignment(Gdiplus::PenAlignmentCenter);
			pen->SetDashStyle(Gdiplus::DashStyleSolid);

			const CSymbolLine* symbol = (CSymbolLine*)pList.GetItemData(nItem);
			const Gdiplus::Point point1 = m_viewMonitor.DocToClient<Gdiplus::Point>(CPoint(symbol->m_nStep*1000, -100*10000));
			const Gdiplus::Point point2 = m_viewMonitor.DocToClient<Gdiplus::Point>(CPoint(symbol->m_nStep*1000, 100*10000));

			g.DrawLine(pen, point1, point2);

			::delete pen;
			pen = nullptr;
		}
	}

	m_editgeom.SwitchAnchorsFocused(pDC);
}
/////////////////////////////////////////////////////////////////////////////
// CSymbolView message handlers
template<class T>
void CSymbolView<T>::OnLButtonDown(UINT nFlags, CPoint point)
{
	CView::OnLButtonDown(nFlags, point);

	if(m_document.m_pTool!=nullptr)
	{
		CPoint docPoint = m_viewMonitor.ClientToDoc(point, false);
		m_document.m_pTool->OnLButtonDown(this, m_viewMonitor, nFlags, point, docPoint);
	}
}
template<class T>
void CSymbolView<T>::OnLButtonUp(UINT nFlags, CPoint point)
{
	CView::OnLButtonUp(nFlags, point);

	if(m_document.m_pTool!=nullptr)
	{
		CPoint docPoint = m_viewMonitor.ClientToDoc(point, false);
		SnapPoint(docPoint.x, docPoint.y);
		m_document.m_pTool->OnLButtonUp(this, m_viewMonitor, nFlags, point, docPoint);
	}
}
template<class T>
void CSymbolView<T>::OnMouseMove(UINT nFlags, CPoint point)
{
	CView::OnMouseMove(nFlags, point);

	CPoint docPoint = m_viewMonitor.ClientToDoc(point, false);

	CMFCStatusBar* pStatusBar = this->GetStatusBar();
	if(pStatusBar!=nullptr)
	{
		CString	str;
		str.Format(_T("X: %g Y: %g"), docPoint.x/10000.f, docPoint.y/10000.f);
		pStatusBar->SetPaneText(3, str);
	}

	if(m_document.m_pTool!=nullptr)
	{
		SnapPoint(docPoint.x, docPoint.y);
		m_document.m_pTool->OnMouseMove(this, m_viewMonitor, nFlags, point, docPoint);
	}
}

#ifdef _DEBUG
template<class T>
void CSymbolView<T>::AssertValid() const
{
	CView::AssertValid();
}
template<class T>
void CSymbolView<T>::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif

template<class T>
BOOL CSymbolView<T>::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if(nHitTest==HTCLIENT)
		return m_document.m_pTool->SetCursor(this, m_viewMonitor);
	else
		return CView::OnSetCursor(pWnd, nHitTest, message);
}
template<class T>
void CSymbolView<T>::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CClientDC dc(this);

	switch(nChar)
	{
		case VK_TAB:
			{
				InactivateAll();
			}
			break;
		case VK_RIGHT:
			{
				CSize Δ = m_viewMonitor.ClientToDoc(Gdiplus::SizeF(1, 0));
				if(m_editgeom.m_pGeom!=nullptr&&m_editgeom.m_Anchors.size()!=0)
				{
					m_editgeom.MoveAnchor(this, Δ, FALSE);
					m_editgeom.ChangeAnchor(this, &dc, Δ, FALSE);
				}
				else
					SendMessage(WM_MOVEGEOM, 0, (LONG)&Δ);
			}
			break;
		case VK_LEFT:
			{
				CSize Δ = -m_viewMonitor.ClientToDoc(Gdiplus::SizeF(1, 0));
				if(m_editgeom.m_pGeom!=nullptr&&m_editgeom.m_Anchors.size()!=0)
				{
					m_editgeom.MoveAnchor(this, Δ, FALSE);
					m_editgeom.ChangeAnchor(this, &dc, Δ, FALSE);
				}
				else
					SendMessage(WM_MOVEGEOM, 0, (LONG)&Δ);
			}
			break;
		case VK_UP:
			{
				CSize Δ = m_viewMonitor.ClientToDoc(Gdiplus::SizeF(0, 1));
				if(m_editgeom.m_pGeom!=nullptr&&m_editgeom.m_Anchors.size()!=0)
				{
					m_editgeom.MoveAnchor(this, Δ, FALSE);
					m_editgeom.ChangeAnchor(this, &dc, Δ, FALSE);
				}
				else
					SendMessage(WM_MOVEGEOM, 0, (LONG)&Δ);
			}
			break;
		case VK_DOWN:
			{
				CSize Δ = -m_viewMonitor.ClientToDoc(Gdiplus::SizeF(0, 1));
				if(m_editgeom.m_pGeom!=nullptr&&m_editgeom.m_Anchors.size()!=0)
				{
					m_editgeom.MoveAnchor(this, Δ, FALSE);
					m_editgeom.ChangeAnchor(this, &dc, Δ, FALSE);
				}
				else
					SendMessage(WM_MOVEGEOM, 0, (LONG)&Δ);
			}
			break;
		default:
			if(m_document.m_pTool!=nullptr)
			{
				m_document.m_pTool->OnKeyDown(this, m_viewMonitor, nChar, nRepCnt, nFlags, m_document.m_pTool, m_document.m_oldTool);
			}
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
template<class T>
void CSymbolView<T>::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(m_document.m_pTool!=nullptr)
	{
		m_document.m_pTool->OnKeyUp(this, m_viewMonitor, nChar, nRepCnt, nFlags, m_document.m_pTool, m_document.m_oldTool);
	}

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}
template<class T>
void CSymbolView<T>::OnViewMagnificationActual()
{
	this->OnInitialUpdate();
}

template<class T>
void CSymbolView<T>::OnArrange(UINT dwId)
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
	m_layertree.m_root.Arrange(this, m_viewMonitor, m_document.m_actionStack, arrange);

	SendMessage(WM_DOCMODIFIED, 0, TRUE);
}

template<class T>
void CSymbolView<T>::OnAlign(UINT nId)
{
	ALIGN align = ALIGN(nId-ID_ALIGN_LEFT);
	CRect* rect = (CRect*)(SendMessage(WM_GETACTIVERECT, 0, 0));

	m_document.m_actionStack.Start();
	m_layertree.m_root.Align(this, m_viewMonitor, m_document.m_actionStack, rect, align);
	m_document.m_actionStack.Stop();

	SendMessage(WM_DOCMODIFIED, 0, TRUE);
}
template<class T>
void CSymbolView<T>::OnSelectAll()
{
	CRect cliRect;
	GetClientRect(cliRect);
	CRect docRect = m_viewMonitor.ClientToDoc(cliRect);
	if(docRect.IsRectNull())
		return;

	m_layertree.m_root.ActivateAll(this, ACTIVEALL::All, m_document.m_Datainfo, m_viewMonitor, docRect);
}
template<class T>
void CSymbolView<T>::InactivateAll()
{
	CRect cliRect;
	GetClientRect(cliRect);
	CRect docRect = m_viewMonitor.ClientToDoc(cliRect);
	if(docRect.IsRectNull())
		return;

	m_editgeom.UnFocusAll(this);
	m_layertree.m_root.InactivateAll(this, m_document.m_Datainfo, m_viewMonitor, docRect);
}
template<class T>
LONG CSymbolView<T>::OnTransform(UINT WPARAM, LONG LPARAM)
{
	const double* a = (double*)WPARAM;
	double m11, m21, m31, m12, m22, m32;
	m11 = a[0], m21 = a[1], m31 = a[2], m12 = a[3], m22 = a[4], m32 = a[5];
	Gdiplus::Matrix matrixTransform(m11, m12, m21, m22, m31, m32);

	CClientDC dc(this);
	m_layertree.m_root.Transform(&dc, m_viewMonitor, m11, m21, m31, m12, m22, m32);

	return 0L;
}
template<class T>
LONG CSymbolView<T>::OnTransformed(UINT WPARAM, LONG LPARAM)
{
	const double* a = (double*)WPARAM;
	double m11, m21, m31, m12, m22, m32;
	m11 = a[0], m21 = a[1], m31 = a[2], m12 = a[3], m22 = a[4], m32 = a[5];

	m_layertree.m_root.Transform(this, m_viewMonitor, m_document.m_actionStack, m11, m21, m31, m12, m22, m32);

	SendMessage(WM_DOCMODIFIED, 0, TRUE);
	return 0L;
}
template<class T>
LONG CSymbolView<T>::OnPickGeom(UINT WPARAM, LONG LPARAM)
{
	CPoint point = *(CPoint*)LPARAM;

	CClientDC dc(this);

	CLibrary library;
	CGeom* geom = m_layertree.m_root.Pick(this, m_viewMonitor, point, true);
	if(geom!=NULL)
	{
		if(geom->m_bActive==true)
			return false;

		if(::GetKeyState(VK_SHIFT)>=0)
			InactivateAll();
		geom->m_bActive = true;
		geom->DrawAnchors(&dc, m_viewMonitor);
		m_layertree.m_root.m_nActiveCount++;
		return (LONG)geom;
	}

	if(::GetKeyState(VK_SHIFT)>=0)
		InactivateAll();
	return FALSE;
}
template<class T>
LONG CSymbolView<T>::OnPickByRect(UINT WPARAM, LONG LPARAM)
{
	CRect rect = *(CRect*)LPARAM;

	m_layertree.m_root.Pick(this, m_viewMonitor, rect, true);
	return 0L;
}

template<class T>
LONG CSymbolView<T>::OnNewGeomDrew(UINT WPARAM, LONG LPARAM)
{
	InactivateAll();

	CGeom* pGeom = (CGeom*)WPARAM;
	pGeom->m_pLine = new CLine;
	pGeom->m_pFill = nullptr;

	CLibrary library;
	m_layertree.m_root.NewGeom(this, m_document.m_Datainfo, pGeom, m_document.m_actionStack);
	m_editgeom.m_pLayer = &m_layertree.m_root;
	m_layertree.m_root.Invalidate(this, m_viewMonitor, pGeom);

	SendMessage(WM_DOCMODIFIED, 0, TRUE);
	return TRUE;
}
template<class T>
LONG CSymbolView<T>::OnRemovGeomtry(UINT WPARAM, LONG LPARAM)
{
	CGeom* pGeom = (CGeom*)LPARAM;
	POSITION pos = m_layertree.m_root.m_geomlist.Find(pGeom);
	if(pos!=nullptr)
	{
		int index = GetIndexFromOBList(m_layertree.m_root.m_geomlist, pGeom);
		std::list<std::pair<CGeom*, unsigned int>> geom;
		geom.push_back(std::pair<CGeom*, unsigned int>(pGeom, index));
		Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pRemove = new Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(this, &m_layertree.m_root, geom);
		pRemove->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
		pRemove->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
		pRemove->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_AppendRemove_Geoms;
		m_document.m_actionStack.Record(pRemove);

		m_layertree.m_root.Invalidate(m_document, pGeom);
		this->SendMessage(WM_PREREMOVEGEOM, (LONG)&m_layertree.m_root, (UINT)pGeom);
		m_layertree.m_root.m_geomlist.RemoveAt(pos);
		return TRUE;
	}
	else
		return FALSE;
}

template<class T>
LONG CSymbolView<T>::OnZoomIn(UINT WPARAM, LONG LPARAM)
{
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
		GetClientRect(&clientRect);
		const CPoint center = clientRect.CenterPoint();

		float xRrato = (float)clientRect.Width()/(float)cliRect.Width;
		float yRatio = (float)clientRect.Height()/(float)cliRect.Height;
		float fRatio = min(xRrato, yRatio);

		m_viewMonitor.MoveBy(this, cliPoint.x-center.x, cliPoint.y-center.y);
		m_viewMonitor.ZoomBy(this, fRatio, center);
	}
	return 0L;
}
template<class T>
LONG CSymbolView<T>::OnZoomOut(UINT WPARAM, LONG LPARAM)
{
	const CRect docRect = *(CRect*)WPARAM;
	const Gdiplus::Rect cliRect = m_viewMonitor.DocToClient <Gdiplus::Rect>(docRect);
	const CPoint center = CPoint(cliRect.X+cliRect.Width/2, cliRect.Y+cliRect.Height/2);

	if(cliRect.Width<5||cliRect.Height<5)
	{
		m_viewMonitor.ZoomBy(this, 0.5, center);
	}
	else
	{
		CRect clientRect;
		GetClientRect(&clientRect);

		const float ratioH = (float)cliRect.Width/clientRect.Width();
		const float ratioV = (float)cliRect.Height/clientRect.Height();
		const float fRatio = max(ratioH, ratioV);

		m_viewMonitor.ZoomBy(this, fRatio, center);
	}
	return 0L;
}
template<class T>
LONG CSymbolView<T>::OnPickAnchor(UINT WPARAM, LONG LPARAM)
{
	UINT   nFlags = WPARAM;
	CPoint& docPoint = *((CPoint*)LPARAM);
	const Gdiplus::Point cliPoint = m_viewMonitor.DocToClient<Gdiplus::Point>(docPoint);

	if((nFlags&MK_SHIFT)==0)
		m_editgeom.UnFocusAll(this);

	CTypedPtrList<CObList, CGeom*>& geomlist = m_layertree.m_root.GetGeomList();
	POSITION pos1 = geomlist.GetTailPosition();
	while(pos1!=NULL)
	{
		CGeom* pGeom = geomlist.GetPrev(pos1);
		if(pGeom->m_bActive==false)
			continue;

		const long nAnchor = pGeom->PickAnchor(m_document.m_Datainfo, m_viewMonitor, cliPoint);
		if(nAnchor!=0)
		{
			if(m_editgeom.m_pGeom!=pGeom)
			{
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

			CMFCStatusBar* pStatusBar = this->GetStatusBar();
			if(pStatusBar!=nullptr)
			{
				const CPoint& coor = pGeom->GetAnchor(nAnchor);
				CString	text(_T(""));
				text.Format(_T("X:%g Y:%g"), coor.x/10000.f, coor.y/10000.f);
				pStatusBar->SetPaneText(2, text);
			}
			docPoint = pGeom->GetAnchor(nAnchor);
			return TRUE;
		}
	}

	return FALSE;
}
template<class T>
LONG CSymbolView<T>::OnMoveGeomtry(UINT WPARAM, LONG LPARAM)
{
	CSize Δ = *(CSize*)LPARAM;
	m_layertree.m_root.Move(this, m_viewMonitor, m_document.m_actionStack, Δ);

	SendMessage(WM_DOCMODIFIED, 0, TRUE);
	return m_layertree.m_root.m_nActiveCount>0 ? 1L : 0L;
}
template<class T>
LONG CSymbolView<T>::OnGetActiveRect(UINT WPARAM, LONG LPARAM)
{
	static CRect rect;
	rect.SetRectEmpty();

	rect = m_layertree.m_root.GetActiveRect();
	return (LONG)(&rect);
}
template<class T>
LONG CSymbolView<T>::OnGetActiveArea(UINT WPARAM, LONG LPARAM)
{
	static Gdiplus::Rect rect;
	rect.X = rect.Y = rect.Width = rect.Height = 0;

	rect = m_layertree.m_root.GetActiveArea(m_viewMonitor);

	return (LONG)(&rect);
}
template<class T>
LONG CSymbolView<T>::OnCaptureImage(UINT WPARAM, LONG LPARAM)
{
	const Gdiplus::Rect clipBox = *(Gdiplus::Rect*)WPARAM;
	if(clipBox.IsEmptyArea()==TRUE)
		return NULL;

	const CClientDC dc(this);
	m_viewMonitor.m_sizeDPI = CSize(dc.GetDeviceCaps(LOGPIXELSX), dc.GetDeviceCaps(LOGPIXELSY));
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
		return (LONG)NULL;
}
template<class T>
LONG CSymbolView<T>::OnGetGeomInflate(UINT WPARAM, LONG LPARAM)
{
	CGeom* pGeom = (CGeom*)LPARAM;
	if(pGeom==nullptr)
		return 0;
	else if(pGeom->m_pLine==nullptr)
		return 3;
	else
	{
		const double scale = m_viewMonitor.CurrentRatio();
		const float points = pGeom->m_pLine->m_nWidth*scale/20.0;

		return CViewinfo::PointsToPixels(1.5*points, m_viewMonitor.m_sizeDPI)+3;
	}
}
template<class T>
void CSymbolView<T>::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);

	if(bActivate&&pActivateView==this)
	{
		CTool::SetEditGeom(&m_editgeom);
		if((GetAsyncKeyState(VK_LBUTTON)&0X8000)==0X8000)// VK_LBUTTON is down
		{
			this->SetCapture();
		}
	}

	if(bActivate==FALSE&&pDeactiveView==this)
	{
	}
}
template<class T>
LONG CSymbolView<T>::OnDocModified(UINT WPARAM, LONG LPARAM)
{
	return 0;
	BOOL bModified = (BOOL)LPARAM;

	m_document.SetModifiedFlag(bModified);
	return 0L;
}
template<class T>
void CSymbolView<T>::DrawRuler(Gdiplus::Graphics& g)
{
	const Gdiplus::Point center = m_viewMonitor.DocToClient<Gdiplus::Point>(CPoint(0, 0));
	g.TranslateTransform(center.X, center.Y);

	const Gdiplus::Pen pen(Gdiplus::Color(192, 192, 192), 0.1);

	g.DrawLine(&pen, 0, -10000, 0, 10000);
	g.DrawLine(&pen, -10000, 0, 10000, 0);

	const CString strFont(_T("Arial"));
	const _bstr_t btrFont(strFont);
	const Gdiplus::FontFamily fontFamily(btrFont);
	::SysFreeString(btrFont);

	Gdiplus::Font font(&fontFamily, 8, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

	const float pixels = m_viewMonitor.m_xFactorMapToView*(10*a_UnitArray[a_nUnitIndex].pointpro);

	int exponent = 0;
	while(pow(10, exponent)*pixels<50)
		exponent++;
	while(pow(10, exponent)*pixels>100)
		exponent--;
	if(exponent*pixels<50)
		exponent++;

	const float cx = pixels*pow(10, exponent);
	const float cy = pixels*pow(10, exponent);

	long xNum = 0;
	for(long xStep = 0; xStep<10000; xStep += cx)
	{
		TCHAR string[100];
		_stprintf(string, _T("%.0f"), (xNum++)*pow(10, exponent)*10);

		g.DrawLine(&pen, xStep, 0, xStep, 6*1.5);
		//		g.DrawString(CT2W(string),-1,&font,stringFormat,testBrush);
		//		pDC->TextOut(xStep+0, l*2,);	

		g.DrawLine(&pen, -xStep, 0, -xStep, 6*1.5);
		//		pDC->TextOut(-xStep-0, l*2,  string,lstrlen(string));	
		//		g.DrawString(CT2W(string),-1,&font,stringFormat,testBrush);

		for(long i = 1; i<10; i++)
		{
			const float Δ = cx*i/10.f;
			if(i==5)
			{
				g.DrawLine(&pen, xStep+Δ+0.5, 0, xStep+Δ+0.5, 6);
				g.DrawLine(&pen, -xStep-Δ-0.5, 0, -xStep-Δ-0.5, 6);
			}
			else
			{
				g.DrawLine(&pen, xStep+Δ+0.5, 0, xStep+Δ+0.5, 3);
				g.DrawLine(&pen, -xStep-Δ-0.5, 0, -xStep-Δ-0.5, 3);
			}
		}
	}

	long yNum = 0;
	for(long yStep = 0; yStep<10000; yStep += cy)
	{
		TCHAR string[100];
		_stprintf(string, _T("%.0f"), (yNum++)*pow(10, exponent)*10);

		g.DrawLine(&pen, 0, yStep+0.5, 6*1.5, yStep);
		//		pDC->TextOut(l*2,   yStep+0,string,lstrlen(string));	


		g.DrawLine(&pen, 0, -yStep, 6*1.5, -yStep);
		//		pDC->TextOut(l*2,  -yStep-0,string,lstrlen(string));	

		for(long i = 1; i<10; i++)
		{
			const long Δ = (cy*i)/10.f;

			if(i==5)
			{
				g.DrawLine(&pen, 0, yStep+Δ+0.5, 6, yStep+Δ+0.5);
				g.DrawLine(&pen, 0, -yStep-Δ-0.5, 6, -yStep-Δ-0.5);
			}
			else
			{
				g.DrawLine(&pen, 0, yStep+Δ+0.5, 3, yStep+Δ+0.5);
				g.DrawLine(&pen, 0, -yStep-Δ-0.5, 3, -yStep-Δ-0.5);
			}
		}
	}

	g.TranslateTransform(-center.X, -center.Y);

	m_docTick = pow(10, exponent)*a_UnitArray[a_nUnitIndex].pointpro*10000;
}
/////////////////////////////////////////////////////////////////////////////
// CSymbolView message handlers


template<class T>
void CSymbolView<T>::OnGeomSetstyle()
{
	CTypedPtrList<CObList, CGeom*>& geomlist = m_layertree.m_root.GetGeomList();
	POSITION oPos = geomlist.GetHeadPosition();
	while(oPos!=nullptr)
	{
		CGeom* pGeom = geomlist.GetNext(oPos);
		if(pGeom->m_bActive==false)
			continue;

		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Style.dll"));
		AfxSetResourceHandle(hInst);
		CLibrary library;
		CStyleDlg dlg(this, false, library, (CLine*)(pGeom->m_pLine), (CFill*)(pGeom->m_pFill));
		if(dlg.DoModal()==IDOK)
		{
			m_layertree.m_root.Invalidate(this, m_viewMonitor, pGeom);

			delete pGeom->m_pLine;
			pGeom->m_pLine = dlg.d_pLine;
			dlg.d_pLine = nullptr;

			delete pGeom->m_pFill;
			pGeom->m_pFill = (CFillCompact*)dlg.d_pFill;
			dlg.d_pFill = nullptr;

			m_layertree.m_root.Invalidate(this, m_viewMonitor, pGeom);
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}

	m_document.SetModifiedFlag(TRUE);
}