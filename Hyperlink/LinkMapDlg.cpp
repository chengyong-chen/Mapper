#include "stdafx.h"
#include "Link.h"
#include "LinkMap.h"
#include "LinkMapDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL AFXAPI AfxFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);

CLinkMapDlg::CLinkMapDlg(CWnd* pParent /*=nullptr*/, CLinkMap* plink)
	: CDialog(CLinkMapDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinkMapDlg)
	m_strMap = _T("");
	m_uZoom = 0;
	m_nX = 0;
	m_nY = 0;
	//}}AFX_DATA_INIT

	d_pLink = plink;
	m_strMap = plink->m_strTarget;
	m_nX = plink->m_Anchor.x;
	m_nY = plink->m_Anchor.y;
	m_uZoom = plink->m_nScale;
}

void CLinkMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinkMapDlg)
	DDX_Text(pDX, IDC_MAP, m_strMap);
	DDX_Text(pDX, IDC_ZOOM, m_uZoom);
	DDV_MinMaxInt(pDX, m_uZoom, 10, 1000);
	DDX_Text(pDX, IDC_ANCHORX, m_nX);
	DDX_Text(pDX, IDC_ANCHORY, m_nY);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLinkMapDlg, CDialog)
	//{{AFX_MSG_MAP(CLinkMapDlg)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkMapDlg message handlers

BOOL CLinkMapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CButton*)GetDlgItem(IDC_AUTARCHY))->SetCheck(d_pLink->m_bAutarchy);
	SetWindowPos(&wndTop, 10, 63, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	return TRUE;
}

void CLinkMapDlg::OnOK()
{
	CDialog::OnOK();

	TCHAR szPath[_MAX_PATH];
	AfxFullPath(szPath, m_strMap);
	m_strMap = szPath;

	d_pLink->m_strTarget = m_strMap;
	d_pLink->m_Anchor = CPoint(m_nX, m_nY);
	d_pLink->m_nScale = m_uZoom;

	d_pLink->m_bAutarchy = ((CButton*)GetDlgItem(IDC_AUTARCHY))->GetCheck();
}

void CLinkMapDlg::OnSearch()
{
	TCHAR CurrentDir[256];
	GetCurrentDirectory(255, CurrentDir);

	CString strPath = AfxGetApp()->GetProfileString(_T(""), _T("Link Map Path"), nullptr);
	SetCurrentDirectory(strPath);

	CFileDialog dlg(TRUE,
		nullptr,
		nullptr,
		OFN_HIDEREADONLY,
		_T("Diwatu Map File(*.Grf,*.Gis,*.Gis)|*.Grf;*.Gis;*.Mif;*.Grf||"),
		this);

	if(dlg.DoModal()==IDOK)
	{
		const CString strExt = dlg.GetFileExt();
		if(strExt.CompareNoCase(_T("MAP"))!=0&&strExt.CompareNoCase(_T("GIS"))!=0&&strExt.CompareNoCase(_T("MIF"))!=0)
		{
			AfxMessageBox(_T("The file isn't a Diwatu map file!"));
			return;
		}

		strPath = dlg.GetPathName();

		((CStatic*)GetDlgItem(IDC_MAP))->SetWindowText(strPath);
		m_strMap = strPath;

		strPath = strPath.Left(strPath.ReverseFind(_T('\\'))+1);
		AfxGetApp()->WriteProfileString(_T(""), _T("Link Map Path"), strPath);
	}

	SetCurrentDirectory(CurrentDir);
}
