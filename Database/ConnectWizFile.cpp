#include "stdafx.h"
//#include "agrmmccomdlg.h"
#include "ConnectWizFile.h"

#include "../Public/ODBCDatabase.h"

#include <map>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectWizFile property page

CConnectWizFile::CConnectWizFile(std::map<CString, CODBCDatabase*>& databaselist, CODBCDatabase& connectioninfo)
	: CPropertyPage(CConnectWizFile::IDD, 0, IDS_ADOCONNECTWIZ_PICKFILE, 0), m_ConnectInfo(connectioninfo), m_existinglist(databaselist)
{
	m_pIMyFolderFilter = nullptr;
}

CConnectWizFile::~CConnectWizFile()
{
}

void CConnectWizFile::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CConnectWizFile, CPropertyPage)
	ON_BN_CLICKED(IDC_PICKFILE, &CConnectWizFile::OnBnClickedPickfile)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// Internal use

void CConnectWizFile::SetWizardButtons() const
{
	CPropertySheet* psheet = (CPropertySheet*)GetParent();

	CString strName;
	((CWnd*)GetDlgItem(IDC_EDIT))->GetWindowText(strName);
	if(strName.IsEmpty()==TRUE)
		psheet->SetWizardButtons(PSWIZB_BACK);
	else
		psheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
}

/////////////////////////////////////////////////////////////////////////////
// CConnectWizFile message handlers

BOOL CConnectWizFile::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	((CWnd*)GetDlgItem(IDC_EDIT))->SetWindowText(m_ConnectInfo.m_strDatabase);

	CPropertySheet* psheet = (CPropertySheet*)GetParent();
	if(m_ConnectInfo.m_strDatabase.IsEmpty()==FALSE)
		psheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	else
		psheet->SetWizardButtons(PSWIZB_BACK);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CConnectWizFile::OnSetActive()
{
	SetWizardButtons();

	return CPropertyPage::OnSetActive();
}

LRESULT CConnectWizFile::OnWizardBack()
{
	return IDD_ADOCONNECTWIZ_DBTYPE;
}

LRESULT CConnectWizFile::OnWizardNext()
{
	return IDD_ADOCONNECTWIZ_USERINFO;
}

int CALLBACK SHBrowseForFolderCallbackProc(IN HWND hWnd, IN UINT uMsg, IN LPARAM lParam, IN LPARAM lpData)
{
	switch(uMsg)
	{
		case BFFM_IUNKNOWN:
			{
				IFolderFilterSite* pIFolderFilterSite = nullptr;
				const HRESULT hResult = ((IUnknown*)lParam)->QueryInterface(IID_IFolderFilterSite, (LPVOID*)&pIFolderFilterSite);
				if(SUCCEEDED(hResult))
				{
					IMyFolderFilter* pIMyFolderFilter = new IMyFolderFilter;
					pIFolderFilterSite->SetFilter((IUnknown*)pIMyFolderFilter);
					SAFE_RELEASE(pIFolderFilterSite);
					pIMyFolderFilter->m_pszFilter = _T("*.mdb");
				}
			}
			break;
		default:
			break;
	}
	return 0;
}

void CConnectWizFile::OnBnClickedPickfile()
{
	TCHAR szFile[MAX_PATH];
	_tcscpy_s(szFile, m_ConnectInfo.m_strServer);
	BROWSEINFO bInfo;
	bInfo.hwndOwner = this->m_hWnd;
	bInfo.pidlRoot = nullptr;
	bInfo.pszDisplayName = szFile; // Address of a buffer to receive the display name of the folder selected by the user 
	bInfo.lpszTitle = _T("Please select a Microsft Access .MDB file"); // Title of the dialog 
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE|BIF_NONEWFOLDERBUTTON|BIF_BROWSEINCLUDEFILES;
	bInfo.lpfn = SHBrowseForFolderCallbackProc;
	bInfo.lParam = (LPARAM)this;
	bInfo.iImage = -1;
	CPropertySheet* psheet = (CPropertySheet*)GetParent();
	const LPITEMIDLIST lpItem = SHBrowseForFolder(&bInfo);
	if(lpItem!=nullptr)
	{
		SHGetPathFromIDList(lpItem, szFile);
		m_ConnectInfo.m_strDatabase = m_ConnectInfo.m_strServer;
		((CWnd*)GetDlgItem(IDC_EDIT))->SetWindowText(m_ConnectInfo.m_strDatabase);
		psheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	}
	else
	{
		psheet->SetWizardButtons(PSWIZB_BACK);
	}
}

IMyFolderFilter::IMyFolderFilter(VOID)
	: m_ulRef(0), m_pszFilter(nullptr)
{
}

STDMETHODIMP IMyFolderFilter::QueryInterface(IN REFIID riid, IN OUT LPVOID* ppvObj)
{
	ATLASSERT(ppvObj!=nullptr);
	if(!ppvObj)
		return E_FAIL;

	HRESULT hResult = E_NOINTERFACE;
	*ppvObj = nullptr;

	if(IsEqualIID(riid, IID_IUnknown))
		*ppvObj = static_cast<IUnknown*>(this);
	else if(IsEqualIID(riid, IID_IFolderFilter))
		*ppvObj = static_cast<IFolderFilter*>(this);

	if(*ppvObj)
	{
		reinterpret_cast<IUnknown*>(*ppvObj)->AddRef();
		hResult = S_OK;
	}

	return hResult;
}

STDMETHODIMP_(ULONG) IMyFolderFilter::AddRef(VOID)
{
	return ::InterlockedIncrement((PLONG)&m_ulRef);
}

STDMETHODIMP_(ULONG) IMyFolderFilter::Release(VOID)
{
	const ULONG ulNewRef = ::InterlockedDecrement((PLONG)&m_ulRef);
	if(ulNewRef<=0)
	{
		delete this;
		return 0;
	}

	return ulNewRef;
}

STDMETHODIMP IMyFolderFilter::ShouldShow(IN IShellFolder* pIShellFolder, IN LPCITEMIDLIST /*pidlFolder*/, IN LPCITEMIDLIST pidlItem)
{
	ASSERT(pIShellFolder!=nullptr);
	ASSERT(pidlItem!=nullptr);

	ASSERT(m_pszFilter!=nullptr);
	if(!m_pszFilter)
		return E_FAIL;

	// If an item is a folder, then accept it

	LPCITEMIDLIST pidl[1] = {pidlItem};
	SFGAOF ulAttr = SFGAO_FOLDER;

	pIShellFolder->GetAttributesOf(1, pidl, &ulAttr);

	if((ulAttr&SFGAO_FOLDER)==SFGAO_FOLDER)
		return S_OK;

	// If an item is a file, then filter it

	STRRET strRet = {0};
	HRESULT hResult = pIShellFolder->GetDisplayNameOf(pidlItem, SHGDN_NORMAL|SHGDN_FORPARSING, &strRet);

	if(SUCCEEDED(hResult))
	{
		LPTSTR pszDisplayName = nullptr;
		hResult = ::StrRetToStr(&strRet, pidlItem, &pszDisplayName);

		if(SUCCEEDED(hResult))
		{
			hResult = S_FALSE;

			if(::PathMatchSpec(pszDisplayName, m_pszFilter))
				hResult = S_OK;

			SAFE_COTASKMEMFREE(pszDisplayName);
		}
	}

	return hResult;
}

STDMETHODIMP IMyFolderFilter::GetEnumFlags(IN IShellFolder* /*pIShellFolder*/, IN LPCITEMIDLIST /*pidlFolder*/, IN HWND* /*phWnd*/, OUT LPDWORD pdwFlags)
{
	ASSERT(pdwFlags!=nullptr);

	*pdwFlags = (DWORD)(SHCONTF_FOLDERS|SHCONTF_NONFOLDERS);
	return S_OK;
}
