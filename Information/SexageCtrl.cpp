#include "stdafx.h"
#include  <stdio.h>
#include  <stdlib.h>
#include  <cmath>
#include "SexageCtrl.h"
#include "CooQueryEar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSexageCtrl dialog

CSexageCtrl::CSexageCtrl(CWnd* pParent /*=nullptr*/)
	: CDialog(CSexageCtrl::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSexageCtrl)
	m_LatD = 0;
	m_LatM = 0;
	m_LatS = 0;
	m_LonD = 0;
	m_LonM = 0;
	m_LonS = 0;
	//}}AFX_DATA_INIT
}

void CSexageCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSexageCtrl)
	DDX_Text(pDX, IDC_LATD, m_LatD);
	DDV_MinMaxInt(pDX, m_LatD, -90, 90);
	DDX_Text(pDX, IDC_LATM, m_LatM);
	DDV_MinMaxInt(pDX, m_LatM, 0, 60);
	DDX_Text(pDX, IDC_LATS, m_LatS);
	DDV_MinMaxFloat(pDX, m_LatS, 0.f, 60.f);
	DDX_Text(pDX, IDC_LOND, m_LonD);
	DDV_MinMaxInt(pDX, m_LonD, -180, 180);
	DDX_Text(pDX, IDC_LONM, m_LonM);
	DDV_MinMaxInt(pDX, m_LonM, 0, 60);
	DDX_Text(pDX, IDC_LONS, m_LonS);
	DDV_MinMaxFloat(pDX, m_LonS, 0.f, 60.f);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSexageCtrl, CDialog)
	//{{AFX_MSG_MAP(CSexageCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSexageCtrl message handlers

void CSexageCtrl::OnOK()
{
	UpdateData(TRUE);

	double lng = std::abs(m_LonD)+(double)(std::abs(m_LonM)+(double)std::abs(m_LonS)/60)/60;
	if(m_LonD<0)
		lng *= -1;

	double lat = std::abs(m_LatD)+(double)(std::abs(m_LatM)+(double)std::abs(m_LatS)/60)/60;
	if(m_LatD<0)
		lat *= -1;

	CCooQueryEar* p = (CCooQueryEar*)GetParent();
	p->m_geoPoint = CPointF(lng, lat);
}

BOOL CSexageCtrl::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_RETURN:
			CWnd* pWnd = GetParent();
			if(pWnd!=nullptr)
				pWnd->SendMessage(WM_COMMAND, IDC_LOCATE, 0);
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
