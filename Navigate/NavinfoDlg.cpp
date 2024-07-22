#include "stdafx.h"
#include <math.h>

#include "NavinfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CNavinfoDlg::CNavinfoDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CNavinfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNavinfoDlg)
	//}}AFX_DATA_INIT

	m_fSpeed = 1.0;
	m_nAltitude = 0;
	m_nSatelites = 0;
	m_nAngle = 0;
	m_bAvailable = TRUE;
}

void CNavinfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNavinfoDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNavinfoDlg, CDialog)
	//{{AFX_MSG_MAP(CNavinfoDlg)
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNavinfoDlg message handlers

void CNavinfoDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CWnd* pWnd = GetDlgItem(IDC_COMPASS);
	if(pWnd!=nullptr)
	{
		CRect rect;
		pWnd->GetClientRect(rect);

		CRgn rgn;
		rgn.CreateEllipticRgnIndirect(rect);

		pWnd->SetWindowRgn(rgn, TRUE);
		rgn.DeleteObject();
	}

	CDialog::OnPaint();

	if(pWnd!=nullptr)
	{
		pWnd->SetWindowRgn(nullptr, FALSE);
	}
}

void CNavinfoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CWnd* pWnd = GetDlgItem(IDC_COMPASS);
	if(pWnd!=nullptr)
	{
		CRect rect;
		pWnd->GetClientRect(rect);
		pWnd->ClientToScreen(rect);
		ScreenToClient(rect);
		pWnd->SetWindowPos(&wndTop, (cx-rect.Width())/2, rect.top, 0, 0, SWP_NOSIZE);
	}
}

void CNavinfoDlg::SetInfo(const DWORD& dwId, SMessage& message)
{
	if(dwId!=m_dwId)
		return;

	if(message.nSatelites!=m_nSatelites)
	{
		m_nSatelites = message.nSatelites;
		CString strSatelites;
		strSatelites.Format(_T("%d"), m_nSatelites);
		((CStatic*)GetDlgItem(IDC_SATELITE))->SetWindowText(strSatelites);
	}
	if(message.m_bAvailable!=m_bAvailable)
	{
		m_bAvailable = message.m_bAvailable;
		if(m_bAvailable==TRUE)
			((CStatic*)GetDlgItem(IDC_AVAILABLE))->SetWindowText(_T("信号有效"));
		else
			((CStatic*)GetDlgItem(IDC_AVAILABLE))->SetWindowText(_T("信号无效"));
	}
	if(message.nTime!=m_nTime)
	{
		m_nTime = message.nTime;
		CString strTime;
		int nHour = m_nTime/10000;
		const int nMinites = (m_nTime-nHour*10000)/100;
		int nSecond = m_nTime-nHour*10000-nMinites*100;

		nHour = (nHour-8+24)%24;
		strTime.Format(_T("%2d点%2d分"), nHour, nMinites);
		((CStatic*)GetDlgItem(IDC_TIME))->SetWindowText(strTime);
	}

	if(message.fSpeed!=m_fSpeed)
	{
		m_fSpeed = message.fSpeed;
		CString strSpeed;
		strSpeed.Format(_T("%.1f"), m_fSpeed);
		((CStatic*)GetDlgItem(IDC_SPEED))->SetWindowText(strSpeed);
	}

	int nAngle = (int)(message.nBearing+2.5)/5;
	nAngle = nAngle*5;
	if(nAngle!=m_nAngle)
	{
		CWnd* pWnd = GetDlgItem(IDC_COMPASS);
		if(pWnd!=nullptr)
		{
			CDC* pDC = pWnd->GetDC();
			CRect rect;
			pWnd->GetClientRect(rect);
			const CPoint olgOrg = pDC->SetViewportOrg(rect.CenterPoint());

			CFont font;
			font.CreatePointFont(120, _T("Arial"), nullptr);
			CPen pen;
			pen.CreatePen(PS_SOLID, 2, RGB(255, 255, 255));

			CFont* oldFont = (CFont*)pDC->SelectObject(&font);
			CPen* oldPen = (CPen*)pDC->SelectObject(&pen);
			const int oldAli = pDC->SetTextAlign(TA_CENTER|TA_TOP);
			COLORREF oldColor = pDC->SetTextColor(RGB(255, 255, 255));
			TEXTMETRIC tm;
			pDC->GetTextMetrics(&tm);

			CPoint point1;
			CPoint point2;
			CPoint point3;
			point1.y = -52*cos(-m_nAngle*M_PI/180);
			point1.x = 52*sin(-m_nAngle*M_PI/180);
			point2.y = -60*cos(-m_nAngle*M_PI/180);
			point2.x = 60*sin(-m_nAngle*M_PI/180);
			point3.y = -65*cos(-m_nAngle*M_PI/180);
			point3.x = 65*sin(-m_nAngle*M_PI/180);

			pDC->MoveTo(point1.x, point1.y);
			pDC->LineTo(point2.x, point2.y);

			pDC->MoveTo(-point1.x, -point1.y);
			pDC->LineTo(-point2.x, -point2.y);

			pDC->MoveTo(-point1.y, point1.x);
			pDC->LineTo(-point2.y, point2.x);

			pDC->MoveTo(point1.y, -point1.x);
			pDC->LineTo(point2.y, -point2.x);

			pDC->TextOut(point3.x, point3.y-tm.tmHeight/2, _T("北"));
			pDC->TextOut(-point3.x, -point3.y-tm.tmHeight/2, _T("南"));
			pDC->TextOut(-point3.y, point3.x-tm.tmHeight/2, _T("东"));
			pDC->TextOut(point3.y, -point3.x-tm.tmHeight/2, _T("西"));

			if(m_nAngle%90>20&&m_nAngle%90<70)
			{
				if(m_nAngle>0&&m_nAngle<90)
				{
					pDC->TextOut(0, -65-tm.tmHeight/2, _T("东北"));
				}
				else if(m_nAngle>90&&m_nAngle<180)
				{
					pDC->TextOut(0, -65-tm.tmHeight/2, _T("东南"));
				}
				else if(m_nAngle>180&&m_nAngle<270)
				{
					pDC->TextOut(0, -65-tm.tmHeight/2, _T("西南"));
				}
				else if(m_nAngle>270&&m_nAngle<360)
				{
					pDC->TextOut(0, -65-tm.tmHeight/2, _T("西北"));
				}
			}

			pDC->SetTextColor(oldColor);
			pDC->SelectObject(oldPen);
			pen.DeleteObject();
			pen.CreatePen(PS_SOLID, 2, RGB(134, 107, 173));
			oldPen = (CPen*)pDC->SelectObject(&pen);
			oldColor = pDC->SetTextColor(RGB(61, 17, 123));

			point1.y = -52*cos(-nAngle*M_PI/180);
			point1.x = 52*sin(-nAngle*M_PI/180);
			point2.y = -60*cos(-nAngle*M_PI/180);
			point2.x = 60*sin(-nAngle*M_PI/180);
			point3.y = -65*cos(-nAngle*M_PI/180);
			point3.x = 65*sin(-nAngle*M_PI/180);

			pDC->MoveTo(point1.x, point1.y);
			pDC->LineTo(point2.x, point2.y);

			pDC->MoveTo(-point1.x, -point1.y);
			pDC->LineTo(-point2.x, -point2.y);

			pDC->MoveTo(-point1.y, point1.x);
			pDC->LineTo(-point2.y, point2.x);

			pDC->MoveTo(point1.y, -point1.x);
			pDC->LineTo(point2.y, -point2.x);

			pDC->TextOut(point3.x, point3.y-tm.tmHeight/2, _T("北"));
			pDC->TextOut(-point3.x, -point3.y-tm.tmHeight/2, _T("南"));
			pDC->TextOut(-point3.y, point3.x-tm.tmHeight/2, _T("东"));
			pDC->TextOut(point3.y, -point3.x-tm.tmHeight/2, _T("西"));

			if(nAngle%90>20&&nAngle%90<70)
			{
				if(nAngle>0&&nAngle<90)
				{
					pDC->TextOut(0, -65-tm.tmHeight/2, _T("东北"));
				}
				else if(nAngle>90&&nAngle<180)
				{
					pDC->TextOut(0, -65-tm.tmHeight/2, _T("东南"));
				}
				else if(nAngle>180&&nAngle<270)
				{
					pDC->TextOut(0, -65-tm.tmHeight/2, _T("西南"));
				}
				else if(nAngle>270&&nAngle<360)
				{
					pDC->TextOut(0, -65-tm.tmHeight/2, _T("西北"));
				}
			}
			m_nAngle = nAngle;

			pDC->SetTextColor(oldColor);
			pDC->SetViewportOrg(olgOrg);
			pDC->SelectObject(oldPen);
			pDC->SelectObject(oldFont);
			pDC->SetTextAlign(oldAli);
			font.DeleteObject();
			pen.DeleteObject();
			pWnd->ReleaseDC(pDC);
		}
	}

	if(message.altitude!=m_nAltitude)
	{
		m_nAltitude = message.altitude;
		CString strAltitude;
		strAltitude.Format(_T("%d"), m_nAltitude);
		((CStatic*)GetDlgItem(IDC_ALTITUDE))->SetWindowText(strAltitude);
	}
}

void CNavinfoDlg::SetID(const DWORD& dwId)
{
	if(dwId!=m_dwId)
	{
		//	((CStatic*)GetDlgItem(IDC_ROAD))->SetWindowText(nullptr);
		((CStatic*)GetDlgItem(IDC_SATELITE))->SetWindowText(_T("0"));
		((CStatic*)GetDlgItem(IDC_ALTITUDE))->SetWindowText(_T("0"));
		((CStatic*)GetDlgItem(IDC_SPEED))->SetWindowText(_T("0"));
		((CStatic*)GetDlgItem(IDC_TIME))->SetWindowText(nullptr);

		((CStatic*)GetDlgItem(IDC_COMPASS))->Invalidate();

		m_fSpeed = 0.0;
		m_nAltitude = 0;
		m_nSatelites = 0;
		m_nAngle = 0;
	}

	m_dwId = dwId;
}

void CNavinfoDlg::SetRouteLength(float fLength) const
{
	CString strLength;
	strLength.Format(_T("%.2f"), fLength);
	strLength.TrimRight(_T("0"));
	strLength.TrimRight(_T("."));
	((CStatic*)GetDlgItem(IDC_ROUTELENGTH))->SetWindowText(strLength);
}

void CNavinfoDlg::SetLeftLength(float fLength) const
{
	CString strLength;
	strLength.Format(_T("%.2f"), fLength);
	strLength.TrimRight(_T("0"));
	strLength.TrimRight(_T("."));
	((CStatic*)GetDlgItem(IDC_LEFTLENGTH))->SetWindowText(strLength);
}
