#include "stdafx.h"

#include "Type.h"
#include "TypeDlg.h"
#include "TextStyleDlg0.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTypeDlg::CTypeDlg(CWnd* pParent, CHint& type)
	: CHintDlg(pParent, type)
{
	//{{AFX_DATA_INIT(CTypeDlg)
	//}}AFX_DATA_INIT

	m_pTextStyleDlg0 = nullptr;
}

CTypeDlg::~CTypeDlg()
{
}

void CTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CHintDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTypeDlg)
	DDX_Text(pDX, IDC_HORZSCALE, ((CType&)m_hint).m_deform.m_nHorzscale);
	DDV_MinMaxInt(pDX, ((CType&)m_hint).m_deform.m_nHorzscale, 20, 200);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTypeDlg, CHintDlg)
	//{{AFX_MSG_MAP(CTypeDlg)	
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTypeDlg message handlers

BOOL CTypeDlg::OnInitDialog()
{
	CHintDlg::OnInitDialog();

	CRect rect;
	((CWnd*)GetDlgItem(IDC_HOLDER))->GetWindowRect(&rect);
	this->ScreenToClient(rect);
	CTextStyleDlg0* pTextStyleDlg0 = new CTextStyleDlg0((CWnd*)this, ((CType&)m_hint).m_deform);
	pTextStyleDlg0->Create(IDD_TEXTSTYLE0, (CWnd*)this);
	pTextStyleDlg0->SetWindowPos(&wndTop, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW|SWP_NOSIZE);
	m_pTextStyleDlg0 = pTextStyleDlg0;

	CRect rect1;
	GetDlgItem(IDC_GROUPBOX)->GetWindowRect(&rect1);
	this->ScreenToClient(rect1);
	rect1.OffsetRect(0, rect.Height());
	GetDlgItem(IDC_GROUPBOX)->MoveWindow(rect1, 1);

	CRect rect2;
	GetDlgItem(IDC_FILTERS)->GetWindowRect(&rect2);
	this->ScreenToClient(rect2);
	rect2.OffsetRect(0, rect.Height());
	GetDlgItem(IDC_FILTERS)->MoveWindow(rect2, 1);

	CRect rect3;
	GetDlgItem(IDC_PREVIEW)->GetWindowRect(&rect3);
	this->ScreenToClient(rect3);
	rect3.OffsetRect(0, rect.Height());
	GetDlgItem(IDC_PREVIEW)->MoveWindow(rect3, 1);

	CRect rect4;
	GetDlgItem(IDOK)->GetWindowRect(&rect4);
	this->ScreenToClient(rect4);
	rect4.OffsetRect(0, rect.Height());
	GetDlgItem(IDOK)->MoveWindow(rect4, 1);

	CRect rect5;
	GetDlgItem(IDCANCEL)->GetWindowRect(&rect5);
	this->ScreenToClient(rect5);
	rect5.OffsetRect(0, rect.Height());
	GetDlgItem(IDCANCEL)->MoveWindow(rect5, 1);

	CRect rect6;
	this->GetWindowRect(&rect6);
	//	this->ScreenToClient(rect6);
	rect6.bottom += rect.Height();
	this->SetWindowPos(&wndTop, rect6.left, rect6.top, rect6.Width(), rect6.Height(), SWP_SHOWWINDOW);

	if(m_pTextStyleDlgx!=nullptr)
	{
		CRect rect7;
		m_pTextStyleDlgx->GetWindowRect(&rect7);
		this->ScreenToClient(rect7);
		rect7.OffsetRect(0, rect.Height());
		m_pTextStyleDlgx->MoveWindow(rect7, 1);
	}
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTypeDlg::OnDestroy()
{
	if(m_pTextStyleDlg0!=nullptr)
	{
		m_pTextStyleDlg0->PostMessage(WM_DESTROY, 0, 0);
		m_pTextStyleDlg0->DestroyWindow();
		delete m_pTextStyleDlg0;
		m_pTextStyleDlg0 = nullptr;
	}

	CHintDlg::OnDestroy();
}

void CTypeDlg::OnContextChanged()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(m_pTextStyleDlg0!=nullptr)
	{
		m_pTextStyleDlg0->UpdateData();
	}

	CHintDlg::OnContextChanged();
}
