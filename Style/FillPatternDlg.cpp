#include "stdafx.h"

#include "Global.h"
#include "FillPattern.h"
#include "FillPatternDlg.h"
#include "PatternMid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFillPatternDlg dialog

CFillPatternDlg::CFillPatternDlg(CWnd* pParent, CMidtable<CPatternMid>& tagtable, CFillPattern* pFill)
	: TPatternMidDlg(CFillPatternDlg::IDD, pParent, tagtable, pFill)
{
	//{{AFX_DATA_INIT(CFillPatternDlg)

	//}}AFX_DATA_INIT
	m_nAngle = pFill==nullptr ? 0 : pFill->m_nAngle;
}

void CFillPatternDlg::DoDataExchange(CDataExchange* pDX)
{
	TPatternMidDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFillPatternDlg)
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_ANGLE, m_nAngle);
	DDV_MinMaxShort(pDX, m_nAngle, 0, 180);
}

BEGIN_MESSAGE_MAP(CFillPatternDlg, TPatternMidDlg)
	//{{AFX_MSG_MAP(CFillPatternDlg)
	ON_EN_CHANGE(IDC_ANGLE, OnChangeAngle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFillPatternDlg message handlers
BOOL CFillPatternDlg::OnInitDialog()
{
	TPatternMidDlg::OnInitDialog();

	SetWindowPos(&wndTop, 8, 45, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFillPatternDlg::OnOK()
{
	TPatternMidDlg::OnOK();

	if(m_pPattern!=nullptr)
	{
		((CFillPattern*)m_pPattern)->m_nAngle = (unsigned short)m_nAngle;
	}
}

void CFillPatternDlg::OnChangeAngle()
{
	const int nItem = m_comboFile.GetCurSel();
	if(nItem==-1)
		return;

	GetParent()->SendMessage(WM_REDRAWPREVIEW);
}
