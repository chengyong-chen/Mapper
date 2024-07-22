#include "stdafx.h"
#include "TextStyleDlg1.h"
#include "afxdialogex.h"
#include "Color.h"
#include "TypeDlg.h"
#include "TextStyle.h"
// CTextStyleDlg1 dialog

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CTextStyleDlg1, CDialogEx)

CTextStyleDlg1::CTextStyleDlg1(CWnd* pParent, TextFrame& frame)
	: CDialogEx(CTextStyleDlg1::IDD, pParent), m_frame(frame)
{
}

CTextStyleDlg1::~CTextStyleDlg1()
{
}

void CTextStyleDlg1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextStyleDlg0)
	DDX_Text(pDX, IDC_BORDWIDTH, m_frame.m_widthBorder);
	DDV_MinMaxInt(pDX, m_frame.m_widthBorder, 0, 100);
	DDX_Check(pDX, IDC_ROUNDRECT, m_frame.m_bRoundCorner);
	DDX_Check(pDX, IDC_GROUND, m_frame.m_bBackground);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTextStyleDlg1, CDialogEx)
	ON_BN_CLICKED(IDC_GROUNDCOLOR, OnColorBnClicked)
	ON_BN_CLICKED(IDC_BORDCOLOR, OnColorBnClicked)
	ON_BN_CLICKED(IDC_ROUNDRECT, OnCheckBnClicked)
	ON_BN_CLICKED(IDC_GROUND, OnCheckBnClicked)
END_MESSAGE_MAP()

BOOL CTextStyleDlg1::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	((CMFCColorButton*)GetDlgItem(IDC_BORDCOLOR))->SetColor(CColor::ARGBtoRGB(m_frame.m_colorBorder));
	((CMFCColorButton*)GetDlgItem(IDC_GROUNDCOLOR))->SetColor(CColor::ARGBtoRGB(m_frame.m_colorBackground));
	return TRUE;
}

void CTextStyleDlg1::OnColorBnClicked()
{
	m_frame.m_colorBorder = CColor::RGBtoARGB(((CMFCColorButton*)GetDlgItem(IDC_BORDCOLOR))->GetColor());
	m_frame.m_colorBackground = CColor::RGBtoARGB(((CMFCColorButton*)GetDlgItem(IDC_GROUNDCOLOR))->GetColor());
	((CHintDlg*)GetParent())->OnContextChanged();
}

void CTextStyleDlg1::OnCheckBnClicked()
{
	((CHintDlg*)GetParent())->OnContextChanged();
}
