#include "stdafx.h"
#include "Regtool.h"
#include "RegtoolDlg.h"

#include <afxwin.h>
#include <iostream>
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT ReadStringLength(CFile& file);
typedef LPCSTR (*GETCODE)();
typedef LPCSTR (*GETPWD)(WORD,CString);

/////////////////////////////////////////////////////////////////////////////
// CRegtoolDlg dialog

CRegtoolDlg::CRegtoolDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CRegtoolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegtoolDlg)
	m_Key1 = 0;
	m_Key2 = 0;
	m_RegCode = _T("");
	m_strFolder = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRegtoolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegtoolDlg)
	DDX_Text(pDX, IDC_KEY1, m_Key1);
	DDX_Text(pDX, IDC_KEY2, m_Key2);
	DDX_Text(pDX, IDC_REGCODE, m_RegCode);
	DDV_MaxChars(pDX, m_RegCode, 16);     
	DDX_Text(pDX, IDC_FOLDER, m_strFolder);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRegtoolDlg, CDialog)
	//{{AFX_MSG_MAP(CRegtoolDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDGET, OnGet)
	ON_BN_CLICKED(IDWRITE, OnWrite)
	ON_BN_CLICKED(IDC_GETFOLDER, OnGetFolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegtoolDlg message handlers

BOOL CRegtoolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRegtoolDlg::OnPaint() 
{
	if(IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1)/2;
		int y = (rect.Height() - cyIcon + 1)/2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRegtoolDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CRegtoolDlg::OnGet() 
{
	UpdateData(TRUE);

	if(m_RegCode.GetLength() != 16)
	{
		AfxMessageBox("您输入的位数不对！");
		return;
	}
	if(m_Key1 == 0 || m_Key2 == 0)
	{
		AfxMessageBox("您还没输入序列号呢！");
		return;
	}

	HINSTANCE hDLL = AfxLoadLibrary("HardwareSerial.dll");
	if(hDLL != nullptr)
	{
		CString	RegCode = m_RegCode;
		
		CString strPWD1;
		CString strPWD2;
		CString strPWD3;
		CString strPWD4;
		
		GETPWD GetPWD1 = (GETPWD)GetProcAddress(hDLL, "GetPWD1"); 
		if(GetPWD1 != nullptr) 
		{
			strPWD1 = GetPWD1(m_Key1,RegCode);
		}
		GETPWD GetPWD2 = (GETPWD)GetProcAddress(hDLL, "GetPWD2");
		if(GetPWD2 != nullptr) 
		{ 
			strPWD2 = GetPWD2(m_Key1,RegCode); 
		}
		GETPWD GetPWD3 = (GETPWD)GetProcAddress(hDLL, "GetPWD3"); 
		if(GetPWD3 != nullptr) 
		{ 
			strPWD3 = GetPWD3(m_Key1,RegCode); 
		}
		GETPWD GetPWD4 = (GETPWD)GetProcAddress(hDLL, "GetPWD4"); 
		if(GetPWD4 != nullptr) 
		{ 
			strPWD4 = GetPWD4(m_Key1,RegCode); 
		}
		::FreeLibrary(hDLL); 
	
		GetDlgItem(IDC_PWD1)->SetWindowText(strPWD1);	
		GetDlgItem(IDC_PWD2)->SetWindowText(strPWD2);	
		GetDlgItem(IDC_PWD3)->SetWindowText(strPWD3);	
		GetDlgItem(IDC_PWD4)->SetWindowText(strPWD4);	
	}
	else
	{
		AfxMessageBox(_T("缺少HardwareSerial.dll文件"));
	}
}

void CRegtoolDlg::OnWrite() 
{
	if(m_strFolder.IsEmpty() == TRUE)
		return;
	else if(_taccess(m_strFolder,00) == -1)
		return;

	UpdateData(TRUE);	
	Recursion(m_strFolder+"*.*");
}

void CRegtoolDlg::Recursion(CString strPath)
{
	HANDLE           hFile;
	WIN32_FIND_DATA  FindDate;

 	hFile = FindFirstFile(strPath,&FindDate);
	if(hFile==(HANDLE)0xffffffff)
		return;
	strPath = strPath.Left(strPath.ReverseFind('\\'));
	do   
    {
		if((FindDate.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		{	
			CString strPathname = FindDate.cFileName;	     
			if(strPathname == ".")
				continue;
			if(strPathname == "..")
				continue;
			
			strPathname = strPath + "\\" + strPathname + "\\*.*";    
			Recursion(strPathname);
			continue;
		}

		CString filename = FindDate.cFileName;	        
		filename.TrimRight();
		CString Ext = filename.Right(3);
		Ext.MakeUpper();
		if(Ext != _T("MAP") && Ext != _T("GIS") && Ext != _T("MIF"))
		{
			continue;
		}
		filename = strPath + "\\" + filename;
		
		CFile file;
		CFileException e;
		if(!file.Open(filename, CFile::modeReadWrite | CFile::typeBinary, &e))
		{
			AfxMessageBox("Cann't open file!");
			continue;
		}		
		file.SeekToBegin();
		file.Seek(sizeof(float), CFile::begin);

		UINT nNewLen1 = ReadStringLength(file);
		if(nNewLen1 == (UINT)-1)
		{
			nNewLen1 = ReadStringLength(file);
			file.Seek(nNewLen1*2, CFile::current);
		}
		else
			file.Seek(nNewLen1, CFile::current);

		file.Seek(sizeof(CTime), CFile::current);
		file.Seek(4, CFile::current);//不知道为什么这个地方多了4个字节
		file.Seek(sizeof(float), CFile::current);

		UINT nNewLen2 = ReadStringLength(file);
		if(nNewLen2 == (UINT)-1)
		{
			nNewLen2 = ReadStringLength(file);
			file.Seek(nNewLen2*2, CFile::current);
		}
		else
			file.Seek(nNewLen2, CFile::current);
		
		file.Seek(sizeof(double), CFile::current);
		file.Seek(sizeof(double), CFile::current);
		
		DWORD SoftKey = m_Key1;
		SoftKey = SoftKey<< 16;
		SoftKey = SoftKey | m_Key2;
		file.Write(&SoftKey,4);

		file.Close();
	} while (FindNextFile(hFile,&FindDate)==TRUE);

	return;
}

UINT ReadStringLength(CFile& file)
{
	DWORD nNewLen;

	// attempt BYTE length first
	BYTE bLen;
	file.Read(&bLen,1);
	if(bLen < 0xff)
		return bLen;

	// attempt WORD length
	WORD wLen;
	file.Read(&wLen,2);
	if(wLen == 0xfffe)
	{
		// UNICODE string prefix (length will follow)
		return (UINT)-1;
	}
	else if(wLen == 0xffff)
	{
		// read DWORD of length
		file.Read(&nNewLen,4);
		return (UINT)nNewLen;
	}
	else
		return wLen;
}

CString strTmpPath;

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	TCHAR szDir[MAX_PATH];
	switch(uMsg){
	case BFFM_INITIALIZED:
		if(lpData){
			strcpy_s(szDir, strTmpPath.GetBuffer(strTmpPath.GetLength()));
			SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)szDir);
			strTmpPath.ReleaseBuffer();
		}
		break;
	case BFFM_SELCHANGED: {
	   if(SHGetPathFromIDList((LPITEMIDLIST) lParam ,szDir)){
		  SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);
	   }
	   break;
	}
	default:
	   break;
	}
         
	return 0;
}

BOOL GetFolder(CString* strSelectedFolder,
				   const char* lpszTitle,
				   const HWND hwndOwner, 
				   const char* strRootFolder, 
				   const char* strStartFolder)
{
	char pszDisplayName[MAX_PATH];
	LPITEMIDLIST lpID;
	BROWSEINFOA bi;
	
	bi.hwndOwner = hwndOwner;
	if(strRootFolder == nullptr){
		bi.pidlRoot = nullptr;
	}else{
	   LPITEMIDLIST  pIdl = nullptr;
	   IShellFolder* pDesktopFolder;
	   char          szPath[MAX_PATH];
	   OLECHAR       olePath[MAX_PATH];
	   ULONG         chEaten;
	   ULONG         dwAttributes;

	   strcpy_s(szPath, (LPCTSTR)strRootFolder);
	   if(SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
	   {
		   MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szPath, -1, olePath, MAX_PATH);
		   pDesktopFolder->ParseDisplayName(nullptr, nullptr, olePath, &chEaten, &pIdl, &dwAttributes);
		   pDesktopFolder->Release();
	   }
	   bi.pidlRoot = pIdl;
	}
	bi.pszDisplayName = pszDisplayName;
	bi.lpszTitle = lpszTitle;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
	bi.lpfn = BrowseCallbackProc;
	if(strStartFolder == nullptr){
		bi.lParam = FALSE;
	}else{
		strTmpPath.Format("%s", strStartFolder);
		bi.lParam = TRUE;
	}
	bi.iImage = nullptr;
	lpID = SHBrowseForFolderA(&bi);
	if(lpID != nullptr){
		BOOL b = SHGetPathFromIDList(lpID, pszDisplayName);
		if(b == TRUE){
			strSelectedFolder->Format("%s",pszDisplayName);
			return TRUE;
		}
	}else{
		strSelectedFolder->Empty();
	}
	return FALSE;
}


void CRegtoolDlg::OnGetFolder() 
{
	CString strFolderPath;
	if(GetFolder(&strFolderPath, "Sample of  getting folder.", this->m_hWnd, nullptr, nullptr))
	{
		if(!strFolderPath.IsEmpty())
		{
			m_strFolder = strFolderPath;
			UpdateData(FALSE);
		}
	}	
}
