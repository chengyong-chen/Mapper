#include "stdafx.h"
#include "TextStyleDlg2.h"
#include "afxdialogex.h"
#include "Color.h"
#include "TextStyle.h"
#include "TypeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CTextStyleDlg2, CDialogEx)

CTextStyleDlg2::CTextStyleDlg2(CWnd* pParent, TextOutline& outline)
	: CDialogEx(CTextStyleDlg2::IDD, pParent), m_outline(outline)
{
}

CTextStyleDlg2::~CTextStyleDlg2()
{
}

void CTextStyleDlg2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextStyleDlg0)
	DDX_Text(pDX, IDC_BORDWIDTH, m_outline.m_width);
	DDV_MinMaxInt(pDX, m_outline.m_width, 0.1, 100);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTextStyleDlg2, CDialogEx)
	ON_EN_CHANGE(IDC_BORDWIDTH, OnContextChanged)
	ON_BN_CLICKED(IDC_BORDCOLOR, OnContextChanged)
END_MESSAGE_MAP()

BOOL CTextStyleDlg2::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	((CMFCColorButton*)GetDlgItem(IDC_BORDCOLOR))->SetColor(CColor::ARGBtoRGB(m_outline.m_color));

	return TRUE;
}

void CTextStyleDlg2::OnContextChanged()
{
	m_outline.m_color = CColor::RGBtoARGB(((CMFCColorButton*)GetDlgItem(IDC_BORDCOLOR))->GetColor());
	((CHintDlg*)GetParent())->OnContextChanged();
}
