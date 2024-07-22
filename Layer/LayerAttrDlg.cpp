#include "stdafx.h"
#include "Layer.h"
#include "LayerAttrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayerAttrDlg dialog

CLayerAttrDlg::CLayerAttrDlg(CWnd* pParent /*=nullptr*/, const WORD wId, BYTE bLevel, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag, BOOL bKeyQuery, bool bLocked, bool bHide, bool bShowTag, BYTE bDynamic, bool bDetour, bool bPivot, bool bCentroid, CStringA strGeoCatogery, bool bTagResistance)
	: CDialog(CLayerAttrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLayerAttrDlg)
	m_wId = 0;
	m_minLevelObj = 0;
	m_maxLevelObj = 20;
	m_minLevelTag = 0;
	m_maxLevelTag = 20;
	m_bKeyQuery = FALSE;
	m_bLocked = FALSE;
	m_bShowTag = FALSE;
	m_bPivot = FALSE;
	m_bCentroid = FALSE;
	m_dwLevel = 0;
	m_bTagResistance = FALSE;
	//}}AFX_DATA_INIT

	m_wId = wId;
	m_minLevelObj = minLevelObj;
	m_maxLevelObj = maxLevelObj;
	m_minLevelTag = minLevelTag;
	m_maxLevelTag = maxLevelTag;
	m_bKeyQuery = bKeyQuery;
	m_bLocked = bLocked;
	m_bHide = bHide;
	m_bShowTag = bShowTag;
	m_bSpotDynamic = (bDynamic&SpotDynamic)==SpotDynamic ? true : false;
	m_bLineDynamic = (bDynamic&LineDynamic)==LineDynamic ? true : false;
	m_bFillDynamic = (bDynamic&FillDynamic)==FillDynamic ? true : false;
	m_bTypeDynamic = (bDynamic&TypeDynamic)==TypeDynamic ? true : false;
	m_bDetour = bDetour;
	m_bPivot = bPivot;
	m_dwLevel = bLevel;
	m_bCentroid = bCentroid;
	m_strGeoCatogery = strGeoCatogery;
	m_bTagResistance = bTagResistance;
}

void CLayerAttrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLayerAttrDlg)
	DDX_Check(pDX, IDC_QUERYNAME, m_bKeyQuery);
	DDX_Check(pDX, IDC_LOCKED, m_bLocked);
	DDX_Check(pDX, IDC_HIDE, m_bHide);
	DDX_Check(pDX, IDC_SHOWTIP, m_bShowTag);
	DDX_Check(pDX, IDC_SPOTDYNAMIC, m_bSpotDynamic);
	DDX_Check(pDX, IDC_LINEDYNAMIC, m_bLineDynamic);
	DDX_Check(pDX, IDC_FILLDYNAMIC, m_bFillDynamic);
	DDX_Check(pDX, IDC_TYPEDYNAMIC, m_bTypeDynamic);
	DDX_Check(pDX, IDC_DETOUR, m_bDetour);
	DDX_Check(pDX, IDC_PIVOT, m_bPivot);
	DDX_Check(pDX, IDC_CENTROID, m_bCentroid);
	DDX_Text(pDX, IDC_GEOCATOGERY, m_strGeoCatogery);
	DDX_Check(pDX, IDC_TAGRESISTANCE, m_bTagResistance);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLayerAttrDlg, CDialog)
	//{{AFX_MSG_MAP(CLayerAttrDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayerAttrDlg message handlers
void CLayerAttrDlg::DDX_CBData(CDataExchange* pDX, int nIDC, DWORD& dwData)
{
	const HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	if(pDX->m_bSaveAndValidate)
	{
		const int nSelected = ::SendMessage(hWndCtrl, CB_GETCURSEL, 0, 0L);
		dwData = ::SendMessage(hWndCtrl, CB_GETITEMDATA, nSelected, 0L);
	}
	else
	{
		const int nCount = ::SendMessage(hWndCtrl, CB_GETCOUNT, 0, 0L);
		for(int i = 0; i<nCount; i++)
		{
			const DWORD dwItemData = ::SendMessage(hWndCtrl, CB_GETITEMDATA, i, 0L);
			if(dwItemData==dwData)
			{
				::SendMessage(hWndCtrl, CB_SETCURSEL, i, 0L);
				break;
			}
		}
	}
}

BOOL CLayerAttrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str;
	str.Format(_T("%.2f"), m_minLevelObj);
	str.TrimRight(_T("0"));
	str.TrimRight(_T("."));
	((CWnd*)GetDlgItem(IDC_MINIMUMLEVELOBJ))->SetWindowText(str);
	str.Format(_T("%.2f"), m_maxLevelObj);
	str.TrimRight(_T("0"));
	str.TrimRight(_T("."));
	((CWnd*)GetDlgItem(IDC_MAXIMUMLEVELOBJ))->SetWindowText(str);

	str.Format(_T("%.2f"), m_minLevelTag);
	str.TrimRight(_T("0"));
	str.TrimRight(_T("."));
	((CWnd*)GetDlgItem(IDC_MINIMUMLEVELTAG))->SetWindowText(str);
	str.Format(_T("%.2f"), m_maxLevelTag);
	str.TrimRight(_T("0"));
	str.TrimRight(_T("."));
	((CWnd*)GetDlgItem(IDC_MAXIMUMLEVELTAG))->SetWindowText(str);

	return TRUE;
}

void CLayerAttrDlg::OnOK()
{
	CDialog::OnOK();

	CString str;
	((CWnd*)GetDlgItem(IDC_MINIMUMLEVELOBJ))->GetWindowText(str);
	m_minLevelObj = _ttof(str);
	((CWnd*)GetDlgItem(IDC_MAXIMUMLEVELOBJ))->GetWindowText(str);
	m_maxLevelObj = _ttof(str);
	((CWnd*)GetDlgItem(IDC_MINIMUMLEVELTAG))->GetWindowText(str);
	m_minLevelTag = _ttof(str);
	((CWnd*)GetDlgItem(IDC_MAXIMUMLEVELTAG))->GetWindowText(str);
	m_maxLevelTag = _ttof(str);
}
