#include "stdafx.h"

#include "Geom.h"
#include "TextPoly.h"
#include "TextPolyDlg.h"
#include "TextEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextPolyDlg dialog

CTextPolyDlg::CTextPolyDlg(CWnd* pParent, DWORD dwStyle)
	: CDialog(CTextPolyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextPolyDlg)

	//}}AFX_DATA_INIT
	m_dwStyle = dwStyle;
}

void CTextPolyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextPolyDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTextPolyDlg, CDialog)
	//{{AFX_MSG_MAP(CTextPolyDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextPolyDlg message handlers

BOOL CTextPolyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	switch(m_dwStyle&0X000FL)
	{
	case ALIGN_BASELINE:
		((CButton*)GetDlgItem(IDC_BASELINE))->SetCheck(TRUE);
		break;
	case ALIGN_CENTER:
		((CButton*)GetDlgItem(IDC_CENTER))->SetCheck(TRUE);
		break;
	case 2:
		((CButton*)GetDlgItem(IDC_DECENT))->SetCheck(TRUE);
		break;
	}

	switch(m_dwStyle&0X00F0L)
	{
	case ORIENT_VERTICAL:
		((CButton*)GetDlgItem(IDC_VERTICAL))->SetCheck(TRUE);
		break;
	case ORIENT_PARALLEL:
		((CButton*)GetDlgItem(IDC_PARALLEL))->SetCheck(TRUE);
		break;
	}

	switch(m_dwStyle&0X0F00L)
	{
	case IDC_ANCHOR:
		((CButton*)GetDlgItem(IDC_ANCHOR))->SetCheck(TRUE);
		break;
	case SPACE_AVERAGE:
		((CButton*)GetDlgItem(IDC_AVERAGE))->SetCheck(TRUE);
		break;
	}

	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTextPolyDlg::OnOK()
{
	CDialog::OnOK();

	m_dwStyle = 0;

	if((((CButton*)GetDlgItem(IDC_BASELINE))->GetState()&0X0003)) m_dwStyle |= ALIGN_BASELINE;
	if((((CButton*)GetDlgItem(IDC_CENTER))->GetState()&0X0003)) m_dwStyle |= ALIGN_CENTER;
	if((((CButton*)GetDlgItem(IDC_DECENT))->GetState()&0X0003)) m_dwStyle |= ALIGN_DECENT;

	if((((CButton*)GetDlgItem(IDC_VERTICAL))->GetState()&0X0003)) m_dwStyle |= ORIENT_VERTICAL;
	if((((CButton*)GetDlgItem(IDC_PARALLEL))->GetState()&0X0003)) m_dwStyle |= ORIENT_PARALLEL;

	if((((CButton*)GetDlgItem(IDC_ANCHOR))->GetState()&0X0003)) m_dwStyle |= SPACE_ANCHOR;
	if((((CButton*)GetDlgItem(IDC_AVERAGE))->GetState()&0X0003)) m_dwStyle |= SPACE_AVERAGE;
}
