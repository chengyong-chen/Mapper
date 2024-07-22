#include "stdafx.h"
#include "PoleDlg.h"

#include "../Public/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UNIT a_UnitArray[3];
extern __declspec(dllimport) BYTE d_nUnitIndex;

CPoleDlg::CPoleDlg(CWnd* pParent, long x, long y, BOOL bDirection)
	: CDialog(CPoleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPoleDlg)
	m_bDirection = bDirection;
	m_fX = (float)(x/(a_UnitArray[d_nUnitIndex].pointpro*10000));
	m_fY = (float)(y/(a_UnitArray[d_nUnitIndex].pointpro*10000));
	//}}AFX_DATA_INIT
}

void CPoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPoleDlg)
	DDX_Check(pDX, IDC_DIRECTION, m_bDirection);
	DDX_Text(pDX, IDC_X, m_fX);
	DDX_Text(pDX, IDC_Y, m_fY);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPoleDlg, CDialog)
	//{{AFX_MSG_MAP(CPoleDlg)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPoleDlg message handlers
