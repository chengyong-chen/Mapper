// MyPropertyPage1.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "OptionPage.h"
#include "..\Socket\ExStdCFunction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(COptionPage1, CPropertyPage)
IMPLEMENT_DYNCREATE(COptionPage2, CPropertyPage)


/////////////////////////////////////////////////////////////////////////////
// COptionPage1 property page

COptionPage1::COptionPage1() : CPropertyPage(COptionPage1::IDD)
{
	//{{AFX_DATA_INIT(COptionPage1)
	m_nDay = 0;
	m_strPath = _T("");
	//}}AFX_DATA_INIT
}

COptionPage1::~COptionPage1()
{
}

void COptionPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionPage1)
	DDX_Text(pDX, IDC_RPTNUM_C, m_nDay);
	DDV_MinMaxInt(pDX, m_nDay, 0, 365);
	DDX_Text(pDX, IDC_INTERVAL_C, m_strPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionPage1, CPropertyPage)
	//{{AFX_MSG_MAP(COptionPage1)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COptionPage2 property page

COptionPage2::COptionPage2() : CPropertyPage(COptionPage2::IDD)
{
	//{{AFX_DATA_INIT(COptionPage2)
		
	//}}AFX_DATA_INIT
}

COptionPage2::~COptionPage2()
{
}

void COptionPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionPage2)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionPage2, CPropertyPage)
	//{{AFX_MSG_MAP(COptionPage2)
	ON_BN_CLICKED(IDC_CHECK_BROADCAST, OnCheckBroadcast)
	ON_BN_CLICKED(IDC_CHECK_FEEMANAGE, OnCheckFeemanage)
	ON_BN_CLICKED(IDC_CHECK_GROUPMANAGE, OnCheckGroupmanage)
	ON_BN_CLICKED(IDC_CHECK_LOG, OnCheckLog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL COptionPage1::OnSetActive() 
{
	CRegKey regKey;
	if(ERROR_SUCCESS == regKey.Open(HKEY_CURRENT_USER,_T("Software\\Mapday\\GServer\\1.0\\backup")))
	{
		char buff[MAX_PATH];
		DWORD len = sizeof(buff);
		memset(buff, 0, MAX_PATH);
		if(ERROR_SUCCESS != regKey.QueryValue(buff, _T("SavePath"), &len))
			m_strPath = _T("");
		else
			m_strPath = buff;

		DWORD dwDay;
		if(ERROR_SUCCESS != regKey.QueryValue(dwDay, _T("IntervalDay")))
			m_nDay = 30;
		else
			m_nDay = dwDay;
		regKey.Close();
	}
	UpdateData(FALSE);
	
	return CPropertyPage::OnSetActive();
}

BOOL COptionPage1::OnKillActive() 
{
	UpdateData();
	CRegKey regKey;
	if(ERROR_SUCCESS == regKey.Create(HKEY_CURRENT_USER,_T("Software\\Mapday\\GServer\\1.0\\backup")))
	{
		regKey.SetValue(m_strPath, _T("SavePath"));
		regKey.SetValue(m_nDay, _T("IntervalDay"));
		regKey.Close();
	}
	
	return CPropertyPage::OnKillActive();
}

void COptionPage2::OnCheckBroadcast() 
{
	UpdateData();
	CRegKey regKey;
	if(ERROR_SUCCESS == regKey.Create(HKEY_CURRENT_USER,
		_T("Software\\Mapday\\GServer\\1.0\\settings"))
		)
	{
		DWORD dwVal = 1;
		if(((CButton*)GetDlgItem(IDC_CHECK_BROADCAST))->GetCheck() == BST_CHECKED)
		{
			dwVal = 1;
			regKey.SetValue(dwVal, _T("broadcastmsg"));
		}else
		{
			dwVal = 0;
			regKey.SetValue(dwVal, _T("broadcastmsg"));
		}

		regKey.Close();
	}
}

void COptionPage2::OnCheckFeemanage() 
{
	UpdateData();
	CRegKey regKey;
	if(ERROR_SUCCESS == regKey.Create(HKEY_CURRENT_USER,
		_T("Software\\Mapday\\GServer\\1.0\\settings\\finance"))
		)
	{
		DWORD dwVal = 1;
		if(((CButton*)GetDlgItem(IDC_CHECK_FEEMANAGE))->GetCheck() == BST_CHECKED)
		{
			dwVal = 1;
			regKey.SetValue(dwVal, _T("UseFinanceManager"));
		}else
		{
			dwVal = 0;
			regKey.SetValue(dwVal, _T("UseFinanceManager"));
		}

		regKey.Close();
	}	
}

void COptionPage2::OnCheckGroupmanage() 
{
	UpdateData();
	CRegKey regKey;
	if(ERROR_SUCCESS == regKey.Create(HKEY_CURRENT_USER,
		_T("Software\\Mapday\\GServer\\1.0\\settings"))
		)
	{
		DWORD dwVal = 1;
		if(((CButton*)GetDlgItem(IDC_CHECK_GROUPMANAGE))->GetCheck() == BST_CHECKED)
		{
			dwVal = 1;
			regKey.SetValue(dwVal, _T("TrayIcon"));
		}else
		{
			dwVal = 0;
			regKey.SetValue(dwVal, _T("TrayIcon"));
		}

		regKey.Close();
	}	
}

void COptionPage2::OnCheckLog() 
{
	UpdateData();
	CRegKey regKey;
	if(ERROR_SUCCESS == regKey.Create(HKEY_CURRENT_USER,
		_T("Software\\Mapday\\GServer\\1.0\\settings"))
		)
	{
		DWORD dwVal = 1;
		if(((CButton*)GetDlgItem(IDC_CHECK_LOG))->GetCheck() == BST_CHECKED)
		{
			dwVal = 1;
			regKey.SetValue(dwVal, _T("NeedLog"));
		}else
		{
			dwVal = 0;
			regKey.SetValue(dwVal, _T("NeedLog"));
		}

		regKey.Close();
	}	
}

BOOL COptionPage2::OnSetActive() 
{
	CRegKey regKey;
	if(ERROR_SUCCESS == regKey.Open(HKEY_CURRENT_USER,_T("Software\\Mapday\\GServer\\1.0\\settings"))
		)
	{
		DWORD dwVal;
		regKey.QueryValue(dwVal, _T("NeedLog"));
		if(1 == dwVal)
			((CButton*)GetDlgItem(IDC_CHECK_LOG))->SetCheck(TRUE);
		else
			((CButton*)GetDlgItem(IDC_CHECK_LOG))->SetCheck(FALSE);

		regKey.QueryValue(dwVal, _T("broadcastmsg"));
		if(1 == dwVal)
			((CButton*)GetDlgItem(IDC_CHECK_BROADCAST))->SetCheck(TRUE);
		else
			((CButton*)GetDlgItem(IDC_CHECK_BROADCAST))->SetCheck(FALSE);

		regKey.Close();
	}

	if(ERROR_SUCCESS == regKey.Open(HKEY_CURRENT_USER,
		_T("Software\\Mapday\\GServer\\1.0\\settings"))
		)
	{
		DWORD dwVal;
		regKey.QueryValue(dwVal, _T("TrayIcon"));
		if(1 == dwVal)
			((CButton*)GetDlgItem(IDC_CHECK_GROUPMANAGE))->SetCheck(TRUE);
		else
			((CButton*)GetDlgItem(IDC_CHECK_GROUPMANAGE))->SetCheck(FALSE);

		regKey.Close();
	}

	if(ERROR_SUCCESS == regKey.Open(HKEY_CURRENT_USER,
		_T("Software\\Mapday\\GServer\\1.0\\settings\\finance"))
		)
	{
		DWORD dwVal;
		regKey.QueryValue(dwVal, _T("UseFinanceManager"));
		if(1 == dwVal)
			((CButton*)GetDlgItem(IDC_CHECK_FEEMANAGE))->SetCheck(TRUE);
		else
			((CButton*)GetDlgItem(IDC_CHECK_FEEMANAGE))->SetCheck(FALSE);

		regKey.Close();
	}
	
	return CPropertyPage::OnSetActive();
}

void COptionPage1::OnButton1() 
{
	CString strPath;
	if(TRUE == BrowseForFolder(strPath, _T("请选择备份文件存放目录")))
	{
		m_strPath = strPath;
		UpdateData(FALSE);
	}
}
