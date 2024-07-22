#include "stdafx.h"
#include "MainFrm.h"
#include "Resource.h"
#include "HtmlView.h"

#include "GLobal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CHtmlView, CWnd)
/////////////////////////////////////////////////////////////////////////////

CHtmlView::CHtmlView()
{
	m_bCanForward = FALSE;
	m_bCanBack = FALSE;
}

CHtmlView::~CHtmlView()
{
}

BEGIN_MESSAGE_MAP(CHtmlView, CWnd)
	//{{AFX_MSG_MAP(CHtmlView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CHtmlView, CWnd)
	//{{AFX_EVENTSINK_MAP(CHtmlView)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 250 /* BeforeNavigate2 */, BeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CHtmlView, AFX_IDW_PANE_FIRST, 105 /* CommandStateChange */, OnCommandStateChange, VTS_I4 VTS_BOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHtmlView drawing

/////////////////////////////////////////////////////////////////////////////
// CHtmlView diagnostics

#ifdef _DEBUG
void CHtmlView::AssertValid() const
{
	CWnd::AssertValid();
}

void CHtmlView::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHtmlView message handlers
int CHtmlView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CWnd::OnCreate(lpCreateStruct)==-1)
		return -1;

	// Create WebBrowser control
	if(!m_WebBrowser.Create(nullptr, WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST))
		return -1;

	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);

	return 0;
}

void CHtmlView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if(m_WebBrowser.m_hWnd!=nullptr)
	{
		m_WebBrowser.MoveWindow(0, 0, cx, cy);
		m_WebBrowser.UpdateWindow();
	}
}

void CHtmlView::Navigate(LPCTSTR lpszNavigatePage)
{
	if(lpszNavigatePage)
	{
		m_WebBrowser.Navigate(lpszNavigatePage, nullptr, nullptr, nullptr, nullptr);
	}
}

////////////////////////////////////////////////////////////////////
// WebBrowser Events
void CHtmlView::GoBack()
{
	// Go back one page
	try
	{
		// Navigate to the previous page in the history list
		m_WebBrowser.GoBack();
	}
	catch(CException*ex)
	{
		//		OutputDebugString(ex->m_strError);
		ex->Delete();
	}
}

void CHtmlView::GoForward()
{
	try
	{
		m_WebBrowser.GoForward();
	}
	catch(CException*ex)
	{
		ex->Delete();
	}
}

void CHtmlView::OnCommandStateChange(long nCommand, BOOL bEnable)
{
	switch(nCommand)
	{
	case CSC_NAVIGATEFORWARD:
		m_bCanForward = bEnable;
		break;
	case CSC_NAVIGATEBACK:
		m_bCanBack = bEnable;
		break;
	}
}

BOOL CHtmlView::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo==nullptr)
	{
		if(nCode==CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CWnd::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			switch(nId)
			{
			case ID_GO_FORWARD:
				pCmdUI->Enable(m_bCanForward);
				return TRUE;
				break;

			case ID_TOOL_PAN:
			case ID_TOOL_PRINT:
			case ID_TOOL_ZOOMIN:
			case ID_TOOL_ZOOMOUT:
			case ID_VIEW_MANAGER:
			case ID_VIEW_RULER:
			case ID_GPS_TRACE:
			case ID_GPS_DEVICE:
				pCmdUI->Enable(FALSE);
				return TRUE;
				break;
			default:
				break;
			}
			pCmdUI->m_bContinueRouting = false;
		}
	}

	return CWnd::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}

void CHtmlView::BeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel)
{
	ASSERT(V_VT(URL)==VT_BSTR);
	ASSERT(V_VT(TargetFrameName)==VT_BSTR);
	ASSERT(V_VT(PostData)==(VT_VARIANT|VT_BYREF));
	ASSERT(V_VT(Headers)==VT_BSTR);
	ASSERT(Cancel!=nullptr);

	USES_CONVERSION;

	VARIANT* vtPostedData = V_VARIANTREF(PostData);
	CByteArray array;
	if(V_VT(vtPostedData)&VT_ARRAY)
	{
		// must be a vector of bytes
		ASSERT(vtPostedData->parray->cDims==1&&vtPostedData->parray->cbElements==1);

		vtPostedData->vt |= VT_UI1;
		COleSafeArray safe(vtPostedData);

		DWORD dwSize = safe.GetOneDimSize();
		LPVOID pVoid;
		safe.AccessData(&pVoid);

		array.SetSize(dwSize);
		LPBYTE lpByte = array.GetData();

		memcpy(lpByte, pVoid, dwSize);
		safe.UnaccessData();
	}
	// make real parameters out of the notification

	CString strTargetFrameName(V_BSTR(TargetFrameName));
	CString strURL(V_BSTR(URL));
	CString strHeaders(V_BSTR(Headers));
	DWORD nFlags = V_I4(Flags);

	// notify the user's class
	OnBeforeNavigate2(strURL, nFlags, strTargetFrameName, array, strHeaders, Cancel);
}

void CHtmlView::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostData, LPCTSTR lpszHeaders, BOOL* bCancel)
{
	// default to continuing
	*bCancel = FALSE;

	// user will override to handle this notification
	UNUSED_ALWAYS(lpszURL);
	UNUSED_ALWAYS(nFlags);
	UNUSED_ALWAYS(lpszTargetFrameName);
	UNUSED_ALWAYS(baPostData);
	UNUSED_ALWAYS(lpszHeaders);

	static CString strURL;
	strURL = lpszURL;
	strURL.MakeUpper();
	if(strURL.GetLength()>11&&strURL.Left(11)==_T("SAMSON:COM="))
	{
		*bCancel = TRUE;

		static CString strPath;
		strPath = m_WebBrowser.GetLocationURL();
		if(strPath.ReverseFind(_T('/'))!=-1)
		{
			strPath = strPath.Left(strPath.ReverseFind(_T('/'))+1);
		}
		if(strPath.GetLength()>=8&&strPath.Left(8)==_T("file:///"))
			strPath = strPath.Mid(8);
		if(strPath.GetLength()>=5&&strPath.Left(5)==_T("file:"))
			strPath = strPath.Mid(5);
		strPath.Replace(_T("%20"), _T(" "));

		AfxGetMainWnd()->PostMessage(WM_URLCHANGED, (WPARAM)&strURL, (WPARAM)&strPath);
	}

	/*	else if(CMainFrame::m_pAtlas != nullptr)
		{
			CWnd* pParents = GetParent();
			if(pParents != nullptr)
			{
				CRect rect;
				pParents->GetClientRect(rect);
				CSize size = rect.CSize();

				CString strFile = CMainFrame::m_pAtlas->m_strHomepage;
				strFile.MakeUpper();
				if(strURL.Find(strFile) != -1)
					SetWindowPos(&wndTop, 0, 0, size.cx, size.cy, SWP_SHOWWINDOW);
				else
					SetWindowPos(&wndTop, 0, 42, size.cx, size.cy-42, SWP_SHOWWINDOW);
			}
		}*/
}
