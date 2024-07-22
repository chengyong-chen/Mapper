#include "stdafx.h"

#include "FillSymbol.h"
#include "Symbol.h"
#include "SymbolFill.h"
#include "FillSymbolDlg.h"
#include "SymbolMid.hpp"
#include "Midtable.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFillSymbolDlg dialog

CFillSymbolDlg::CFillSymbolDlg(CWnd* pParent, CMidtable<CSymbolMid<CSymbolFill>>& tagtable, CFillSymbol* pFill)
	: TSymbolMidDlg(CFillSymbolDlg::IDD, pParent, tagtable, pFill)
{
	//{{AFX_DATA_INIT(CFillSymbolDlg)
	//}}AFX_DATA_INIT
	m_nAngle = pFill==nullptr ? 0 : pFill->m_nAngle;
}

void CFillSymbolDlg::DoDataExchange(CDataExchange* pDX)
{
	TSymbolMidDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpotFlashDlg)
	DDX_Text(pDX, IDC_ANGLE, m_nAngle);
	DDV_MinMaxShort(pDX, m_nAngle, 0, 180);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFillSymbolDlg, TSymbolMidDlg)
	//{{AFX_MSG_MAP(CFillSymbolDlg)
	ON_EN_CHANGE(IDC_ANGLE, OnChangeAngle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFillSymbolDlg message handlers

BOOL CFillSymbolDlg::OnInitDialog()
{
	TSymbolMidDlg::OnInitDialog();

	SetWindowPos(&wndTop, 8, 45, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFillSymbolDlg::OnOK()
{
	TSymbolMidDlg::OnOK();

	if(m_pXSymbol!=nullptr)
	{
		m_pXSymbol->m_nAngle = (unsigned short)m_nAngle;
	}
}

void CFillSymbolDlg::OnChangeAngle()
{
	const int nItem = m_comboFile.GetCurSel();
	if(nItem==-1)
		return;

	TSymbolMidDlg::DrawPreview();
}
