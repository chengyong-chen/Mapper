#include "stdafx.h"
#include "resource.h"
#include <io.h>
#include <stdio.h>
#include "Viewer.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "GrfDoc.h"
#include "GrfView.h"
#include "GisView.h"
#include "Global.h"

#include "../Atlas/Atlas1.h"
#include "../Atlas/Global.h"
#include "../Hyperlink/History.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern BOOL a_bGPSAuto;

extern BOOL AFXAPI AfxComparePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2);

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_COMMAND(ID_GO_FORWARD, OnGoForward)
	ON_COMMAND(ID_GO_BACK, OnGoBack)
	ON_COMMAND(ID_GO_HOME, OnGoHome)
	ON_WM_PAINT()
	ON_COMMAND(ID_VIEW_RULER, OnViewRuler)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_ISMAINCHILD, OnIsMainChild)
	ON_MESSAGE(WM_OPENDOCUMENTFILE, OnOpenDocumentFile)
	ON_MESSAGE(WM_ADDHISTORY, OnAddHistory)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	m_bMainChild = TRUE;

	CHistory history;
	m_Go.AddHistory(history);

	m_ruler.m_bVisible = false;
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.x = 0;
	cs.y = 0;
	cs.cx = ::GetSystemMetrics(SM_CXSCREEN);
	cs.cy = ::GetSystemMetrics(SM_CYSCREEN);

	cs.style |= WS_MAXIMIZE|WS_VISIBLE;
	cs.style &= ~WS_SYSMENU;

	return CMDIChildWndEx::PreCreateWindow(cs);
}

// CChildFrame diagnostics
BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT, CCreateContext* pContext)
{
	// default create client will create a view if asked for it
	if(pContext!=nullptr&&pContext->m_pNewViewClass!=nullptr)
	{
		if(CreateView(pContext, AFX_IDW_PANE_FIRST)==nullptr)
			return FALSE;
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
		TRACE(traceAppMsg, 0, "Warning: Dynamic create of view type %hs failed.\n",
			pContext->m_pNewViewClass->m_lpszClassName);
		return nullptr;
	}
	ASSERT_KINDOF(CWnd, pView);

	// views are always created with a border!
	if(!pView->Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, nId, pContext))
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
#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif//_DEBUG
void CChildFrame::OnGoForward()
{
	m_Go.GoForward(this);
}

void CChildFrame::OnGoBack()
{
	m_Go.GoBack(this);
}

void CChildFrame::OnGoHome()
{
	if(m_Go.CanHome()==TRUE)
	{
		m_Go.GoHome(this);
	}
	if(CMainFrame::m_pAtlas!=nullptr)
	{
		CString strFile = CMainFrame::m_pAtlas->m_strHomePage;
		if(IsWebFile(strFile)==TRUE)
		{
			AfxGetMainWnd()->SendMessage(WM_BROWSEHTML, (UINT)&strFile, 0);
		}
	}
}

BOOL CChildFrame::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo==nullptr)
	{
		if(nCode==CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CMDIChildWnd::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			switch(nId)
			{
			case ID_GO_FORWARD:
				pCmdUI->Enable(m_Go.CanForward());
				return TRUE;
				break;
			case ID_VIEW_RULER:
				pCmdUI->SetCheck(m_ruler.m_bVisible);
				break;
			default:
				break;
			}
			pCmdUI->m_bContinueRouting = false;
		}
	}

	return CMDIChildWnd::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}

LONG CChildFrame::OnIsMainChild(UINT WPARAM, LONG LPARAM)
{
	return m_bMainChild;
}

void CChildFrame::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CView* pView = (CView*)GetActiveView();
	if(pView!=nullptr&&m_ruler.m_bVisible==true)
	{
		CRect client;
		pView->GetClientRect(client);
		const CPoint start = client.CenterPoint();

		m_ruler.DrawH(pView, this, start);
		m_ruler.DrawV(pView, this, start);
	}
}

void CChildFrame::OnViewRuler()
{
	CView* pView = (CView*)GetActiveView();
	if(pView!=nullptr)
	{
		m_ruler.ShowSwitch(pView, ((CGrfView*)pView)->m_viewMonitor);
	}
}

LONG CChildFrame::OnAddHistory(UINT WPARAM, LONG LPARAM)
{
	CDocument* pDocument = (CDocument*)WPARAM;
	if(pDocument==nullptr)
		return 0;

	if(m_bMainChild==FALSE)
		return 0;
	const CString strFile = pDocument->GetPathName();

	if(LPARAM==0)
	{
		const int nIndex = m_Go.m_nCurent-1;
		CHistory* history = m_Go.GetHistory(nIndex);
		if(history!=nullptr)
		{
			TCHAR szMap[_MAX_PATH];
			GetLongPathName(history->m_strMap, szMap, _MAX_PATH);
			TCHAR szFile[_MAX_PATH];
			GetLongPathName(strFile, szFile, _MAX_PATH);

			if(AfxComparePath(szMap, szFile)==TRUE)
			{
				m_Go.m_nCurent -= 1;
				return 0;
			}
		}
	}
	else
	{
		const int nIndex = m_Go.m_nCurent+1;
		CHistory* history = m_Go.GetHistory(nIndex);
		if(history!=nullptr&&strFile.CompareNoCase(history->m_strMap)==0)
		{
			m_Go.m_nCurent += 1;
			return 0;
		}
	}

	CHistory history;
	history.m_strMap = strFile;
	m_Go.AddHistory(history);
	return 1;
}

LONG CChildFrame::OnOpenDocumentFile(UINT WPARAM, LONG LPARAM)
{
	if(m_bMainChild==TRUE) // Set history only for main childframe
	{
		CView* pView = GetActiveView();
		if(pView!=nullptr)
		{
			CHistory* history = m_Go.GetCurentHistory();
			if(history!=nullptr)
			{
				CRect rect;
				pView->GetClientRect(rect);
				const CRect docRect = ((CGrfView*)pView)->m_viewMonitor.ClientToDoc(rect);
				history->m_fScale = ((CGrfView*)pView)->m_viewMonitor.m_scaleCurrent;
				history->m_Anchor.x = docRect.left+docRect.Width()/2;
				history->m_Anchor.x = docRect.top+docRect.Height()/2;

				CGrfDoc* pDoc = (CGrfDoc*)pView->GetDocument();
				if(pDoc!=nullptr)
				{
					history->m_strMap = pDoc->GetPathName();
				}
			}
		}
	}

	const CViewerApp* pApp = (CViewerApp*)AfxGetApp();
	const CString strFile = *(CString*)WPARAM;

	if(_taccess(strFile, 00)==-1)
	{
		AfxMessageBox(IDS_MAPFILENOTEXSIT);
		return 0;
	}

	CDocument* pDocument = GetActiveDocument();
	if(pDocument!=nullptr)
	{
		const CString string = pDocument->GetPathName();
		if(string.CompareNoCase(strFile)==0)
		{
			CView* pView = GetActiveView();
			pView->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			return (LONG)pDocument;
		}
		if(pDocument->CanCloseFrame(this)==FALSE)
			return 0;

		bool bOtherFrame = FALSE;
		POSITION pos = pDocument->GetFirstViewPosition();
		while(pos!=nullptr)
		{
			CView* pView = pDocument->GetNextView(pos);
			ASSERT_VALID(pView);
			if(pView->GetParentFrame()!=this)
			{
				bOtherFrame = TRUE;
				break;
			}
		}

		if(!bOtherFrame)
		{
			const BOOL bAutoDelete = pDocument->m_bAutoDelete;
			pDocument->m_bAutoDelete = FALSE;
			POSITION pos = pDocument->GetFirstViewPosition();
			while(pos!=nullptr)
			{
				CView* pView = pDocument->GetNextView(pos);
				ASSERT_VALID(pView);
				CFrameWnd* pFrame = pView->GetParentFrame();
				ASSERT_VALID(pFrame);

				pDocument->PreCloseFrame(pFrame);
				pView->DestroyWindow();
			}
			pDocument->m_bAutoDelete = bAutoDelete;

			pDocument->DeleteContents();
			if(pDocument->m_bAutoDelete)
				delete pDocument;
		}
		else
		{
			pDocument->PreCloseFrame(this);
			CView* pView = GetActiveView();
			if(pView!=nullptr)
			{
				pView->DestroyWindow();
			}
		}
	}

	CDocTemplate::Confidence bestMatch = CDocTemplate::noAttempt;
	CDocTemplate* pBestTemplate = nullptr;
	CDocument* pOpenDocument = nullptr;

	POSITION pos = pApp->GetFirstDocTemplatePosition();
	while(pos!=nullptr)
	{
		CDocTemplate* pTemplate = pApp->GetNextDocTemplate(pos);
		ASSERT_KINDOF(CDocTemplate, pTemplate);

		CDocTemplate::Confidence match;
		ASSERT(pOpenDocument==nullptr);
		match = pTemplate->MatchDocType(strFile, pOpenDocument);
		if(match>bestMatch)
		{
			bestMatch = match;
			pBestTemplate = pTemplate;
		}
		if(match==CDocTemplate::yesAlreadyOpen)
			break; // stop here
	}

	if(pOpenDocument==nullptr)
	{
		if(pBestTemplate==nullptr)
		{
			AfxMessageBox(AFX_IDP_FAILED_TO_OPEN_DOC);
			return 0;
		}

		pDocument = pBestTemplate->CreateNewDocument();
		if(pDocument==nullptr)
		{
			TRACE0("CDocTemplate::CreateNewDocument returned nullptr.\n");
			AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
			return 0;
		}

		CWaitCursor wait;
		if(!pDocument->OnOpenDocument(strFile))
		{
			// user has be alerted to what failed in OnOpenDocument
			TRACE0("CDocument::OnOpenDocument returned FALSE.\n");
			this->DestroyWindow();
			return 0;
		}

		pDocument->SetPathName(strFile);
	}
	else
	{
		pDocument = pOpenDocument;
	}
	ASSERT_VALID(pDocument);

	//	ASSERT(pBestTemplate->m_nIDResource != 0); // must have a resource ID to load from
	CCreateContext context;
	context.m_pCurrentFrame = this;
	context.m_pCurrentDoc = pDocument;
	//	context.m_pNewViewClass = pBestTemplate->m_pViewClass;
	context.m_pNewDocTemplate = pBestTemplate;

	CString strEXT = strFile.Right(3);
	strEXT.MakeUpper();
	int nIDResource;
	if(strEXT==_T("GRF"))
	{
		nIDResource = IDR_GRFTYPE;
		context.m_pNewViewClass = GWRUNTIME_CLASS(CGrfView);
	}
	if(strEXT==_T("GIS"))
	{
		nIDResource = IDR_GISTYPE;
		context.m_pNewViewClass = GWRUNTIME_CLASS(CGisView);
	}

	if(CreateView(&context, AFX_IDW_PANE_FIRST)==nullptr)
		return 0;

	// post message for initial message string
	PostMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);

	// make sure the child windows have been properly sized
	RecalcLayout();

	CWnd* pWnd = this->GetDescendantWindow(AFX_IDW_PANE_FIRST, TRUE);
	if(pWnd!=nullptr)
	{
		switch(LPARAM)
		{
		case -1:
			((CGrfView*)pWnd)->m_viewMonitor.m_scaleCurrent = ((CGrfDoc*)pDocument)->m_Datainfo.m_scaleMinimum;
			break;
		case 0:
			((CGrfView*)pWnd)->m_viewMonitor.m_scaleCurrent = ((CGrfDoc*)pDocument)->m_Datainfo.m_scaleSource;
			break;
		case 1:
			((CGrfView*)pWnd)->m_viewMonitor.m_scaleCurrent = ((CGrfDoc*)pDocument)->m_Datainfo.m_scaleMaximum;
			break;
		default:
			((CGrfView*)pWnd)->m_viewMonitor.m_scaleCurrent = LPARAM;
			break;
		}
	}

	pBestTemplate->InitialUpdateFrame(this, pDocument, TRUE);

	AfxGetMainWnd()->SendMessage(WM_CHILDDOCCHANGED, (UINT)this, 0);

	return (LONG)pDocument;
}
