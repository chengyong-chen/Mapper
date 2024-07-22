#include "stdafx.h"
#include "PointCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPointCtrl dialog

CPointCtrl::CPointCtrl(CWnd* pParent /*=nullptr*/)
	: CDialog(CPointCtrl::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPointCtrl)
	m_X = 0.0;
	m_Y = 0.0;
	//}}AFX_DATA_INIT
}

void CPointCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPointCtrl)
	DDX_Text(pDX, IDC_X, m_X);
	DDX_Text(pDX, IDC_Y, m_Y);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPointCtrl, CDialog)
	//{{AFX_MSG_MAP(CPointCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPointCtrl message handlers

void CPointCtrl::OnOK()
{
	CDialog::OnOK();

	m_docPoint.x = (long)m_X*10000;
	m_docPoint.y = (long)m_Y*10000;
}
