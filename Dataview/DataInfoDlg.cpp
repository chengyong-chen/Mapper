#include "stdafx.h"
#include "DatainfoDlg.h"
#include "Datainfo.h"

#include "../Projection/Projection1.h"
#include "../Projection/Geocentric.h"
#include "../Projection/Geographic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDatainfoDlg dialog
CDatainfoDlg::PAPERSIZE CDatainfoDlg::g_paper[] =
{
	{_T("Custom"), 0, 0},
	{_T("A4"), 595, 842},
	{_T("A3"), 842, 1190},
	{_T("A2"), 1191, 1684},
	{_T("A1"), 1684, 2384},
	{_T("A0"), 2384, 3370},
	{_T("B4"), 729, 1032},
	{_T("B3"), 1032, 1460},
	{_T("B2"), 1460, 2064},
	{_T("B1"), 2064, 2920},
	{_T("B0"), 2920, 4127},
};

CDatainfoDlg::CDatainfoDlg(CWnd* pParent, CDatainfo& datainfo)
	: CDialog(CDatainfoDlg::IDD, pParent), m_Datainfo(datainfo)
{
	//{{AFX_DATA_INIT(CDatainfoDlg)
	m_strName = _T("");
	m_xOrigin = datainfo.m_pProjection==nullptr ? datainfo.m_mapOrigin.x : datainfo.m_pProjection->MapToUser(datainfo.m_mapOrigin.x);
	m_yOrigin = datainfo.m_pProjection==nullptr ? datainfo.m_mapOrigin.y : datainfo.m_pProjection->MapToUser(datainfo.m_mapOrigin.y);
	m_cxCanvas = datainfo.m_pProjection==nullptr ? datainfo.m_mapCanvas.cx : datainfo.m_pProjection->MapToUser(datainfo.m_mapCanvas.cx);
	m_cyCanvas = datainfo.m_pProjection==nullptr ? datainfo.m_mapCanvas.cy : datainfo.m_pProjection->MapToUser(datainfo.m_mapCanvas.cy);

	m_scaleMinimum = datainfo.m_scaleMinimum;
	m_scaleSource = datainfo.m_scaleSource;
	m_scaleMaximum = datainfo.m_scaleMaximum;
	//}}AFX_DATA_INIT
	m_strName = datainfo.m_strName;
	m_pProjection = nullptr;
}

void CDatainfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDatainfoDlg)
	DDX_Text(pDX, IDC_NAME, m_strName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDatainfoDlg, CDialog)
	//{{AFX_MSG_MAP(CDatainfoDlg)
	ON_CBN_SELCHANGE(IDC_PROJECTIONTYPECOMBO, OnProjectionTypeChange)
	ON_CBN_SELCHANGE(IDC_GCSCOMBO, OnGCSSelchange)
	ON_CBN_SELCHANGE(IDC_PAPERCOMBO, OnSelchangePapercombo)
	ON_BN_CLICKED(IDC_PORTRAITRADIO, OnPortraitradio)
	ON_BN_CLICKED(IDC_LANDSCAPERADIO, OnLandscaperadio)
	ON_BN_CLICKED(IDC_SETUPPROJECTION, OnSetupProjection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDatainfoDlg message handlers

BOOL CDatainfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	for(long I = 0; I<sizeof(g_paper)/sizeof(PAPERSIZE); I++)
	{
		((CComboBox*)GetDlgItem(IDC_PAPERCOMBO))->AddString(g_paper[I].Name);
	}

	((CComboBox*)GetDlgItem(IDC_PROJECTIONTYPECOMBO))->AddString(_T("None"));
	((CComboBox*)GetDlgItem(IDC_PROJECTIONTYPECOMBO))->SetItemData(0, 0XFFFFFFFF);
	((CComboBox*)GetDlgItem(IDC_PROJECTIONTYPECOMBO))->AddString(_T("Geographic"));
	((CComboBox*)GetDlgItem(IDC_PROJECTIONTYPECOMBO))->SetItemData(1, 0);

	CGeocentric::ProjType* pProjTypes = CGeocentric::GetProjTypes();
	for(int index = 0; pProjTypes[index].key!=nullptr; index++)
	{
		const int item = ((CComboBox*)GetDlgItem(IDC_PROJECTIONTYPECOMBO))->AddString(CString(pProjTypes[index].name));
		((CComboBox*)GetDlgItem(IDC_PROJECTIONTYPECOMBO))->SetItemData(item, (DWORD)&pProjTypes[index]);
	}

	Spheroid* pSpheroidList = CProjection::GetSpheroidList();
	for(int index = 0; pSpheroidList[index].key!=nullptr; index++)
	{
		const int item = ((CComboBox*)GetDlgItem(IDC_GCSCOMBO))->AddString(CString(pSpheroidList[index].name));
		((CComboBox*)GetDlgItem(IDC_GCSCOMBO))->SetItemData(item, (DWORD)&pSpheroidList[index]);
	}

	if(m_Datainfo.m_pProjection==nullptr)
	{
		((CComboBox*)GetDlgItem(IDC_PAPERCOMBO))->SetCurSel(0);

		for(long I = 0; I<sizeof g_paper/sizeof(PAPERSIZE); I++)
		{
			if(g_paper[I].width==m_cxCanvas&&g_paper[I].height==m_cyCanvas||g_paper[I].height==m_cxCanvas&&g_paper[I].width==m_cyCanvas)
			{
				((CComboBox*)GetDlgItem(IDC_PAPERCOMBO))->SetCurSel(I);
				break;
			}
		}
	}

	if(m_Datainfo.m_strType=="Grf")
	{
		delete m_Datainfo.m_pProjection;
		m_Datainfo.m_pProjection = nullptr;

		GetDlgItem(IDC_PROJECTIONTYPECOMBO)->EnableWindow(FALSE);
	}
	else if(m_Datainfo.m_strType=="Geo")
	{
		GetDlgItem(IDC_PROJECTIONTYPECOMBO)->EnableWindow(FALSE);
	}

	if(m_Datainfo.m_pProjection!=nullptr)
	{
		CProjection* pProjection = m_Datainfo.m_pProjection->Clone();
		ResetProject(pProjection);
	}
	else
	{
		ResetProject(nullptr);
	}

	m_xOrigin = m_Datainfo.m_pProjection==nullptr ? m_Datainfo.m_mapOrigin.x : m_Datainfo.m_pProjection->MapToUser(m_Datainfo.m_mapOrigin.x);
	m_yOrigin = m_Datainfo.m_pProjection==nullptr ? m_Datainfo.m_mapOrigin.y : m_Datainfo.m_pProjection->MapToUser(m_Datainfo.m_mapOrigin.y);
	m_cxCanvas = m_Datainfo.m_pProjection==nullptr ? m_Datainfo.m_mapCanvas.cx : m_Datainfo.m_pProjection->MapToUser(m_Datainfo.m_mapCanvas.cx);
	m_cyCanvas = m_Datainfo.m_pProjection==nullptr ? m_Datainfo.m_mapCanvas.cy : m_Datainfo.m_pProjection->MapToUser(m_Datainfo.m_mapCanvas.cy);

	m_scaleMinimum = m_Datainfo.m_scaleMinimum;
	m_scaleSource = m_Datainfo.m_scaleSource;
	m_scaleMaximum = m_Datainfo.m_scaleMaximum;

	if(m_cyCanvas>m_cxCanvas)
		((CButton*)GetDlgItem(IDC_PORTRAITRADIO))->SetCheck(TRUE);
	else
		((CButton*)GetDlgItem(IDC_LANDSCAPERADIO))->SetCheck(TRUE);

	UpdateHAndW();
	CenterWindow();
	return TRUE;
}

void CDatainfoDlg::OnSelchangePapercombo()
{
	const int index = ((CComboBox*)GetDlgItem(IDC_PAPERCOMBO))->GetCurSel();
	if(index!=0&&index!=CB_ERR)
	{
		m_cxCanvas = g_paper[index].width;
		m_cyCanvas = g_paper[index].height;
		UpdateHAndW();
	}
}

void CDatainfoDlg::OnPortraitradio()
{
	if(m_cyCanvas<m_cxCanvas)
	{
		const long t = m_cyCanvas;
		m_cyCanvas = m_cxCanvas;
		m_cxCanvas = t;

		UpdateHAndW();
	}
}

void CDatainfoDlg::OnLandscaperadio()
{
	if(m_cyCanvas>m_cxCanvas)
	{
		const long t = m_cxCanvas;
		m_cxCanvas = m_cyCanvas;
		m_cyCanvas = t;
		UpdateHAndW();
	}
}

void CDatainfoDlg::UpdateHAndW() const
{
	CString str;

	str.Format(_T("%.4f"), m_xOrigin);
	str.TrimRight(_T("0"));
	str.TrimRight(_T("."));
	((CWnd*)GetDlgItem(IDC_XORIGIN))->SetWindowText(str);
	str.Format(_T("%.4f"), m_yOrigin);
	str.TrimRight(_T("0"));
	str.TrimRight(_T("."));
	((CWnd*)GetDlgItem(IDC_YORIGIN))->SetWindowText(str);

	str.Format(_T("%.4f"), m_cxCanvas);
	str.TrimRight(_T("0"));
	str.TrimRight(_T("."));
	((CWnd*)GetDlgItem(IDC_CXCANVAS))->SetWindowText(str);
	str.Format(_T("%.4f"), m_cyCanvas);
	str.TrimRight(_T("0"));
	str.TrimRight(_T("."));
	((CWnd*)GetDlgItem(IDC_CYCANVAS))->SetWindowText(str);

	str.Format(_T("%.2f"), m_scaleMinimum);
	str.TrimRight(_T("0"));
	str.TrimRight(_T("."));
	((CWnd*)GetDlgItem(IDC_MINIMUMSCALE))->SetWindowText(str);
	str.Format(_T("%.2f"), m_scaleSource);
	str.TrimRight(_T("0"));
	str.TrimRight(_T("."));
	((CWnd*)GetDlgItem(IDC_SOURCESCALE))->SetWindowText(str);
	str.Format(_T("%.2f"), m_scaleMaximum);
	str.TrimRight(_T("0"));
	str.TrimRight(_T("."));
	((CWnd*)GetDlgItem(IDC_MAXIMUMSCALE))->SetWindowText(str);
}

void CDatainfoDlg::OnOK()
{
	CDialog::OnOK();

	m_Datainfo.m_strName = m_strName;

	CString str;
	((CWnd*)GetDlgItem(IDC_MINIMUMSCALE))->GetWindowText(str);
	m_Datainfo.m_scaleMinimum = _ttof(str);
	((CWnd*)GetDlgItem(IDC_SOURCESCALE))->GetWindowText(str);
	m_Datainfo.m_scaleSource = _ttof(str);
	((CWnd*)GetDlgItem(IDC_MAXIMUMSCALE))->GetWindowText(str);
	m_Datainfo.m_scaleMaximum = _ttof(str);

	((CWnd*)GetDlgItem(IDC_XORIGIN))->GetWindowText(str);
	double userOriginX = _ttof(str);
	((CWnd*)GetDlgItem(IDC_YORIGIN))->GetWindowText(str);
	double userOriginY = _ttof(str);
	((CWnd*)GetDlgItem(IDC_CXCANVAS))->GetWindowText(str);
	double userCanvasCX = _ttof(str);
	((CWnd*)GetDlgItem(IDC_CYCANVAS))->GetWindowText(str);
	double userCanvasCY = _ttof(str);


	m_Datainfo.m_mapOrigin.x = m_Datainfo.m_pProjection==nullptr ? userOriginX : m_Datainfo.m_pProjection->UserToMap(userOriginX);
	m_Datainfo.m_mapOrigin.y = m_Datainfo.m_pProjection==nullptr ? userOriginY : m_Datainfo.m_pProjection->UserToMap(userOriginY);
	m_Datainfo.m_mapCanvas.cx = m_Datainfo.m_pProjection==nullptr ? userCanvasCX : m_Datainfo.m_pProjection->UserToMap(userCanvasCX);
	m_Datainfo.m_mapCanvas.cy = m_Datainfo.m_pProjection==nullptr ? userCanvasCY : m_Datainfo.m_pProjection->UserToMap(userCanvasCY);

	if(m_pProjection!=nullptr)
	{
		m_pProjection->Reconfig();
	}
	m_Datainfo.ResetProjection(m_pProjection, false);
	m_pProjection = nullptr;
}

void CDatainfoDlg::OnProjectionTypeChange()
{
	const int index1 = ((CComboBox*)GetDlgItem(IDC_PROJECTIONTYPECOMBO))->GetCurSel();
	if(index1==-1)
		return;

	const DWORD data = ((CComboBox*)GetDlgItem(IDC_PROJECTIONTYPECOMBO))->GetItemData(index1);
	CProjection* pProjection = nullptr;
	switch(data)
	{
		case 0XFFFFFFFF:
			break;
		case 0:
			{
				const long index2 = ((CComboBox*)GetDlgItem(IDC_GCSCOMBO))->GetCurSel();
				if(index2==-1)
				{
					((CComboBox*)GetDlgItem(IDC_GCSCOMBO))->SetCurSel(0);
					OnGCSSelchange();
					return;
				}
				else
				{
					pProjection = new CGeographic();
					const Spheroid* pSpheroid = (const Spheroid*)((CComboBox*)GetDlgItem(IDC_GCSCOMBO))->GetItemData(index2);
					pProjection->SetupGCS(pSpheroid->key);
					pProjection->Reconfig();
				}
			}
			break;
		default:
			{
				const long index2 = ((CComboBox*)GetDlgItem(IDC_GCSCOMBO))->GetCurSel();
				if(index2==-1)
				{
					((CComboBox*)GetDlgItem(IDC_GCSCOMBO))->SetCurSel(0);
					OnGCSSelchange();
					return;
				}
				else
				{
					const CGeocentric::ProjType* pProjType = (const CGeocentric::ProjType*)data;
					pProjection = new CGeocentric(pProjType->key);
					const Spheroid* pSpheroid = (const Spheroid*)((CComboBox*)GetDlgItem(IDC_GCSCOMBO))->GetItemData(index2);
					pProjection->SetupGCS(pSpheroid->key);
					pProjection->Reconfig();
				}
			}
			break;
	}

	ResetProject(pProjection);
}

void CDatainfoDlg::ResetProject(CProjection* pProjection)
{
	if(pProjection==nullptr)
	{
		GetDlgItem(IDC_UNIT1)->SetWindowText(_T("Point"));
		GetDlgItem(IDC_UNIT2)->SetWindowText(_T("Point"));
		GetDlgItem(IDC_UNIT3)->SetWindowText(_T("Point"));
		GetDlgItem(IDC_UNIT4)->SetWindowText(_T("Point"));

		GetDlgItem(IDC_PAPERCOMBO)->EnableWindow(TRUE);
		GetDlgItem(IDC_PORTRAITRADIO)->EnableWindow(TRUE);
		GetDlgItem(IDC_LANDSCAPERADIO)->EnableWindow(TRUE);
		GetDlgItem(IDC_SOURCESCALE)->EnableWindow(FALSE);

		((CComboBox*)GetDlgItem(IDC_PROJECTIONTYPECOMBO))->SetCurSel(0);
		GetDlgItem(IDC_GCSCOMBO)->EnableWindow(FALSE);
		GetDlgItem(IDC_SETUPPROJECTION)->EnableWindow(FALSE);

		if(m_pProjection!=nullptr)
		{
			m_scaleMinimum = 128;
			m_scaleSource = 1.f;
			m_scaleMaximum = 0.01;

			m_xOrigin = 0.0;
			m_yOrigin = 0.0;
			m_cxCanvas = 595;
			m_cyCanvas = 842;
		}
	}
	else
	{
		const CString strUnit = pProjection->m_strUnit;
		GetDlgItem(IDC_UNIT1)->SetWindowText(strUnit);
		GetDlgItem(IDC_UNIT2)->SetWindowText(strUnit);
		GetDlgItem(IDC_UNIT3)->SetWindowText(strUnit);
		GetDlgItem(IDC_UNIT4)->SetWindowText(strUnit);

		GetDlgItem(IDC_PAPERCOMBO)->EnableWindow(FALSE);
		GetDlgItem(IDC_PORTRAITRADIO)->EnableWindow(FALSE);
		GetDlgItem(IDC_LANDSCAPERADIO)->EnableWindow(FALSE);
		GetDlgItem(IDC_SOURCESCALE)->EnableWindow(TRUE);

		GetDlgItem(IDC_GCSCOMBO)->EnableWindow(TRUE);
		const CString strName = pProjection->GetName();
		((CComboBox*)GetDlgItem(IDC_PROJECTIONTYPECOMBO))->SelectString(-1, strName);
		((CComboBox*)GetDlgItem(IDC_GCSCOMBO))->SetCurSel(CGcs::GetGCSIndex(pProjection->GCS.m_Key));

		if(pProjection->Gettype()==0)
			GetDlgItem(IDC_SETUPPROJECTION)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_SETUPPROJECTION)->EnableWindow(TRUE);

		if(m_pProjection==nullptr)
		{
			m_scaleMinimum = 100000000;
			m_scaleSource = 10000;
			m_scaleMaximum = 1000;
		}
		else if(m_pProjection!=nullptr)
		{
		}
		m_xOrigin = pProjection->UintMinX();
		m_yOrigin = pProjection->UintMinY();
		m_cxCanvas = pProjection->UintMaxX()-pProjection->UintMinX();
		m_cyCanvas = pProjection->UintMaxY()-pProjection->UintMinY();
	}

	delete m_pProjection;
	m_pProjection = pProjection;

	UpdateHAndW();
}

void CDatainfoDlg::OnGCSSelchange()
{
	const long index = ((CComboBox*)GetDlgItem(IDC_GCSCOMBO))->GetCurSel();
	if(m_pProjection!=nullptr)
	{
		const CGcs* gcs = (const CGcs*)((CComboBox*)GetDlgItem(IDC_GCSCOMBO))->GetItemData(index);
		m_pProjection->SetupGCS(gcs->m_Key);
	}
	else
		OnProjectionTypeChange();
}

void CDatainfoDlg::OnSetupProjection()
{
	if(m_pProjection!=nullptr)
	{
		m_pProjection->Setup();
	}
}

BOOL CDatainfoDlg::DestroyWindow()
{
	delete m_pProjection;
	m_pProjection = nullptr;

	return CDialog::DestroyWindow();
}
