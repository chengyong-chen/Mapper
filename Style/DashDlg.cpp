#include "stdafx.h"
#include "resource.h"
#include "DashDlg.h"

#include "../Public/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BYTE d_nUnitIndex;
extern UNIT g_UnitArray[3];

CDashDlg::CDashDlg(CWnd* pParent, DWORD* pseg)
	: CDialog(CDashDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDashDlg)
	d_segon1 = (float)(pseg[0]/(g_UnitArray[d_nUnitIndex].pointpro*10));
	d_segoff1 = (float)(pseg[1]/(g_UnitArray[d_nUnitIndex].pointpro*10));
	d_segon2 = (float)(pseg[2]/(g_UnitArray[d_nUnitIndex].pointpro*10));
	d_segoff2 = (float)(pseg[3]/(g_UnitArray[d_nUnitIndex].pointpro*10));
	d_segon3 = (float)(pseg[4]/(g_UnitArray[d_nUnitIndex].pointpro*10));
	d_segoff3 = (float)(pseg[5]/(g_UnitArray[d_nUnitIndex].pointpro*10));
	d_segon4 = (float)(pseg[6]/(g_UnitArray[d_nUnitIndex].pointpro*10));
	d_segoff4 = (float)(pseg[7]/(g_UnitArray[d_nUnitIndex].pointpro*10));
	//}}AFX_DATA_INIT
}

void CDashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDashDlg)
	DDX_Text(pDX, IDC_SEG1OFF, d_segoff1);
	DDV_MinMaxFloat(pDX, d_segoff1, 0.0f, 100.0f);
	DDX_Text(pDX, IDC_SEG2OFF, d_segoff2);
	DDX_Text(pDX, IDC_SEG3OFF, d_segoff3);
	DDX_Text(pDX, IDC_SEG4OFF, d_segoff4);
	DDX_Text(pDX, IDC_SEG1ON, d_segon1);
	DDX_Text(pDX, IDC_SEG2ON, d_segon2);
	DDX_Text(pDX, IDC_SEG3ON, d_segon3);
	DDX_Text(pDX, IDC_SEG4ON, d_segon4);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDashDlg, CDialog)
	//{{AFX_MSG_MAP(CDashDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDashDlg message handlers

BOOL CDashDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_UNITSHOW)->SetWindowText(g_UnitArray[d_nUnitIndex].unitname);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
