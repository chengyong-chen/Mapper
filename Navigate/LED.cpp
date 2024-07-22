#include "stdafx.h"
#include "LED.h"
#include "Resource.h"

#define ID_TIMER_START 1001

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CImageList CLED::m_SignalImage;

/////////////////////////////////////////////////////////////////////////////
// CLED

CLED::CLED()
{
	m_nMode = -1;
	m_bBright = FALSE;

	if(m_SignalImage.m_hImageList==nullptr)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
		AfxSetResourceHandle(hInst);

		if(m_SignalImage.Create(IDB_SIGNAL, 15, 0, RGB(0, 128, 128))==TRUE)
		{
			m_SignalImage.SetOverlayImage(0, 1);
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
}

CLED::~CLED()
{
}

BEGIN_MESSAGE_MAP(CLED, CStatic)
	//{{AFX_MSG_MAP(CLED)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLED message handlers

void CLED::SetMode(int nMode)
{
	if(nMode==m_nMode)
		return;

	switch(nMode)
	{
	case 0:
		SetTimer(ID_TIMER_START, 1000, nullptr);
		break;
	case 1:
		SetTimer(ID_TIMER_START, 1000, nullptr);
		break;
	case 2:
		SetTimer(ID_TIMER_START, 500, nullptr);
		break;
	default:
	{
		KillTimer(ID_TIMER_START);

		this->Invalidate();
		CPaintDC dc(this);
		m_SignalImage.Draw(&dc, 0, CPoint(0, 0), ILD_NORMAL|ILD_TRANSPARENT);
	}
	break;
	}

	m_nMode = nMode;
}

void CLED::OnPaint()
{
	CPaintDC dc(this);

	m_SignalImage.Draw(&dc, 0, CPoint(0, 0), ILD_NORMAL|ILD_TRANSPARENT);
}

void CLED::OnTimer(UINT nIDEvent)
{
	CClientDC dc(this);

	CBrush brush;
	switch(m_nMode)
	{
	case 0:
		if(m_bBright==true)
		{
			m_SignalImage.Draw(&dc, 2, CPoint(0, 0), ILD_NORMAL|ILD_TRANSPARENT);
			m_bBright = false;
		}
		else
		{
			m_SignalImage.Draw(&dc, 3, CPoint(0, 0), ILD_NORMAL|ILD_TRANSPARENT);
			m_bBright = true;
		}
		break;
	case 1:
		if(m_bBright==true)
		{
			m_SignalImage.Draw(&dc, 4, CPoint(0, 0), ILD_NORMAL|ILD_TRANSPARENT);
			m_bBright = false;
		}
		else
		{
			m_SignalImage.Draw(&dc, 5, CPoint(0, 0), ILD_NORMAL|ILD_TRANSPARENT);
			m_bBright = true;
		}
		break;
	case 2:
		if(m_bBright==true)
		{
			m_SignalImage.Draw(&dc, 6, CPoint(0, 0), ILD_NORMAL|ILD_TRANSPARENT);
			m_bBright = false;
		}
		else
		{
			m_SignalImage.Draw(&dc, 7, CPoint(0, 0), ILD_NORMAL|ILD_TRANSPARENT);
			m_bBright = true;
		}
		break;
	}

	CStatic::OnTimer(nIDEvent);
}
