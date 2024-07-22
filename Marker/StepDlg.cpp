#include "stdafx.h"
#include "StepDlg.h"

#include "../Public/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UNIT a_UnitArray[3];
extern __declspec(dllimport) BYTE d_nUnitIndex;

CStepDlg::CStepDlg(CWnd* pParent /*=nullptr*/, UINT pStep)
	: CDialog(CStepDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStepDlg)
	m_fStep = (float)(pStep/(a_UnitArray[d_nUnitIndex].pointpro*10));
	//}}AFX_DATA_INIT
}

void CStepDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStepDlg)
	DDX_Text(pDX, IDC_STEP, m_fStep);
	DDV_MinMaxFloat(pDX, m_fStep, 0.1f, 100.0f);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStepDlg, CDialog)
	//{{AFX_MSG_MAP(CStepDlg)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()