#include "stdafx.h"

#include "Atlas1.h"
#include "AtlasDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAtlasDlg dialog

CAtlasDlg::CAtlasDlg(CWnd* pParent, CAtlas* pAtlas)
	: CDialog(CAtlasDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAtlasDlg)
	m_bHtml = FALSE;
	m_bNavigate = FALSE;
	m_bFree = FALSE;
	m_bEdit = TRUE;
	m_bRegionQuery = TRUE;
	m_strIndexHtml = _T("");
	m_strTitle = _T("");
	m_strHomePage = _T("");
	//}}AFX_DATA_INIT

	if(pAtlas!=nullptr)
	{
		m_strName = pAtlas->m_strName;
		m_strTitle = pAtlas->m_strTitle;
		m_strIndexHtml = pAtlas->m_strIndexHtml;
		m_strHomePage = pAtlas->m_strHomePage;

		m_bFree = pAtlas->m_bFree;
		m_bEdit = pAtlas->m_bEdit;
		m_bRegionQuery = pAtlas->m_bRegionQuery;
		m_bNavigate = pAtlas->m_bNavigate;
		m_bHtml = pAtlas->m_bHtml;

		m_pAtlas = pAtlas;
	}

	m_pAtlas = pAtlas;;
}

void CAtlasDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAtlasDlg)
	DDX_Check(pDX, IDC_HTML, m_bHtml);
	DDX_Check(pDX, IDC_NAVIGATE, m_bNavigate);
	DDX_Check(pDX, IDC_DATABASE, m_bDatabase);
	DDX_Check(pDX, IDC_OPEN, m_bFree);
	DDX_Check(pDX, IDC_EDIT, m_bEdit);
	DDX_Check(pDX, IDC_REGIONQUERY, m_bRegionQuery);

	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Text(pDX, IDC_TITLE, m_strTitle);
	DDX_Text(pDX, IDC_INDEXHTML, m_strIndexHtml);
	DDX_Text(pDX, IDC_HOMEPAGE, m_strHomePage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAtlasDlg, CDialog)
	//{{AFX_MSG_MAP(CAtlasDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAtlasDlg message handlers

void CAtlasDlg::OnOK()
{
	CDialog::OnOK();

	if(m_pAtlas!=nullptr)
	{
		m_pAtlas->m_strName = m_strName;
		m_pAtlas->m_strTitle = m_strTitle;
		m_pAtlas->m_strIndexHtml = m_strIndexHtml;
		m_pAtlas->m_strHomePage = m_strHomePage;

		m_pAtlas->m_bFree = m_bFree;
		m_pAtlas->m_bEdit = m_bEdit;
		m_pAtlas->m_bRegionQuery = m_bRegionQuery;
		m_pAtlas->m_bNavigate = m_bNavigate;
		m_pAtlas->m_bHtml = m_bHtml;
	}
}
