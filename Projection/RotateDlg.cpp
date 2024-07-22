#include "stdafx.h"

#include "RotateDlg.h"
#include "Projection1.h"

#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CRotateDlg::CRotateDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CRotateDlg::IDD, pParent)
	, m_angle(0)
	, m_lngDelta (0)
	, m_latDelta (0)
{
	//{{AFX_DATA_INIT(CRotateDlg)
	//}}AFX_DATA_INIT
}

CRotateDlg::~CRotateDlg()
{
}

void CRotateDlg::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CRotateDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Text (pDX, IDC_FACING, m_angle);
	DDX_Text (pDX, IDC_LNGDELTA, m_lngDelta);
	DDX_Text (pDX, IDC_LATDELTA, m_latDelta);
	DDV_MinMaxFloat (pDX, m_lngDelta, -180, 180);
	DDV_MinMaxFloat (pDX, m_latDelta, -90, 90);
}

BEGIN_MESSAGE_MAP(CRotateDlg, CDialog)
	//{{AFX_MSG_MAP(CRotateDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRotateDlg message handlers

BOOL CRotateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRotateDlg::OnOK()
{

	CDialog::OnOK();
}