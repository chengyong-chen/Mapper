#include "stdafx.h"
#include "TagCtrl.h"
#include "Resource.h"

#include "Global.h"
#include "Tool.h"
#include "LinkTool.h"
#include "HyperlinkStatic.h"
#include "..\Mapper\Global.h"
#include "../Viewer/Global.h"
#include "../Geometry/Geom.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CTool;
class CZoomTool;
class CLinkTool;

extern __declspec(dllexport) CTool* pTool;
extern __declspec(dllexport) CZoomTool zoominTool;
extern __declspec(dllexport) CZoomTool zoomoutTool;
extern __declspec(dllexport) CLinkTool linkTool;

CTagCtrl::CTagCtrl()
{
	m_hOwnerWnd = nullptr;
}

/////////////////////////////////////////////////////////////////////////////
// CTagCtrl message handlers
BEGIN_MESSAGE_MAP(CTagCtrl, CWnd)
	//{{AFX_MSG_MAP(CTagCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SETTINGCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTagCtrl operations
BOOL CTagCtrl::Create(CWnd* pParentWnd)
{
	NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
	LOGFONT LogFont;
    memcpy(&LogFont, &(ncm.lfStatusFont), sizeof(LOGFONT));
	m_font.CreateFontIndirect(&LogFont);

    CString szClassName = AfxRegisterWndClass(CS_CLASSDC|CS_SAVEBITS, LoadCursor(nullptr, IDC_ARROW));

    return CWnd::CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, szClassName, _T(""), WS_POPUP | WS_CLIPSIBLINGS,0, 0, 10, 10,pParentWnd->GetSafeHwnd(), 0, nullptr);
}

void CTagCtrl::OnPaint() 
{
    if(m_strTag.IsEmpty())
        return;

	CPaintDC dc(this); // device context for painting

    CRect rect;
    this->GetClientRect(rect);

	if(m_pHyperlinkStatic != nullptr)
	{
		CRect staticrect;
		m_pHyperlinkStatic->GetClientRect(staticrect);
		m_pHyperlinkStatic->SetWindowPos(&wndTop, (rect.Width()-staticrect.Width())/2, 5, 0,0, SWP_NOSIZE|SWP_SHOWWINDOW);	    

		dc.FillSolidRect(CRect(0,0,rect.right,staticrect.Height()+10),RGB(0,231,45));
		
		rect.top += staticrect.Height()+10;
	}

    // Draw Border
	CBrush brush;
    brush.CreateSolidBrush(GetSysColor(COLOR_INFOBK));
    CBrush* pOldBrush = dc.SelectObject(&brush);
    dc.FillRect(&rect, &brush);
	dc.SelectObject(pOldBrush);
	brush.DeleteObject();

    dc.SelectStockObject(NULL_BRUSH);
    dc.SelectStockObject(BLACK_PEN);
    dc.Rectangle(rect);

    // Draw Text
    CFont *OldFont = (CFont*)dc.SelectObject(&m_font); 
    dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(GetSysColor(COLOR_INFOTEXT));
    rect.DeflateRect(2,3);

    dc.DrawText(m_strTag, rect, DT_EXPANDTABS|DT_EXTERNALLEADING|DT_NOPREFIX|DT_WORDBREAK);

    dc.SelectObject(OldFont);
}

void CTagCtrl::OnTimer(UINT nIDEvent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	switch(nIDEvent) 
    {    
	case 1:	
		{
			this->KillTimer(1);
			if(m_hOwnerWnd != nullptr)
			{
				CPoint pt;		
				GetCursorPos(&pt);
				CPoint point = pt;
				::ScreenToClient(m_hOwnerWnd,&point);

				if(pTool == (CTool*)&zoominTool)
				{
					BOOL bCanZoomIn = ::SendMessage(m_hOwnerWnd,WM_CANZOOMIN,0,(LONG)&point);
					if(bCanZoomIn == TRUE)
					{
						HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_ZOOMIN);
						::SetCursor(hCursor);
					}
				}
				else if(pTool == (CTool*)&linkTool)
				{	
					CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
					if(pMainFrame != nullptr)
					{
						CFrameWnd* pChildFrame = (CFrameWnd*)pMainFrame->GetActiveFrame();
						if(pChildFrame != nullptr)	
						{
							CWnd* pWnd = pChildFrame->GetActiveView();
							if(pWnd != nullptr && pWnd->m_hWnd == m_hOwnerWnd)
							{
								CViewinfo viewinfo;
								linkTool.OnTimerMouseMove(pWnd, viewinfo, 0, point);
							}
						}
					}
				}
				
				if(true)
				{
					CRect client;
					::GetClientRect(m_hOwnerWnd,client);

					if(client.PtInRect(point) == TRUE)
					{
						CString strTag;
						CFrameWnd* pFrameWnd = (CFrameWnd*)AfxGetMainWnd();
						if(pFrameWnd != nullptr)
						{
							CString* pTag = (CString*)pFrameWnd->SendMessage(WM_LOOKFORTIP,point.x,point.y);
							if(pTag != nullptr)
								strTag = *pTag;
						}

						if(strTag.IsEmpty() == TRUE)	
						{
							m_strTag.Empty();
							this->ShowWindow(SW_HIDE);				
						}
						else if(strTag != m_strTag)
						{	
							m_strTag.Empty();
							this->ShowWindow(SW_HIDE);				
							pt += CPoint(0,10);
							this->DisplayTag(pt,strTag);	
						}
						else
						{
							CRect currentRect;
							this->GetWindowRect(&currentRect);
							CRect anticipantRect(pt,currentRect.Size());
							anticipantRect.OffsetRect(2,2);

							 // Need to check it'll fit on screen
						    CSize ScreenSize(::GetSystemMetrics(SM_CXMAXTRACK), ::GetSystemMetrics(SM_CYMAXTRACK));
							if(anticipantRect.right > ScreenSize.cx)// Too far right?
							{
								int nWidth = anticipantRect.Width();
								anticipantRect.left = ScreenSize.cx-nWidth;
								anticipantRect.right = anticipantRect.left + nWidth;
							}
							if(anticipantRect.bottom > ScreenSize.cy)// Bottom falling out of screen?
							{
								int nHeight = anticipantRect.Height();
								anticipantRect.top = ScreenSize.cy-nHeight;
								anticipantRect.bottom = anticipantRect.top+nHeight;
							}
							if(anticipantRect != currentRect)
							{
								this->SetWindowPos(&wndTopMost,anticipantRect.left,anticipantRect.top,0,0,SWP_NOSIZE);
							}
						}
					}
				}
			}
		}
		break;
	default:    
		ASSERT(FALSE);
	}

	CWnd::OnTimer(nIDEvent);
}

void CTagCtrl::OnDestroy() 
{
    this->KillTimer(1);

	if(m_pHyperlinkStatic != nullptr)
	{
		m_pHyperlinkStatic->DestroyWindow();
		delete m_pHyperlinkStatic;
		m_pHyperlinkStatic = nullptr;
	}

	m_font.DeleteObject();

	CWnd::OnDestroy();
}

void CTagCtrl::RelayEvent(MSG* pMsg)
{
	if(this->m_hWnd == nullptr)
		return;

    if(pMsg->message == WM_MOUSEMOVE)
    {
		if(pMsg->hwnd != m_hOwnerWnd) //pMsg->hwndֻ������CView
		{
			this->KillTimer(1);
			this->ShowWindow(SW_HIDE);
			
			m_hOwnerWnd = pMsg->hwnd;
			m_strTag.Empty();
		}
		else
		{
			this->SetTimer(1,500,nullptr);
		}
    }
	else if((pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) || 
	        (pMsg->message  == WM_ACTIVATE && pMsg->wParam == WA_INACTIVE) ||
	 		 pMsg->message  == WM_NCMOUSEMOVE     ||
			 pMsg->message  == WM_LBUTTONDBLCLK   ||
             pMsg->message  == WM_RBUTTONDOWN     ||
			 pMsg->message  == WM_RBUTTONDBLCLK   ||
             pMsg->message  == WM_MBUTTONDOWN     ||
			 pMsg->message  == WM_MBUTTONDBLCLK   ||
             pMsg->message  == WM_NCLBUTTONDOWN   ||
			 pMsg->message  == WM_NCLBUTTONDBLCLK ||
             pMsg->message  == WM_NCRBUTTONDOWN   ||
			 pMsg->message  == WM_NCRBUTTONDBLCLK ||
             pMsg->message  == WM_NCMBUTTONDOWN   ||
			 pMsg->message  == WM_NCMBUTTONDBLCLK ||
			 pMsg->message  == WM_LBUTTONDOWN)
    {
		this->KillTimer(1);
		m_strTag.Empty();
		this->ShowWindow(SW_HIDE);
		m_hOwnerWnd = nullptr;
    }
	else if(pMsg->message == WM_DESTROY)
	{
		this->KillTimer(1);
		m_strTag.Empty();
		this->ShowWindow(SW_HIDE);
		m_hOwnerWnd = nullptr;
	}
}

void CTagCtrl::DisplayTag(const CPoint& pt,CString strTag)
{
    ASSERT(::IsWindow(m_hWnd));

    if(strTag.IsEmpty())
        return;

    //calculate the width and height of the box dynamically
    CWindowDC dc(nullptr);

	CFont *OldFont = (CFont*)dc.SelectObject(&m_font); 

	if(strTag.GetLength()>5 && strTag.Find(_T("Link:")) != -1)
	{
		int i1 = strTag.Find(_T("Link:"));
		int i2 = strTag.Find(_T("\n"),i1);
		
		if(m_pHyperlinkStatic == nullptr)
		{
			CRect linkrect(0,0,1,1);
		    dc.DrawText(_T("View the link"), linkrect, DT_CALCRECT | DT_EXPANDTABS | DT_NOPREFIX);

			m_pHyperlinkStatic = new CHyperlinkStatic;
			m_pHyperlinkStatic->Create(nullptr,WS_CHILD | WS_VISIBLE,linkrect, this);
		}
		if(m_pHyperlinkStatic != nullptr)
		{
			m_pHyperlinkStatic->m_strURL = strTag.Mid(i1+6,i2-(i1+6));
		}
		strTag = strTag.Left(i1) + strTag.Mid(i2+2); 
	}
	else if(m_pHyperlinkStatic != nullptr)
	{
		m_pHyperlinkStatic->DestroyWindow();
		delete m_pHyperlinkStatic;
		m_pHyperlinkStatic = nullptr;
	}
	
	int nLineWidth = 0;
    int nStart = 0;
	CString strTextCopy=strTag;
	do 
	{
		nStart = strTextCopy.Find(_T("\n"));
		if(nStart >= 0)
		{
			CString strLine = strTextCopy.Left(nStart);
			strTextCopy=strTextCopy.Mid(nStart+1);
			nLineWidth = max(nLineWidth, dc.GetTextExtent(strLine).cx);
		}
		else
		{
			nLineWidth = max(nLineWidth, dc.GetTextExtent(strTextCopy).cx);
		}
	}while(nStart > 0);

    CRect rect(0,0,nLineWidth,0);
    dc.DrawText(strTag, rect, DT_CALCRECT | DT_EXPANDTABS | DT_EXTERNALLEADING | DT_NOPREFIX | DT_WORDBREAK);

	if(m_pHyperlinkStatic != nullptr)
	{
		CRect linkrect(0,0,1,1);
	    dc.DrawText(_T("View the link"), linkrect, DT_CALCRECT | DT_EXPANDTABS | DT_NOPREFIX);
		rect.bottom += linkrect.Height() + 10;
		if(rect.Width() < linkrect.Width())
		{
			rect.right = linkrect.right;
		}
	}

    rect.bottom += 6;
    rect.right += 4;
 
    dc.SelectObject(OldFont);
	
    rect.OffsetRect(-rect.TopLeft());
    rect.OffsetRect(pt);
	rect.OffsetRect(2,2);

    // Need to check it'll fit on screen
    CSize ScreenSize(::GetSystemMetrics(SM_CXMAXTRACK), ::GetSystemMetrics(SM_CYMAXTRACK));
    if(rect.right > ScreenSize.cx) // Too far right?
	{
        int nWidth = rect.Width();
        rect.left = ScreenSize.cx-nWidth;
        rect.right = rect.left + nWidth;
	}
    if(rect.bottom > ScreenSize.cy) // Bottom falling out of screen?
    {
        int nHeight = rect.Height();
        rect.top = ScreenSize.cy-nHeight;
        rect.bottom = rect.top+nHeight;
    }

    ShowWindow(SW_HIDE);
    SetWindowPos(&wndTopMost, rect.left, rect.top, rect.Width(), rect.Height(),SWP_SHOWWINDOW|SWP_NOCOPYBITS|SWP_NOACTIVATE|SWP_NOZORDER);

	m_strTag = strTag;
}    

void CTagCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	this->KillTimer(1);

	CWnd::OnMouseMove(nFlags,point);
}
BOOL CTagCtrl::DestroyWindow()
{
	if(m_pHyperlinkStatic != nullptr)
	{
		m_pHyperlinkStatic->DestroyWindow();
		delete m_pHyperlinkStatic;
		m_pHyperlinkStatic = nullptr;
	}

	m_font.DeleteObject();

	return CWnd::DestroyWindow();
}
