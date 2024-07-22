#include "stdafx.h"
#include "ThemeDlg.h"
#include "afxdialogex.h"
#include "Theme.h"
#include "ChoroplethDlg.h"
#include "GraduatedDlg.h"
#include "PieDlg.h"
#include "BarDlg.h"
#include "CylinderDlg.h"
#include "../Public/ODBCDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CThemeDlg dialog

using namespace std;
#include <list>
IMPLEMENT_DYNAMIC(CThemeDlg, CDialogEx)

CThemeDlg::CThemeDlg(CWnd* pParent, CDatabase& database, CATTDatasource& datasource, CTheme* pTheme)
	: CDialogEx(CThemeDlg::IDD, pParent), m_database(database), m_datasource(datasource)
{
	m_pTheme = pTheme;
	m_pClassificationDlg = nullptr;
}

CThemeDlg::~CThemeDlg()
{
}

void CThemeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CThemeDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO3, &CThemeDlg::OnChangeType)
	ON_BN_CLICKED(IDC_BUTTON1, &CThemeDlg::OnFieldGoRight)
	ON_BN_CLICKED(IDC_BUTTON2, &CThemeDlg::OnFieldGoLeft)
END_MESSAGE_MAP()

BOOL CThemeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	int index0 = ((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString(_T("None"));
	((CComboBox*)GetDlgItem(IDC_COMBO3))->SetItemData(index0, 0);
	int index1 = ((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString(_T("Choropleth"));
	((CComboBox*)GetDlgItem(IDC_COMBO3))->SetItemData(index1, 1);
	int index2 = ((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString(_T("Graduated"));
	((CComboBox*)GetDlgItem(IDC_COMBO3))->SetItemData(index2, 2);
	int index3 = ((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString(_T("Pie"));
	((CComboBox*)GetDlgItem(IDC_COMBO3))->SetItemData(index3, 5);
	int index4 = ((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString(_T("Bar"));
	((CComboBox*)GetDlgItem(IDC_COMBO3))->SetItemData(index4, 6);
	int index5 = ((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString(_T("Cylinder"));
	((CComboBox*)GetDlgItem(IDC_COMBO3))->SetItemData(index5, 7);

	CRecordset rs(&m_database);
	try
	{
		const CHeaderProfile& headerprofile = m_datasource.GetHeaderProfile();
		CString strSQL = headerprofile.MakeSelectStatment();
		CODBCDatabase::ParseSQL(&m_database, strSQL);

		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);

		int fieldCount = rs.GetODBCFieldCount();
		for(short nIndex = 0; nIndex < fieldCount; nIndex++)
		{
			CODBCFieldInfo fieldinfo;
			rs.GetODBCFieldInfo(nIndex, fieldinfo);
			if(fieldinfo.m_strName.CompareNoCase(_T("ID")) == 0)
				continue;
			else if(fieldinfo.m_strName.CompareNoCase(_T("X")) == 0)
				continue;
			else if(fieldinfo.m_strName.CompareNoCase(_T("Y")) == 0)
				continue;
			else if(fieldinfo.m_strName.CompareNoCase(_T("X")) == 0)
				continue;
			else if(fieldinfo.m_strName.CompareNoCase(_T("Y")) == 0)
				continue;
			else if(fieldinfo.m_strName.CompareNoCase(_T("Altitude")) == 0)
				continue;

			switch(fieldinfo.m_nSQLType)
			{
				case SQL_FLOAT:
				case SQL_REAL:
				case SQL_DOUBLE:
				case SQL_DECIMAL:
				case SQL_NUMERIC:
				case SQL_TINYINT:
				case SQL_SMALLINT:
				case SQL_INTEGER:
				case SQL_BIGINT:
					((CListBox*)GetDlgItem(IDC_FIELDLIST1))->AddString(fieldinfo.m_strName);
					break;
				default:
					break;
			}
		}
		rs.Close();
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}

	if(m_pTheme != nullptr)
	{
		for(int index = ((CListBox*)GetDlgItem(IDC_FIELDLIST1))->GetCount() - 1; index >= 0; index--)
		{
			CString strField;
			((CListBox*)GetDlgItem(IDC_FIELDLIST1))->GetText(index, strField);
			std::list<CString>::iterator it = std::find(m_pTheme->m_fields.begin(), m_pTheme->m_fields.end(), strField);
			if(it != m_pTheme->m_fields.end())
			{
				((CListBox*)GetDlgItem(IDC_FIELDLIST2))->AddString(strField);
				((CListBox*)GetDlgItem(IDC_FIELDLIST1))->DeleteString(index);
			}
		}

		((CComboBox*)GetDlgItem(IDC_COMBO3))->SetCurSel(-1);

		BYTE type = m_pTheme->Gettype();
		for(int index = 0; index < ((CComboBox*)GetDlgItem(IDC_COMBO3))->GetCount(); index++)
		{
			DWORD data = ((CComboBox*)GetDlgItem(IDC_COMBO3))->GetItemData(index);
			if(data == type)
			{
				((CComboBox*)GetDlgItem(IDC_COMBO3))->SetCurSel(index);
				OnChangeType();
				break;
			}
		}
	}

	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE	
}

void CThemeDlg::OnOK()
{
	if(m_pClassificationDlg != nullptr)
	{
		m_pClassificationDlg->SendMessage(WM_COMMAND, IDOK, 0);
	}

	CDialogEx::OnOK();
}

void CThemeDlg::OnChangeType()
{
	//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const int index = ((CComboBox*)GetDlgItem(IDC_COMBO3))->GetCurSel();
	if(index == -1)
		return;

	if(m_pClassificationDlg != nullptr)
	{
		m_pClassificationDlg->PostMessage(WM_DESTROY, 0, 0);
		m_pClassificationDlg->DestroyWindow();
		delete m_pClassificationDlg;
		m_pClassificationDlg = nullptr;
	}
	const DWORD type = ((CComboBox*)GetDlgItem(IDC_COMBO3))->GetItemData(index);
	if(m_pTheme == nullptr)
	{
		m_pTheme = CTheme::Apply(type);
	}
	else if(type == 0)
	{
		delete m_pTheme;
		m_pTheme = nullptr;
	}
	else if(m_pTheme->Gettype() != type)
	{
		CTheme* pTheme = CTheme::Apply(type);
		m_pTheme->CopyTo(pTheme);
		pTheme->RecalculateLimit(m_database, m_datasource);

		delete m_pTheme;
		m_pTheme = pTheme;
	}

	switch(type)
	{
		case 0X01:
			{
				CChoroplethDlg* pDlg = new CChoroplethDlg((CWnd*)this, *((CChoropleth*)m_pTheme), m_database, m_datasource);
				pDlg->Create(IDD_CHOROPLETH, (CWnd*)this);
				m_pClassificationDlg = pDlg;
			}
			break;
		case 0X02:
			{
				CGraduatedDlg* pDlg = new CGraduatedDlg((CWnd*)this, *((CGraduated*)m_pTheme), m_database, m_datasource);
				pDlg->Create(IDD_GRADUATED, (CWnd*)this);
				m_pClassificationDlg = pDlg;
			}
			break;
		case 0X05:
			{
				CPieDlg* pDlg = new CPieDlg((CWnd*)this, *((CPie*)m_pTheme), m_database, m_datasource);
				pDlg->Create(IDD_PIE, (CWnd*)this);
				m_pClassificationDlg = pDlg;
			}
			break;
		case 0X06:
			{
				CBarDlg* pDlg = new CBarDlg((CWnd*)this, *((CBar*)m_pTheme), m_database, m_datasource);
				pDlg->Create(IDD_BAR, (CWnd*)this);
				m_pClassificationDlg = pDlg;
			}
			break;
		case 0X07:
			{
				CCylinderDlg* pDlg = new CCylinderDlg((CWnd*)this, *((CCylinder*)m_pTheme), m_database, m_datasource);
				pDlg->Create(IDD_GRADUATED, (CWnd*)this);
				m_pClassificationDlg = pDlg;
			}
			break;
	}

	if(m_pClassificationDlg != nullptr && m_pClassificationDlg->m_hWnd != nullptr)
	{
		CRect rect;
		((CWnd*)GetDlgItem(IDC_HOLDER))->GetWindowRect(&rect);
		this->ScreenToClient(rect);
		m_pClassificationDlg->SetWindowPos(&wndTop, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
		std::list<CString> fields;
		const int count = ((CListBox*)GetDlgItem(IDC_FIELDLIST2))->GetCount();
		for(int index = 0; index < count; index++)
		{
			CString strField;
			((CListBox*)GetDlgItem(IDC_FIELDLIST2))->GetText(index, strField);
			fields.push_back(strField);
		}
		m_pClassificationDlg->AddFields(fields);
	}
}

void CThemeDlg::OnFieldGoRight()
{
	const int index = ((CListBox*)GetDlgItem(IDC_FIELDLIST1))->GetCurSel();
	if(index == -1)
		return;

	CString strField;
	((CListBox*)GetDlgItem(IDC_FIELDLIST1))->GetText(index, strField);
	((CListBox*)GetDlgItem(IDC_FIELDLIST1))->DeleteString(index);
	((CListBox*)GetDlgItem(IDC_FIELDLIST2))->AddString(strField);
	if(m_pClassificationDlg != nullptr)
	{
		m_pClassificationDlg->OnFieldAdded(strField);
	}

	if(((CListBox*)GetDlgItem(IDC_FIELDLIST1))->GetCount() > 0)
	{
		((CListBox*)GetDlgItem(IDC_FIELDLIST1))->SetCurSel(0);
	}
}

void CThemeDlg::OnFieldGoLeft()
{
	const int index = ((CListBox*)GetDlgItem(IDC_FIELDLIST2))->GetCurSel();
	if(index == -1)
		return;

	CString strField;
	((CListBox*)GetDlgItem(IDC_FIELDLIST2))->GetText(index, strField);
	((CListBox*)GetDlgItem(IDC_FIELDLIST2))->DeleteString(index);
	((CListBox*)GetDlgItem(IDC_FIELDLIST1))->AddString(strField);
	if(m_pClassificationDlg != nullptr)
	{
		m_pClassificationDlg->OnFieldRemoved(strField);
	}
	const int nCount = ((CListBox*)(GetDlgItem(IDC_FIELDLIST2)))->GetCount();
	if(nCount > 0)
	{
		((CListBox*)GetDlgItem(IDC_FIELDLIST2))->SetCurSel(nCount - 1);
	}
}

void CThemeDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	if(m_pClassificationDlg != nullptr)
	{
		m_pClassificationDlg->PostMessage(WM_DESTROY, 0, 0);
		m_pClassificationDlg->DestroyWindow();
		delete m_pClassificationDlg;
		m_pClassificationDlg = nullptr;
	}
}
