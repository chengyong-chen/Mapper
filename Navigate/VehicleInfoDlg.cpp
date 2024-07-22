#include "stdafx.h"
#include <math.h>
#include "VehicleInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVehicleInfoDlg dialog

CVehicleInfoDlg::CVehicleInfoDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CVehicleInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVehicleInfoDlg)
	m_fSpeed = 0.0;
	m_strRoad = _T("");
	m_nAltitude = 0;
	m_nSatelites = 0;
	//}}AFX_DATA_INIT
	m_nBear = -1;
}

void CVehicleInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVehicleInfoDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVehicleInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CVehicleInfoDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVehicleInfoDlg message handlers

void CVehicleInfoDlg::SetInfo(SMessage& message)
{
	if(message.nSatelites!=m_nSatelites)
	{
		m_nSatelites = message.nSatelites;
		CString strSatelites;
		strSatelites.Format(_T("%d"), m_nSatelites);
		((CStatic*)GetDlgItem(IDC_SATELITE))->SetWindowText(strSatelites);
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
	const int n = (int)(message.nBearing)/5;
	if(n!=m_nBear)
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

			float angle = m_nBear*5;
			CPoint point1;
			CPoint point2;
			CPoint point3;
			point1.y = -37*cos((-angle/180)*M_PI);
			point1.x = 37*sin((-angle/180)*M_PI);
			point2.y = -45*cos((-angle/180)*M_PI);
			point2.x = 45*sin((-angle/180)*M_PI);
			point3.y = -48*cos((-angle/180)*M_PI);
			point3.x = 48*sin((-angle/180)*M_PI);

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

			if(m_nBear%18>4&&m_nBear%18<14)
			{
				if(m_nBear>0&&m_nBear<18)
				{
					pDC->TextOut(0, -47-tm.tmHeight/2, _T("东北"));
				}
				else if(m_nBear>18&&m_nBear<36)
				{
					pDC->TextOut(0, -47-tm.tmHeight/2, _T("东南"));
				}
				else if(m_nBear>36&&m_nBear<54)
				{
					pDC->TextOut(0, -47-tm.tmHeight/2, _T("西南"));
				}
				else if(m_nBear>54&&m_nBear<72)
				{
					pDC->TextOut(0, -47-tm.tmHeight/2, _T("西北"));
				}
			}
			pDC->SetTextColor(oldColor);
			pDC->SelectObject(oldPen);
			pen.DeleteObject();
			pen.CreatePen(PS_SOLID, 2, RGB(134, 107, 173));
			oldPen = (CPen*)pDC->SelectObject(&pen);
			oldColor = pDC->SetTextColor(RGB(61, 17, 123));

			angle = n*5;
			point1.y = -37*cos((-angle/180)*M_PI);
			point1.x = 37*sin((-angle/180)*M_PI);
			point2.y = -45*cos((-angle/180)*M_PI);
			point2.x = 45*sin((-angle/180)*M_PI);
			point3.y = -48*cos((-angle/180)*M_PI);
			point3.x = 48*sin((-angle/180)*M_PI);

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

			if(n%18>4&&n%18<14)
			{
				if(n>0&&n<18)
				{
					pDC->TextOut(0, -47-tm.tmHeight/2, _T("东北"));
				}
				else if(n>18&&n<36)
				{
					pDC->TextOut(0, -47-tm.tmHeight/2, _T("东南"));
				}
				else if(n>36&&n<54)
				{
					pDC->TextOut(0, -47-tm.tmHeight/2, _T("西南"));
				}
				else if(n>54&&n<72)
				{
					pDC->TextOut(0, -47-tm.tmHeight/2, _T("西北"));
				}
			}
			m_nBear = n;

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

void CVehicleInfoDlg::SetRoad(CString strRoad)
{
	if(strRoad==m_strRoad)
		return;

	m_strRoad = strRoad;
	//	((CStatic*)GetDlgItem(IDC_ROAD))->SetWindowText(strRoad);
}
