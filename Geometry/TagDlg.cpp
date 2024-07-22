#include "stdafx.h"
#include "TagDlg.h"

#include "Geom.h"
#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTagDlg dialog

CTagDlg::CTagDlg(CWnd* pParent, ANCHOR& anchor, HALIGN& hAlign, VALIGN& vAlign)
	: CDialog(CTagDlg::IDD, pParent), m_anchor(anchor), m_hAlign(hAlign), m_vAlign(vAlign)
{
	//{{AFX_DATA_INIT(CTagDlg)
	//}}AFX_DATA_INIT
}

void CTagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTagDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTagDlg, CDialog)
	//{{AFX_MSG_MAP(CTagDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTagDlg message handlers

BOOL CTagDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	switch(m_anchor)
	{
	case ANCHOR_1:
		((CButton*)GetDlgItem(IDC_ANCHOR1))->SetCheck(1);
		break;
	case ANCHOR_2:
		((CButton*)GetDlgItem(IDC_ANCHOR2))->SetCheck(1);
		break;
	case ANCHOR_3:
		((CButton*)GetDlgItem(IDC_ANCHOR3))->SetCheck(1);
		break;
	case ANCHOR_4:
		((CButton*)GetDlgItem(IDC_ANCHOR4))->SetCheck(1);
		break;
	case ANCHOR_5:
		((CButton*)GetDlgItem(IDC_ANCHOR5))->SetCheck(1);
		break;
	case ANCHOR_6:
		((CButton*)GetDlgItem(IDC_ANCHOR6))->SetCheck(1);
		break;
	case ANCHOR_7:
		((CButton*)GetDlgItem(IDC_ANCHOR7))->SetCheck(1);
		break;
	case ANCHOR_8:
		((CButton*)GetDlgItem(IDC_ANCHOR8))->SetCheck(1);
		break;
	case ANCHOR_9:
		((CButton*)GetDlgItem(IDC_ANCHOR9))->SetCheck(1);
		break;
	default:
		((CButton*)GetDlgItem(IDC_ANCHOR1))->SetCheck(1);
		break;
	}
	switch(m_hAlign)
	{
	case HALIGN::HA_LEFT:
		((CButton*)GetDlgItem(IDC_HLEFT))->SetCheck(1);
		break;
	case HALIGN::HA_CENTER:
		((CButton*)GetDlgItem(IDC_HCENTER))->SetCheck(1);
		break;
	case HALIGN::HA_RIGHT:
		((CButton*)GetDlgItem(IDC_HRIGHT))->SetCheck(1);
		break;
	default:
		((CButton*)GetDlgItem(IDC_HLEFT))->SetCheck(1);
		break;
	}
	switch(m_vAlign)
	{
	case VALIGN::VA_BOTTOM:
		((CButton*)GetDlgItem(IDC_VBOTTOM))->SetCheck(1);
		break;
	case VALIGN::VA_CENTER:
		((CButton*)GetDlgItem(IDC_VCENTER))->SetCheck(1);
		break;
	case VALIGN::VA_TOP:
		((CButton*)GetDlgItem(IDC_HLEFT))->SetCheck(1);
		break;
	default:
		((CButton*)GetDlgItem(IDC_VTOP))->SetCheck(1);
		break;
	}

	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTagDlg::OnOK()
{
	CDialog::OnOK();

	m_anchor = ANCHOR::ANCHOR_0;
	if((((CButton*)GetDlgItem(IDC_ANCHOR1))->GetState()&0X0003)) m_anchor = ANCHOR::ANCHOR_1;
	if((((CButton*)GetDlgItem(IDC_ANCHOR2))->GetState()&0X0003)) m_anchor = ANCHOR::ANCHOR_2;
	if((((CButton*)GetDlgItem(IDC_ANCHOR3))->GetState()&0X0003)) m_anchor = ANCHOR::ANCHOR_3;
	if((((CButton*)GetDlgItem(IDC_ANCHOR4))->GetState()&0X0003)) m_anchor = ANCHOR::ANCHOR_4;
	if((((CButton*)GetDlgItem(IDC_ANCHOR5))->GetState()&0X0003)) m_anchor = ANCHOR::ANCHOR_5;
	if((((CButton*)GetDlgItem(IDC_ANCHOR6))->GetState()&0X0003)) m_anchor = ANCHOR::ANCHOR_6;
	if((((CButton*)GetDlgItem(IDC_ANCHOR7))->GetState()&0X0003)) m_anchor = ANCHOR::ANCHOR_7;
	if((((CButton*)GetDlgItem(IDC_ANCHOR8))->GetState()&0X0003)) m_anchor = ANCHOR::ANCHOR_8;
	if((((CButton*)GetDlgItem(IDC_ANCHOR9))->GetState()&0X0003)) m_anchor = ANCHOR::ANCHOR_9;

	if((((CButton*)GetDlgItem(IDC_HLEFT))->GetState()&0X0003)) m_hAlign = HALIGN::HA_LEFT;
	if((((CButton*)GetDlgItem(IDC_HCENTER))->GetState()&0X0003)) m_hAlign = HALIGN::HA_CENTER;
	if((((CButton*)GetDlgItem(IDC_HRIGHT))->GetState()&0X0003)) m_hAlign = HALIGN::HA_RIGHT;

	if((((CButton*)GetDlgItem(IDC_VBOTTOM))->GetState()&0X0003)) m_vAlign = VALIGN::VA_BOTTOM;
	if((((CButton*)GetDlgItem(IDC_VCENTER))->GetState()&0X0003)) m_vAlign = VALIGN::VA_CENTER;
	if((((CButton*)GetDlgItem(IDC_VTOP))->GetState()&0X0003)) m_vAlign = VALIGN::VA_TOP;
}
