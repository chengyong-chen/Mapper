#include "stdafx.h"
#include "resource.h"
#include "ChildFrm.h"
#include "GrfView.h"
#include "GrfDoc.h"
#include "Global.h"
#include "../Framework/Ruler.h"
#include "../Framework/GCreateContext.h"

#include "../Viewer/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
	//{{AFX_MSG_MAP(CChildFrame)
	//ON_WM_LBUTTONDOWN()
	//ON_WM_LBUTTONDBLCLK()
	//ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_COMMAND(ID_VIEW_RULER, OnViewRuler)
	ON_COMMAND(ID_MAP_ATTFORM, OnMapAttForm)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RULER, OnUpdateViewRuler)
	ON_UPDATE_COMMAND_UI(ID_MAP_ATTFORM, OnUpdateAttForm)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETRULER, OnSetRuler)
	ON_MESSAGE(WM_ISMAINCHILD, OnIsMainChild)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame() noexcept
	: m_wndSplitter(m_ruler)
{
	m_ruler.m_bVisible = true;
}

CChildFrame::~CChildFrame()
{
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG
int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CMDIChildWndEx::OnCreate(lpCreateStruct)==-1)
		return -1;

	//	if(m_DynamicPane.Create(_T(""), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PANE10, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM | CBRS_SIZE_FIXED))
	//	{
	//		m_DynamicPane.EnableGripper(FALSE);
	//		CMDIChildWndEx::AddPane(&m_DynamicPane);
	//		CMDIChildWndEx::DockPane(&m_DynamicPane, CBRS_BOTTOM);
	//		m_DynamicPane.ShowPane(TRUE, FALSE, FALSE);
	//	}
	return 0;
}
BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT, CCreateContext* pContext)
{
	CRect rect;
	GetClientRect(rect);

	GCreateContext* pGContext = (GCreateContext*)pContext;
	if(pGContext==nullptr)
		return FALSE;
	else if(pGContext->m_pSecondViewClass==nullptr)
	{
		if(CreateView(pContext, AFX_IDW_PANE_FIRST)==nullptr)
			return FALSE;
	}
	else if(pGContext->m_pSecondViewClass!=nullptr&&m_wndSplitter.CreateStatic(this, 1, 2)==FALSE)
	{
		TRACE(_T("Failed to CreateStaticSplitter\n"));
		return FALSE;
	}
	else if(!m_wndSplitter.CreateView(0, 0, pGContext->m_pFirstViewClass, CSize(max(rect.Width()*3/4, 500), 0), pContext))
	{
		TRACE(_T("Failed to create second pane\n"));
		return FALSE;
	}
	else if(!m_wndSplitter.CreateView(0, 1, pGContext->m_pSecondViewClass, CSize(max(rect.Width()/4, 100), 0), pContext))
	{
		TRACE(_T("Failed to create first pane\n"));
		return FALSE;
	}
	else
	{
		for(int col = 1; col<m_wndSplitter.GetColumnCount(); col++)
		{
			m_wndSplitter.SetColumnInfo(col, 300, 0);
		}

		CWnd* pView = m_wndSplitter.GetPane(0, 0);
		if(pView!=nullptr)
		{
			SetActiveView((CView*)pView);
			CWnd* pAidView = m_wndSplitter.GetPane(0, 1);
			((CGrfView*)pView)->SetAidView((CView*)pAidView);
		}

		m_wndSplitter.HideColumn(1);
	}
	return TRUE;
}
CWnd* CChildFrame::CreateView(CCreateContext* pContext, UINT nId)
{
	ASSERT(m_hWnd!=nullptr);
	ASSERT(::IsWindow(m_hWnd));
	ENSURE_ARG(pContext!=nullptr);
	ENSURE_ARG(pContext->m_pNewViewClass!=nullptr);

	// Note: can be a CWnd with PostNcDestroy self cleanup
	CWnd* pView = (CWnd*)((GRuntimeClass*)(pContext->m_pNewViewClass))->CreateObject(*(pContext->m_pCurrentDoc));
	if(pView==nullptr)
	{
		TRACE(traceAppMsg, 0, "Warning: Dynamic create of view type %hs failed.\n", pContext->m_pNewViewClass->m_lpszClassName);
		return nullptr;
	}
	ASSERT_KINDOF(CWnd, pView);

	// views are always created with a border!
	if(!pView->Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, nId, pContext))
	{
		TRACE(traceAppMsg, 0, "Warning: could not create view for frame.\n");
		return nullptr; // can't continue without a view
	}

	if(pView->GetExStyle()&WS_EX_CLIENTEDGE)
	{
		// remove the 3d style from the frame, since the view is
		//  providing it.
		// make sure to recalc the non-client area
		ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
	}
	return pView;
}
BOOL CChildFrame::DestroyWindow()
{
	if(m_DynamicPane.m_hWnd!=nullptr)
	{
		m_DynamicPane.PostMessage(WM_DESTROY, 0, 0);
		m_DynamicPane.DestroyWindow();
	}

	return CMDIChildWndEx::DestroyWindow();
}
void CChildFrame::OnPaint()
{
	CPaintDC dc(this);
	CView* pView = (CView*)GetActiveView();
	if(pView->IsKindOf(RUNTIME_CLASS(CPreviewViewEx)))
	{
	}
	else if(m_ruler.m_bVisible==TRUE)
	{
		const Gdiplus::Point start = ((CGrfView*)pView)->m_viewMonitor.DataMapToClient<Gdiplus::Point>(CPointF(0, 0));
		m_ruler.DrawH((CView*)pView, this, start);
		m_ruler.DrawV((CView*)pView, this, start);
	}
}
void CChildFrame::ResetRuler(CViewMonitor& viewMonitor)
{
	if(m_ruler.m_bVisible==false)
		return;

	CView* pView = (CView*)GetActiveView();
	if(pView==nullptr)
		return;
	if(pView->IsKindOf(RUNTIME_CLASS(CPreviewViewEx)))
		return;

	if(viewMonitor.m_pBackground!=nullptr)
		m_ruler.Reset(pView, viewMonitor.m_mapOrigin, viewMonitor.m_xFactorMapToView);
	else
		m_ruler.Reset(pView, viewMonitor.m_mapOrigin, viewMonitor.m_xFactorMapToView);

	OnPaint();
}
void CChildFrame::AdjustDockingLayout(HDWP hdwp)
{
	ASSERT_VALID(this);

	if(m_dockManager.IsInAdjustLayout())
		return;

	m_dockManager.AdjustDockingLayout(hdwp);
	AdjustClientArea();
}
void CChildFrame::AdjustClientArea()
{
	CWnd* pChildWnd = (m_pTabbedControlBar!=nullptr&&m_pTabbedControlBar->IsMDITabbed()&&m_pTabbedControlBar->GetParent()==this) ? m_pTabbedControlBar : GetDlgItem(AFX_IDW_PANE_FIRST);
	if(pChildWnd!=nullptr)
	{
		if(!pChildWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
			pChildWnd->ModifyStyle(0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
		else
			pChildWnd->ModifyStyle(0, WS_CLIPSIBLINGS);

		if(!CDockingManager::m_bFullScreenMode)
		{
			CRect rectClientAreaBounds = m_dockManager.GetClientAreaBounds();
			if(!pChildWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
			{
				rectClientAreaBounds.left += m_rectBorder.left+(m_ruler.m_bVisible ? 13 : 0);
				rectClientAreaBounds.top += m_rectBorder.top+(m_ruler.m_bVisible ? 13 : 0);
				rectClientAreaBounds.right -= m_rectBorder.right;
				rectClientAreaBounds.bottom -= m_rectBorder.bottom;
			}
			pChildWnd->SetWindowPos(&wndBottom, rectClientAreaBounds.left, rectClientAreaBounds.top, rectClientAreaBounds.Width(), rectClientAreaBounds.Height(), SWP_NOACTIVATE);
		}
	}
}
//void CChildFrame::OnLButtonDown(UINT nFlags, CPoint point) 
//{
//	CGrfView* pView = (CGrfView*)GetActiveView();
//	if(pView != nullptr)
//	{
//		CRuler* ruler = (CRuler*)pView->SendMessage(WM_GETRULER,0,0);
//		if(ruler != nullptr && ruler->m_bVisible == TRUE && a_pTool != nullptr)
//		{
//			CPoint docPoint = pView->m_viewMonitor.ClientToDoc(point);
//			CTool::m_oldTool = a_pTool;
//			a_pTool = &rulerTool;
//			a_pTool->OnLButtonDown(pView,pView->m_viewMonitor,nFlags,docPoint);	
//			a_pTool->SetCursor(this);	
//		}
//	}
//	
//	CMDIChildWndEx::OnLButtonDown(nFlags, point);
//}
//
//void CChildFrame::OnLButtonUp(UINT nFlags, CPoint point) 
//{
//	CGrfView* pView = (CGrfView*)GetActiveView();
//	if(pView!=nullptr)
//	{
//		CRuler* ruler = (CRuler*)pView->SendMessage(WM_GETRULER,0,0);
//		if(ruler->m_bVisible == TRUE && a_pTool != nullptr)
//		{
//			CPoint docPoint = pView->m_viewMonitor.ClientToDoc(point);
//			a_pTool->OnLButtonUp(pView,pView->m_viewMonitor,nFlags,docPoint);
//		}
//	}
//		
//	CMDIChildWndEx::OnLButtonUp(nFlags, point);
//}
//
//void CChildFrame::OnLButtonDblClk(UINT nFlags, CPoint point) 
//{
//	CGrfView* pView = (CGrfView*)GetActiveView();
//	if(pView!=nullptr)
//	{
//		CPoint point(0,0);
//		SendMessage(WM_SETRULER,(UINT)(&point),0);
//	}
//
//	CMDIChildWndEx::OnLButtonDblClk(nFlags, point);
//}
void CChildFrame::OnViewRuler()
{
	CView* pView = (CView*)GetActiveView();
	if(pView!=nullptr)
	{
		m_ruler.ShowSwitch(m_wndSplitter.m_hWnd==nullptr ? this : (CWnd*)&m_wndSplitter, pView);
	}
}
void CChildFrame::OnUpdateViewRuler(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_ruler.m_bVisible);
}
void CChildFrame::OnUpdateAttForm(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndSplitter.GetMaxColumnCount()>1);
	pCmdUI->SetCheck(m_wndSplitter.IsPaneVisible(0, 1));
}
LONG CChildFrame::OnSetRuler(UINT WPARAM, LONG LPARAM)
{
	/*CView* pView = (CView*)GetActiveView();
	if(pView!=nullptr)
	{
		CRuler* ruler = (CRuler*)pView->SendMessage(WM_GETRULER,0,0);
		if(ruler != nullptr)
		{
			CPoint origin = *(CPoint*)WPARAM;
			ruler->m_origin = origin;

			ruler->DrawH(pView,this);
			ruler->DrawV(pView,this);

			return 1L;
		}
	}
*/
	return 0L;
}
LONG CChildFrame::OnIsMainChild(UINT WPARAM, LONG LPARAM)
{
	return TRUE;
}
void CChildFrame::OnMapAttForm()
{
	if(m_wndSplitter.GetMaxColumnCount()>1)
	{
		if(m_wndSplitter.IsPaneVisible(0, 1)==TRUE)
		{
			m_wndSplitter.HideColumn(1);
		}
		else
		{
			m_wndSplitter.SetColumnInfo(1, 300, 0);
			m_wndSplitter.ShowColumn(1);
			CWnd* pView1 = m_wndSplitter.GetPane(0, 0);
			if(pView1!=nullptr)
			{
				SetActiveView((CView*)pView1);
			}
			CWnd* pView2 = m_wndSplitter.GetPane(0, 0);
			if(pView2!=nullptr)
			{
				SetActiveView((CView*)pView2);
			}
		}
	}
}