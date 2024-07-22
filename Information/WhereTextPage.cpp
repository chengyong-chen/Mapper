#include "stdafx.h"
#include "WhereTextPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWhereTextPage dialog
IMPLEMENT_DYNCREATE(CWhereTextPage, CPropertyPage)

CWhereTextPage::CWhereTextPage()
	: CPropertyPage(CWhereTextPage::IDD)
{
	//{{AFX_DATA_INIT(CWhereTextPage)
	//}}AFX_DATA_INIT
}

void CWhereTextPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWhereTextPage)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWhereTextPage, CPropertyPage)
	//{{AFX_MSG_MAP(CWhereTextPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWhereTextPage message handlers

/////////////////////////////////////////////////////////////////////////////
// CWhereTextPage message handlers
BOOL CWhereTextPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWhereTextPage::OnSetActive()
{
	if(m_strWhere!=nullptr)
	{
		const CStringW strWhere = *m_strWhere;
		GetDlgItem(IDC_WHEREEDIT)->SetWindowText(strWhere);
	}

	return CPropertyPage::OnSetActive();
}

BOOL CWhereTextPage::OnKillActive()
{
	if(m_strWhere!=nullptr)
	{
		CString strWhere;
		GetDlgItem(IDC_WHEREEDIT)->GetWindowText(strWhere);
		*m_strWhere = strWhere;
	}

	return CPropertyPage::OnKillActive();
}

BOOL CWhereTextPage::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
		case VK_RETURN:
			return TRUE;
			break;
		}
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}
