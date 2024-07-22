#include "stdafx.h"
#include "TextStyleDlg0.h"
#include "TextStyle.h"

// CTextStyleDlg0 dialog

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CTextStyleDlg0, CDialog)

CTextStyleDlg0::CTextStyleDlg0(CWnd* pParent, TextDeform& deform)
	: CDialog(CTextStyleDlg0::IDD, pParent), m_deform(deform)
{
}

CTextStyleDlg0::~CTextStyleDlg0()
{
}

void CTextStyleDlg0::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextStyleDlg0)
	DDX_Text(pDX, IDC_HANGLE, m_deform.m_nHSkewAngle);
	DDV_MinMaxInt(pDX, m_deform.m_nHSkewAngle, -80, 80);
	DDX_Text(pDX, IDC_VANGLE, m_deform.m_nVSkewAngle);
	DDV_MinMaxInt(pDX, m_deform.m_nVSkewAngle, -80, 80);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTextStyleDlg0, CDialog)
END_MESSAGE_MAP()

// CTextStyleDlg0 message handlers
