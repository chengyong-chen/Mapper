// Credit.cpp : implementation file
//

#include "stdafx.h"
#include "Setup.h"
#include <string.h>

#include "Credit.h"
#include "SetupDlg.h"
#include "GeoDib32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCredit

CCredit::CCredit()
{
	m_hBitmap = nullptr;
	m_nOffset = 5;
	m_count   = 0;
	m_nAllCount = 0;

	m_colCreditHeadColor = RGB(255, 0, 0);
	m_colCreditSecondColor = RGB(255, 255, 0);
}

CCredit::~CCredit()
{
	if(m_hBitmap)
	{
		::DeleteObject(m_hBitmap);
		m_hBitmap = nullptr;
	}
}


BEGIN_MESSAGE_MAP(CCredit, CWnd)
	//{{AFX_MSG_MAP(CCredit)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCredit message handlers

BOOL CCredit::OnEraseBkgnd(CDC* pDC) 
{
	
	return TRUE;
}

void CCredit::OnPaint() 
{
	CPaintDC dc(this);

	RECT rcClient;
	GetClientRect(&rcClient);

	HDC hmemDC = ::CreateCompatibleDC(dc.m_hDC);
	if(hmemDC != nullptr)
	{
		if(m_hBitmap != nullptr)
		{
			HBITMAP old = (HBITMAP)::SelectObject(hmemDC, m_hBitmap);
			BitBlt(dc.m_hDC, 0, 0, rcClient.right, rcClient.bottom, hmemDC, 0, 0, SRCCOPY);
			::SelectObject(hmemDC, old);
		}

		::DeleteDC(hmemDC);
	}
}

#define IDT_SCROLL 1
void CCredit::OnTimer(UINT nIDEvent) 
{
	if(IDT_SCROLL == nIDEvent)
	{
		RECT rcClient, rcDraw, rcText;
		
		CSetupDlg* pDlg = (CSetupDlg*)GetParent();
		pDlg->GetClientRect(&rcClient);
		if(rcClient.right - rcClient.left == 0 || rcClient.bottom - rcClient.top == 0)
		{
			return;
		}

		GetClientRect(&rcClient);

		HDC hDC = ::GetDC(m_hWnd);
		HDC hmemDC = ::CreateCompatibleDC(hDC);
		if(!m_hBitmap)
		{
			m_hBitmap = CreateCompatibleBitmap(hDC, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
		}
		::ReleaseDC(m_hWnd, hDC);

		int nSaveDC = ::SaveDC(hmemDC);
		::SelectObject(hmemDC, m_hBitmap);
		if(pDlg->m_hDIB)
		{
			pDlg->GetClientRect(&rcClient);
			pDlg->ClientToScreen(&rcClient);
			ScreenToClient(&rcClient);
			PaintDIB(hmemDC, &rcClient/*CRect(0, 0, 398, 270)*/, pDlg->m_hDIB, CRect(0, 29, 398, 299)/*CRect(0, 0, 398, 270)*/, nullptr);
		}
		
		GetClientRect(&rcClient);
		rcDraw = rcClient;
		rcDraw.bottom;
		::OffsetRect(&rcDraw, 0, m_nOffset); 
		rcDraw.left  += 5;
		rcDraw.right -= 5;

		HFONT font, oldfont;
		LOGFONT logfont;
		ZeroMemory(&logfont, sizeof(LOGFONT));
		::GetObject((HFONT)::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &logfont);
		logfont.lfHeight = -18;
		logfont.lfWeight = 700;
		wsprintf(logfont.lfFaceName, _T("����_GB2312"));
		font = ::CreateFontIndirect(&logfont);
		oldfont = (HFONT)::GetCurrentObject(hmemDC, OBJ_FONT);

		CString line, head, tail, r;
		::SetBkMode(hmemDC, TRANSPARENT);
		for(int I = 0; I < m_rCreditArray.GetSize(); I ++)
		{
			line = m_rCreditArray[I];
			int length = line.GetLength();
			int n = line.Find(_T(";"));
			if(n == -1) continue;
			head = line.Left(n);
			tail = line.Right(length - n - 1);
			head.TrimLeft(); head.TrimRight();
			tail.TrimLeft(); tail.TrimRight();

			if(head.GetLength())
			{
				::SetTextColor(hmemDC, m_colCreditHeadColor);
				::SelectObject(hmemDC, font);
				rcText = rcDraw;
				::DrawText(hmemDC, head, head.GetLength(), &rcText, DT_CENTER | DT_WORDBREAK | DT_CALCRECT | DT_EXTERNALLEADING);
				::DrawText(hmemDC, head, head.GetLength(), &rcDraw, DT_CENTER | DT_WORDBREAK | DT_EXTERNALLEADING);
				::OffsetRect(&rcDraw, 0, rcText.bottom - rcText.top + 8);
			}
			
			if(tail.GetLength())
			{
				::SetTextColor(hmemDC, m_colCreditSecondColor);
				::SelectObject(hmemDC, oldfont);

				char seps[]   = ",";
				char *token;
				char *p = tail.GetBuffer(1);
				token = strtok(p, seps);
				while(token != nullptr)
				{
					r = token;
					rcText = rcDraw;
					::DrawText(hmemDC, r, r.GetLength(), &rcText, DT_CENTER | DT_WORDBREAK | DT_CALCRECT | DT_EXTERNALLEADING);
					::DrawText(hmemDC, r, r.GetLength(), &rcDraw, DT_CENTER | DT_WORDBREAK | DT_EXTERNALLEADING);
					::OffsetRect(&rcDraw, 0, rcText.bottom - rcText.top + 5);
					token = strtok(nullptr, seps);
				}
				::OffsetRect(&rcDraw, 0, rcText.bottom - rcText.top + 10);
				tail.ReleaseBuffer();
			}
		}
		
		::RestoreDC(hmemDC, nSaveDC);
		::DeleteDC(hmemDC);
		::DeleteObject(font);
		
		m_nAllCount ++;
		if((m_nOffset <= 5 && m_nOffset > 0) && m_count < 20)
		{
			if(m_nAllCount > 300) 
				this->KillTimer(IDT_SCROLL);
			m_count ++;
		}
		else
		{
			m_count = 0;
			if(rcDraw.top < 0)
				m_nOffset = rcClient.bottom;
			else
				m_nOffset -= 5;
		}

		Invalidate();
	}
	
	CWnd::OnTimer(nIDEvent);
}

void CCredit::PreSubclassWindow() 
{
	CWnd::PreSubclassWindow();
	this->SetTimer(IDT_SCROLL, 100, nullptr);
}
