#pragma once

#include <afxtabview.h>
#include "gfx.h"
#include "GSplitterWnd.h"

template<class T>
class CAidView final : public CTabView
{
	DECLARE_GWDYNCREATE_T(CAidView, T)

protected:
	CAidView(CObject& mainview); // protected constructor used by dynamic creation
	~CAidView() override;

public:
	T& m_mainview;

public:
	void OnInitialUpdate() override; // called first time after construct
	virtual int AddTab(CRuntimeClass* pViewClass, const CString& strViewLabel, int iIndex = -1, CCreateContext* pContext = nullptr);
	virtual int AddTab(GRuntimeClass* pViewClass, const CString& strViewLabel, int iIndex = -1, CCreateContext* pContext = nullptr);
public:
	virtual CWnd* GetTab(CRuntimeClass* pViewClass);
	virtual CWnd* GetTab(GRuntimeClass* pViewClass);

public:
#ifdef _DEBUG
	void AssertValid() const override;
#ifndef _WIN32_WCE
	void Dump(CDumpContext& dc) const override;
#endif
#endif

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);
	afx_msg LRESULT OnTabDestroy(WPARAM wp, LPARAM);
	DECLARE_MESSAGE_MAP()
};

//MPLEMENT_GWDYNCREATE_T(CAidView,T,CTabView)         //this doesn't work
template<typename T>
CObject* PASCAL CAidView<T>::CreateObject(CObject& parameter)
{
	return new CAidView<T>(parameter);
}

IMPLEMENT_GWRUNTIMECLASS_T(CAidView, T, CTabView, 0xFFFF, CAidView<T>::CreateObject, NULL)

template<class T>
CAidView<T>::CAidView(CObject& mainview)
	: m_mainview((T&)mainview)
{
}

template<class T>
CAidView<T>::~CAidView()
{
};

BEGIN_TEMPLATE_MESSAGE_MAP(CAidView, T, CTabView)
	ON_WM_CREATE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_ERASEBKGND()
	ON_REGISTERED_MESSAGE(AFX_WM_CHANGE_ACTIVE_TAB, OnChangeActiveTab)
	ON_MESSAGE(WM_MDIDESTROY, OnTabDestroy)
	END_MESSAGE_MAP()

#ifdef _DEBUG
template<class T>
void CAidView<T>::AssertValid() const
{
	CTabView::AssertValid();
};

#ifndef _WIN32_WCE
template<class T>
void CAidView<T>::Dump(CDumpContext& dc) const
{
	CTabView::Dump(dc);
}
#endif
#endif //_DEBUG

template<class T>
int CAidView<T>::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CView::OnCreate(lpCreateStruct)==-1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	if(!m_wndTabs.Create(CMFCTabCtrl::Style::STYLE_3D_ONENOTE, rectDummy, this, 1, CMFCTabCtrl::LOCATION_TOP, FALSE))
	{
		TRACE0("Failed to create tab window\n");
		return -1; // fail to create
	}
	else
	{
		m_wndTabs.ModifyTabStyle(CMFCTabCtrl::Style::STYLE_3D_ONENOTE);
		m_wndTabs.SetLocation(CMFCTabCtrl::LOCATION_TOP);
		//		m_wndTabs.m_bCloseBtn = TRUE;
		m_wndTabs.EnableActiveTabCloseButton(TRUE);
		m_wndTabs.EnableTabDocumentsMenu(TRUE);
		m_wndTabs.EnableAutoColor(TRUE);
		m_wndTabs.EnableTabSwap(TRUE);
		m_wndTabs.SetTabBorderSize(1);
		m_wndTabs.EnableCustomToolTips(TRUE);

		m_wndTabs.HideInactiveWindow(FALSE);
		m_wndTabs.HideNoTabs();
		m_wndTabs.AutoSizeWindow(TRUE);
		m_wndTabs.AutoDestroyWindow(FALSE);
		m_wndTabs.SetFlatFrame(TRUE);
		//		m_wndTabs.m_bTransparent = TRUE;
		//		m_wndTabs.m_bTopEdge = TRUE;
		m_wndTabs.SetDrawNoPrefix(TRUE, FALSE);
		m_wndTabs.SetActiveTabBoldFont();
		m_wndTabs.m_bActivateLastVisibleTab = TRUE;
		m_wndTabs.m_bActivateTabOnRightClick = TRUE;

		//		m_wndTabs.m_bIsMDITab = TRUE;
		//m_wndTabs.SetFlatFrame ();
		//m_wndTabs.SetTabBorderSize (0);
		//m_wndTabs.AutoDestroyWindow (TRUE);
		return 0;
	}
}

template<class T>
void CAidView<T>::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	m_bIsReady = TRUE;
	OnChangeActiveTab(m_nFirstActiveTab, 0);
}

template<class T>
BOOL CAidView<T>::OnEraseBkgnd(CDC* pDC)
{
	//CRect rect;
	//this->GetClientRect(rect);
	//pDC->FillSolidRect(rect,RGB(255,255,254));	
	return TRUE;
}

template<class T>
int CAidView<T>::AddTab(CRuntimeClass* pWndClass, const CString& strTitle, int iIndex, CCreateContext* pContext)
{
	ASSERT_VALID(this);
	ENSURE(pWndClass!=NULL);
	ENSURE(pWndClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));

	CFrameWndEx* pWnd = DYNAMIC_DOWNCAST(CFrameWndEx, pWndClass->CreateObject());
	ASSERT_VALID(pWnd);
	if(!pWnd->Create(NULL, _T(""), WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0), &m_wndTabs, NULL, NULL, pContext))
	{
		TRACE1("CTabView:Failed to create view '%s'\n", pWndClass->m_lpszClassName);
		return -1;
	}

	//CDocument* pDoc = GetDocument();
	//if (pDoc != nullptr)
	//{
	//	ASSERT_VALID(pDoc);

	//	BOOL bFound = FALSE;
	//	for (POSITION pos = pDoc->GetFirstViewPosition(); !bFound && pos != nullptr;)
	//	{
	//		if (pDoc->GetNextView(pos) == pView)
	//		{
	//			bFound = TRUE;
	//		}
	//	}

	//	if (!bFound)
	//	{
	//		pDoc->AddView(pView);
	//	}
	//}

	m_wndTabs.InsertTab(pWnd, strTitle, iIndex);
	const int nTabs = m_wndTabs.GetTabsNum();
	this->SetActiveView(nTabs-1);
	if(CTabView::GetTabControl().GetTabsNum()>0)
	{
		GSplitterWnd* pSplitterWnd = (GSplitterWnd*)this->GetParent();
		pSplitterWnd->ShowColumn(1);
	}

	return nTabs-1;
}

template<class T>
int CAidView<T>::AddTab(GRuntimeClass* pWndClass, const CString& strTitle, int iIndex, CCreateContext* pContext)
{
	ASSERT_VALID(this);
	ENSURE(pWndClass!=NULL);
	ENSURE(pWndClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));

	CFrameWndEx* pWnd = DYNAMIC_DOWNCAST(CFrameWndEx, pWndClass->CreateObject(m_mainview));
	ASSERT_VALID(pWnd);
	if(!pWnd->Create(NULL, _T(""), WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0), &m_wndTabs, NULL, NULL, pContext))
	{
		TRACE1("CTabView:Failed to create view '%s'\n", pWndClass->m_lpszClassName);
		return -1;
	}

	//CDocument* pDoc = GetDocument();
	//if (pDoc != nullptr)
	//{
	//	ASSERT_VALID(pDoc);

	//	BOOL bFound = FALSE;
	//	for (POSITION pos = pDoc->GetFirstViewPosition(); !bFound && pos != nullptr;)
	//	{
	//		if (pDoc->GetNextView(pos) == pView)
	//		{
	//			bFound = TRUE;
	//		}
	//	}

	//	if (!bFound)
	//	{
	//		pDoc->AddView(pView);
	//	}
	//}

	m_wndTabs.InsertTab(pWnd, strTitle, iIndex);
	const int nTabs = m_wndTabs.GetTabsNum();
	this->SetActiveView(nTabs-1);
	if(CTabView::GetTabControl().GetTabsNum()>0)
	{
		GSplitterWnd* pSplitterWnd = (GSplitterWnd*)this->GetParent();
		pSplitterWnd->ShowColumn(1);
	}
	return nTabs-1;
}

template<class T>
CWnd* CAidView<T>::GetTab(CRuntimeClass* pViewClass)
{
	for(int index = 0; index<CTabView::GetTabControl().GetTabsNum(); index++)
	{
		CWnd* pWnd = CTabView::GetTabControl().GetTabWnd(index);
		if(pWnd==NULL)
			continue;
		if(pWnd->IsKindOf(pViewClass)==TRUE)
			return pWnd;
	}
	return NULL;
}

template<class T>
CWnd* CAidView<T>::GetTab(GRuntimeClass* pViewClass)
{
	for(int index = 0; index<CTabView::GetTabControl().GetTabsNum(); index++)
	{
		CWnd* pWnd = CTabView::GetTabControl().GetTabWnd(index);
		if(pWnd==NULL)
			continue;
		if(pWnd->IsKindOf(pViewClass)==TRUE)
			return pWnd;
	}
	return NULL;
}

template<class T>
int CAidView<T>::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	//CView* pCurrView = GetActiveView();
	//if (pCurrView == nullptr)
	//{
	//	return CView::OnMouseActivate(pDesktopWnd, nHitTest, message);
	//}

	//int nResult = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
	//if (nResult == MA_NOACTIVATE || nResult == MA_NOACTIVATEANDEAT)
	//	return nResult;   // frame does not want to activate

	//CFrameWnd* pParentFrame = AFXGetParentFrame(this);
	//if (pParentFrame != nullptr)
	//{
	//	// eat it if this will cause activation
	//	ASSERT(pParentFrame == pDesktopWnd || pDesktopWnd->IsChild(pParentFrame));

	//	// either re-activate the current view, or set this view to be active
	//	CView* pView = pParentFrame->GetActiveView();
	//	HWND hWndFocus = ::GetFocus();
	//	if (pView == pCurrView && pCurrView->m_hWnd != hWndFocus && !::IsChild(pCurrView->m_hWnd, hWndFocus))
	//	{
	//		// re-activate this view
	//		((CInternalTabView*)pCurrView)->OnActivateView(TRUE, pCurrView, pCurrView);
	//	}
	//	else
	//	{
	//		// activate this view
	//		pParentFrame->SetActiveView(pCurrView);
	//	}
	//}

	//return nResult;

	return 0;
}

template<class T>
LRESULT CAidView<T>::OnChangeActiveTab(WPARAM wp, LPARAM)
{
	if(!m_bIsReady)
	{
		m_nFirstActiveTab = (int)wp;
	}

	return 0;
}

template<class T>
LRESULT CAidView<T>::OnTabDestroy(WPARAM wParam, LPARAM)
{
	const int iTab = CTabView::GetTabControl().GetTabFromHwnd((HWND)wParam);
	CTabView::GetTabControl().RemoveTab(iTab, TRUE);
	if(CTabView::GetTabControl().GetTabsNum()==0)
	{
		GSplitterWnd* pSplitterWnd = (GSplitterWnd*)this->GetParent();
		pSplitterWnd->HideColumn(1);
	}
	return 0;
}
