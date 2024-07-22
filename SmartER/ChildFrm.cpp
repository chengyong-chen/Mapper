
// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "SmartER.h"

#include "ChildFrm.h"

#include "..\Framework\GCreateContext.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
	//{{AFX_MSG_MAP(CChildFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CChildFrame construction/destruction

CChildFrame::CChildFrame()
	:m_wndSplitter(m_ruler)
{
	// TODO: add member initialization code here
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

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

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT, CCreateContext* pContext)
{
	GCreateContext* pGContext = (GCreateContext*)pContext;
	if(pGContext == nullptr)
		return FALSE;
	else if (pGContext->m_pSecondViewClass == nullptr)
	{
		if (CreateView(pContext, AFX_IDW_PANE_FIRST) == nullptr)
			return FALSE;
	}
	else if(pGContext->m_pSecondViewClass != nullptr && m_wndSplitter.CreateStatic(this, 1, 2) == FALSE)
	{
		TRACE(_T("Failed to CreateStaticSplitter\n"));
		return FALSE;
	}
	else if(!m_wndSplitter.CreateView(0,0,pGContext->m_pFirstViewClass,CSize(500, 0),pContext))
	{
		TRACE(_T("Failed to create second pane\n"));
		return FALSE;
	}
	else if(!m_wndSplitter.CreateView(0,1,pGContext->m_pSecondViewClass,CSize(100,0),pContext))
	{
		TRACE(_T("Failed to create first pane\n"));
		return FALSE;
	}
	else
	{
		for(int row=0;row<m_wndSplitter.GetRowCount();row++)
		{
			for(int col=0;col<m_wndSplitter.GetColumnCount();col++)
			{
				CWnd* pPane = m_wndSplitter.GetPane(row,col);
				if(row==0 && col==0)
					SetActiveView((CView*)pPane);
				else 
					pPane->ShowWindow(SW_HIDE);
			}
			m_wndSplitter.SetColumnInfo(0, 500, 0);
			m_wndSplitter.HideColumn(1);
		}
		return TRUE;
	}

	return TRUE;
}
CWnd* CChildFrame::CreateView(CCreateContext* pContext, UINT nId)
{
	ASSERT(m_hWnd != nullptr);
	ASSERT(::IsWindow(m_hWnd));
	ENSURE_ARG(pContext != nullptr);
	ENSURE_ARG(pContext->m_pNewViewClass != nullptr);

	// Note: can be a CWnd with PostNcDestroy self cleanup
	CWnd* pView = (CWnd*)((GRuntimeClass*)(pContext->m_pNewViewClass))->CreateObject(*(pContext->m_pCurrentDoc));
	if (pView == nullptr)
	{
		TRACE(traceAppMsg, 0, "Warning: Dynamic create of view type %hs failed.\n",	pContext->m_pNewViewClass->m_lpszClassName);
		return nullptr;
	}
	ASSERT_KINDOF(CWnd, pView);

	// views are always created with a border!
	if (!pView->Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW,	CRect(0,0,0,0), this, nId, pContext))
	{
		TRACE(traceAppMsg, 0, "Warning: could not create view for frame.\n");
		return nullptr;        // can't continue without a view
	}

	if (pView->GetExStyle() & WS_EX_CLIENTEDGE)
	{
		// remove the 3d style from the frame, since the view is
		//  providing it.
		// make sure to recalc the non-client area
		ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
	}
	return pView;
}
// CChildFrame message handlers
