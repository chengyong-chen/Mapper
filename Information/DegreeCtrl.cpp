#include "stdafx.h"
#include "DegreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDegreeCtrl dialog

//double CDegreeCtrl::m_y = 0;
//double CDegreeCtrl::m_x = 0;

CDegreeCtrl::CDegreeCtrl(CWnd* pParent, CPointF& point)
	: CDialog(CDegreeCtrl::IDD, pParent), m_point(point)
{
	//{{AFX_DATA_INIT(CDegreeCtrl)
	m_y = point.y;
	m_x = point.x;
	//}}AFX_DATA_INIT		
}

void CDegreeCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDegreeCtrl)
	DDX_Text(pDX, IDC_Y, m_y);
	DDX_Text(pDX, IDC_X, m_x);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDegreeCtrl, CDialog)
	//{{AFX_MSG_MAP(CDegreeCtrl)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDegreeCtrl message handlers

BOOL CDegreeCtrl::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CButton*)GetDlgItem(IDC_GPS))->SetCheck(TRUE);

	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDegreeCtrl::OnOK()
{
	UpdateData(TRUE);

	if((((CButton*)GetDlgItem(IDC_GPS))->GetState()&0X0003))
	{
		int degree = int(m_x/100);
		double minutes = m_x-degree*100;

		m_point.x = degree+minutes/60;

		degree = int(m_y/100);
		minutes = m_y-degree*100;
		m_point.y = degree+minutes/60;
	}
	else
	{
		m_point.x = m_x;
		m_point.y = m_y;
	}
}

BOOL CDegreeCtrl::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_RETURN:
			CWnd* pWnd = GetParent();
			if(pWnd!=nullptr)
				pWnd->SendMessage(WM_COMMAND, IDC_LOCATE, 0);
			break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
