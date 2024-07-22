#include "Stdafx.h"
#include "ConnectionDlg.h"
#include "RenameDlg.h"

#include "Wizard.h"
#include "ConnectWizStart.h"
#include "ConnectWizName.h"
#include "ConnectWizType.h"
#include "ConnectWizServer.h"
#include "ConnectWizfile.h"
#include "ConnectWizLogin.h"
#include "ConnectWizComplete.h"
#include <algorithm>

#include "../Public/ODBCDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CConnectionDlg::CConnectionDlg(CWnd* pParent, bool bSupport, std::map<CString, CODBCDatabase*>& databaselist, std::list<CString*>& activenames, CString& strDatabase)
	: CDialogEx(CConnectionDlg::IDD, pParent), m_bSupport(bSupport), m_databaselist(databaselist), m_strDatabase(strDatabase), m_activenames(activenames)
{
}

CConnectionDlg::~CConnectionDlg()
{
}

void CConnectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CConnectionDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_CONFIGURE, OnConfigure)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_RENAME, OnRename)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDC_SUPPORTATT, OnCheckedSupport)
	ON_LBN_SELCHANGE(IDC_DATABASELIST, OnSelchangeDatabase)
END_MESSAGE_MAP()

BOOL CConnectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CListBox* pDatabaseListBox = (CListBox*)GetDlgItem(IDC_DATABASELIST);
	if(pDatabaseListBox!=nullptr)
	{
		for(std::map<CString, CODBCDatabase*>::iterator it = m_databaselist.begin(); it!=m_databaselist.end(); ++it)
		{
			const int index = pDatabaseListBox->AddString(it->first);
			pDatabaseListBox->SetItemData(index, (DWORD)it->second);
		}

		pDatabaseListBox->SelectString(-1, m_strDatabase);
		OnSelchangeDatabase();
	}

	((CButton*)GetDlgItem(IDC_SUPPORTATT))->SetCheck(m_bSupport);
	OnCheckedSupport();
	return TRUE;
}

CODBCDatabase* CConnectionDlg::GetSelectedDatabase() const
{
	CListBox* pDatabaseListBox = (CListBox*)GetDlgItem(IDC_DATABASELIST);
	if(pDatabaseListBox==nullptr)
		return nullptr;
	const int index = pDatabaseListBox->GetCurSel();
	if(index==-1)
		return nullptr;

	pDatabaseListBox->GetText(index, m_strDatabase);
	return (CODBCDatabase*)pDatabaseListBox->GetItemData(index);
}

void CConnectionDlg::OnCheckedSupport()
{
	m_bSupport = ((CButton*)GetDlgItem(IDC_SUPPORTATT))->GetCheck()==BST_CHECKED ? true : false;
	if(m_bSupport==true)
	{
		GetDlgItem(IDC_DATABASELIST)->EnableWindow(TRUE);
		GetDlgItem(IDC_ADD)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_DATABASELIST)->EnableWindow(FALSE);
		GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_REMOVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CONFIGURE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);

		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
}

void CConnectionDlg::OnAdd()
{
	CString strName = _T("");
	CODBCDatabase ConnectInfo;
	CWizard ADOConnectionWiz(_T("Create a new database connection"), nullptr, 0, nullptr, nullptr, nullptr);
	CConnectWizStart startPage(ConnectInfo);
	CConnectWizName namePage(strName, ConnectInfo);
	CConnectWizType dbTypePage(ConnectInfo);
	CConnectWizServer serverPage(ConnectInfo);
	CConnectWizFile filePage(m_databaselist, ConnectInfo);
	CConnectWizLogin loginPage(ConnectInfo);
	CConnectWizComplete completePage(ConnectInfo);

	// Add pages to wizard
	ADOConnectionWiz.AddPage(&startPage);
	ADOConnectionWiz.AddPage(&namePage);
	ADOConnectionWiz.AddPage(&dbTypePage);
	ADOConnectionWiz.AddPage(&filePage);
	ADOConnectionWiz.AddPage(&serverPage);
	ADOConnectionWiz.AddPage(&loginPage);
	ADOConnectionWiz.AddPage(&completePage);
	ADOConnectionWiz.SetWizardMode();

	if(ADOConnectionWiz.DoModal()==ID_WIZFINISH)
	{
		if(m_databaselist.find(strName)!=m_databaselist.end())
			AfxMessageBox(_T("The name you gave must be unique!"));
		else
		{
			CODBCDatabase* pDatabase = new CODBCDatabase();
			pDatabase->m_dbms = ConnectInfo.m_dbms;
			pDatabase->m_strServer = ConnectInfo.m_strServer;
			pDatabase->m_strDatabase = ConnectInfo.m_strDatabase;
			pDatabase->m_strUID = ConnectInfo.m_strUID;
			pDatabase->m_strPWD = ConnectInfo.m_strPWD;
			m_databaselist[strName] = pDatabase;
			pDatabase->Open();
			CListBox* pDatabaseListBox = (CListBox*)GetDlgItem(IDC_DATABASELIST);
			if(pDatabaseListBox!=nullptr)
			{
				int index = pDatabaseListBox->AddString(strName);
				pDatabaseListBox->SetItemData(index, (DWORD)pDatabase);
				pDatabaseListBox->SetCurSel(index);
			}
		}
	}
}

void CConnectionDlg::OnRemove()
{
	CListBox* pDatabaseListBox = (CListBox*)GetDlgItem(IDC_DATABASELIST);
	const int index = pDatabaseListBox->GetCurSel();
	CString strName;
	pDatabaseListBox->GetText(index, strName);
	const std::map<CString, CODBCDatabase*>::iterator it = m_databaselist.find(strName);
	if(it!=m_databaselist.end())
	{
		it->second->Close();
		delete it->second;
		it->second = nullptr;
		m_databaselist.erase(it);

		pDatabaseListBox->DeleteString(index);
	}
}

void CConnectionDlg::OnConfigure()
{
	CODBCDatabase* pDatabase = GetSelectedDatabase();
	if(pDatabase!=nullptr)
	{
		CODBCDatabase ConnectInfo;
		ConnectInfo.m_dbms = pDatabase->m_dbms;
		ConnectInfo.m_strServer = pDatabase->m_strServer;
		ConnectInfo.m_strDatabase = pDatabase->m_strDatabase;
		ConnectInfo.m_strUID = pDatabase->m_strUID;
		ConnectInfo.m_strPWD = pDatabase->m_strPWD;

		CWizard ADOConnectionWiz(_T("Configure a database connection"), nullptr, 0, nullptr, nullptr, nullptr);
		CConnectWizType dbTypePage(ConnectInfo);
		CConnectWizServer serverPage(ConnectInfo);
		CConnectWizFile filePage(m_databaselist, ConnectInfo);
		CConnectWizLogin loginPage(ConnectInfo);
		CConnectWizComplete completePage(ConnectInfo);
		ADOConnectionWiz.AddPage(&dbTypePage);
		ADOConnectionWiz.AddPage(&serverPage);
		ADOConnectionWiz.AddPage(&filePage);
		ADOConnectionWiz.AddPage(&loginPage);
		ADOConnectionWiz.AddPage(&completePage);
		ADOConnectionWiz.SetWizardMode();
		if(ADOConnectionWiz.DoModal()==ID_WIZFINISH)
		{
			pDatabase->m_dbms = ConnectInfo.m_dbms;
			pDatabase->m_strServer = ConnectInfo.m_strServer;
			pDatabase->m_strDatabase = ConnectInfo.m_strDatabase;
			pDatabase->m_strUID = ConnectInfo.m_strUID;
			pDatabase->m_strPWD = ConnectInfo.m_strPWD;
			pDatabase->Close();
			pDatabase->Open();
		}
	}
}

void CConnectionDlg::OnConnect()
{
	const CListBox* pDatabaseListBox = (CListBox*)GetDlgItem(IDC_DATABASELIST);
	const int index = pDatabaseListBox->GetCurSel();
	CString strName;
	pDatabaseListBox->GetText(index, strName);

	CODBCDatabase* pDatabase = GetSelectedDatabase();
	if(pDatabase!=nullptr)
	{
		pDatabase->Close();
		if(pDatabase->Open()==true)
			AfxMessageBox(_T("Connection to ")+strName+_T(" succeeded!"));
		else
			AfxMessageBox(_T("Connection to ")+strName+_T(" failed!"));
	}
}

void CConnectionDlg::OnRename()
{
	CListBox* pDatabaseListBox = (CListBox*)GetDlgItem(IDC_DATABASELIST);
	int index = pDatabaseListBox->GetCurSel();
	CString oldName;
	pDatabaseListBox->GetText(index, oldName);
	CRenameDlg dlg(this, oldName);
	if(dlg.DoModal()==IDOK)
	{
		const CString newName = dlg.m_cNewName;
		for(std::map<CString, CODBCDatabase*>::iterator it = m_databaselist.begin(); it!=m_databaselist.end(); ++it)
		{
			if(it->first==oldName)
			{
				m_databaselist[newName] = it->second;
				m_databaselist.erase(it);
				break;
			}
		}
		for(std::list<CString*>::iterator it = m_activenames.begin(); it!=m_activenames.end(); ++it)
		{
			if(**it==oldName)
			{
				**it = newName;
			}
		}
		CODBCDatabase* pDatabase = m_databaselist[newName];
		pDatabaseListBox->DeleteString(index);
		index = pDatabaseListBox->InsertString(index, newName);
		pDatabaseListBox->SetItemData(index, (DWORD)pDatabase);
		pDatabaseListBox->SetCurSel(index);
	}
}

void CConnectionDlg::OnOk()
{
	const CListBox* pDatabaseListBox = (CListBox*)GetDlgItem(IDC_DATABASELIST);
	const int index = pDatabaseListBox->GetCurSel();
	if(index!=-1)
		pDatabaseListBox->GetText(index, m_strDatabase);
	else
		m_strDatabase.Empty();

	CDialog::OnOK();
}

void CConnectionDlg::OnSelchangeDatabase()
{
	CODBCDatabase* pDatabase = GetSelectedDatabase();
	if(pDatabase!=nullptr)
	{
		GetDlgItem(IDC_REMOVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CONFIGURE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_RENAME)->EnableWindow(TRUE);
	}
}
