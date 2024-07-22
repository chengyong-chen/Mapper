// SetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Setup.h"
#include "SetupDlg.h"
#include "GeoDib32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg dialog

CSetupDlg::CSetupDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CSetupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupDlg)
		
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_rSetup     = CRect(0, 0, 0, 0);
	m_rRun       = CRect(0, 0, 0, 0);
	m_rUninstall = CRect(0, 0, 0, 0);
	m_rQuit      = CRect(0, 0, 0, 0);
	m_rReadme    = CRect(0, 0, 0, 0);
	m_rCredit    = CRect(0, 0, 0, 0);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hDIB = nullptr;
	m_colButtonNormalColor = 0;
	m_colButtonSelectColor = 0XFFFFFF;
	m_colButtonDisableColor = RGB(128, 128, 128);
}

CSetupDlg::~CSetupDlg()
{
	if(m_hDIB)
		DestroyDIB(m_hDIB);
}

void CSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupDlg)
	DDX_Control(pDX, IDC_SETUP_README, m_readme);
	DDX_Control(pDX, IDC_SETUP_EXIT, m_exit);
	DDX_Control(pDX, IDC_SETUP_UNINSTALL, m_uninstall);
	DDX_Control(pDX, IDC_SETUP_RUN, m_run);
	DDX_Control(pDX, IDC_SETUP_SETUP, m_setup);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CSetupDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SETUP_SETUP, OnSetupSetup)
	ON_BN_CLICKED(IDC_SETUP_RUN, OnSetupRun)
	ON_BN_CLICKED(IDC_SETUP_EXIT, OnSetupExit)
	ON_BN_CLICKED(IDC_SETUP_UNINSTALL, OnSetupUninstall)
	ON_WM_ACTIVATE()
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_SETUP_README, OnSetupReadme)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg message handlers

// �����ʼ���ļ�
BOOL CSetupDlg::LoadInitialFile()
{
	// Now try to open the setup.inf file
	CString rFile, rFilePath;
	LPSTR p = rFile.GetBuffer(MAX_PATH);
	::GetModuleFileName(AfxGetApp()->m_hInstance, p, MAX_PATH);
	rFile.ReleaseBuffer();
	rFile  = rFile.Left(rFile.ReverseFind('\\') + 1);
	rFilePath = rFile + _T("setup.inf");
	
	CStdioFile file;
	file.Open(rFilePath, CFile::modeRead);
	if(file.m_hFile == CFile::hFileNull)
	{
		AfxMessageBox(IDS_NOSETUPFILE);
		return FALSE;
	}
	
	int nMark = -1, length;
	CString line, head, tail;
	while(file.ReadString(line))
	{
		length = line.GetLength();
		if(length && line[0] == _T(';'))                  // ע��
			continue;
		else if(!line.CompareNoCase(_T("[Setup]")))     // ��װ��Ϣ��
		{
			nMark = 0;
			continue;
		}
		else if(!line.CompareNoCase(_T("[Run]")))       // ������Ϣ��
		{
			nMark = 1;
			continue;
		}
		else if(!line.CompareNoCase(_T("[Uninstall]"))) // ж����Ϣ��
		{
			nMark = 2;
			continue;
		}
		else if(!line.CompareNoCase(_T("[Quit]")))      // �˳���Ϣ��
		{
			nMark = 3;
			continue;
		}
		else if(!line.CompareNoCase(_T("[Readme]")))    // ˵����Ϣ��
		{
			nMark = 4;
			continue;
		}else if(!line.CompareNoCase(_T("[Credit]")))   // ��Ļ��Ϣ��
		{
			nMark = 5;
			continue;
		}
		

		int n = line.Find(_T("="));
		if(n == -1) continue;
		head = line.Left(n);
		tail = line.Right(length - n - 1);
		head.TrimLeft(); head.TrimRight();
		tail.TrimLeft(); tail.TrimRight();

		switch(nMark)
		{
		case 0:
			{
				if(!head.CompareNoCase(_T("Position")))
				{
					_stscanf(tail, _T("%d,%d"), &m_rSetup.left, &m_rSetup.top);
					m_rSetup.right  = m_rSetup.left + 63;
					m_rSetup.bottom = m_rSetup.top + 27;
				}
				else if(!head.CompareNoCase(_T("SetupName")))
					m_rSetupName = tail;
				else if(!head.CompareNoCase(_T("SetupDir")))
					m_rSetupDir = tail;
				else if(!head.CompareNoCase(_T("ClassName")))
					m_rClassName = tail;
				else if(!head.CompareNoCase(_T("WindowTitle")))
					m_rWindowTitle = tail;
				else if(!head.CompareNoCase(_T("ButtonNormalColor")))
				{
					int red, green, blue;
					_stscanf(tail, _T("%d,%d,%d"), &red, &green, &blue);
					m_colButtonNormalColor = RGB(red, green, blue);
				}
				else if(!head.CompareNoCase(_T("ButtonSelectColor")))
				{
					int red, green, blue;
					_stscanf(tail, _T("%d,%d,%d"), &red, &green, &blue);
					m_colButtonSelectColor = RGB(red, green, blue);
				}
				else if(!head.CompareNoCase(_T("ButtonDisableColor")))
				{
					int red, green, blue;
					_stscanf(tail, _T("%d,%d,%d"), &red, &green, &blue);
					m_colButtonDisableColor = RGB(red, green, blue);
				}
			}
			break;
		case 1:
			{
				if(!head.CompareNoCase(_T("Position")))
				{
					_stscanf(tail, _T("%d,%d"), &m_rRun.left, &m_rRun.top);
					m_rRun.right  = m_rRun.left + 63;
					m_rRun.bottom = m_rRun.top + 27;
				}
				else if(!head.CompareNoCase(_T("RunName")))
					m_rRunName = tail;
				else if(!head.CompareNoCase(_T("RunKey")))
					m_rRunKey = tail;
				else if(!head.CompareNoCase(_T("LocalPath")))
					m_rLocalPath = tail;
			}
			break;
		case 2:
			{
				if(!head.CompareNoCase(_T("Position")))
				{
					_stscanf(tail, _T("%d,%d"), &m_rUninstall.left, &m_rUninstall.top);
					m_rUninstall.right  = m_rUninstall.left + 63;
					m_rUninstall.bottom = m_rUninstall.top + 27;
				}
				else if(!head.CompareNoCase(_T("UninstallName")))
					m_rUninstallName = tail;
				else if(!head.CompareNoCase(_T("UninstallKey")))
					m_rUninstallKey = tail;
			}
			break;
		case 3:
			{
				if(!head.CompareNoCase(_T("Position")))
				{
					_stscanf(tail, _T("%d,%d"), &m_rQuit.left, &m_rQuit.top);
					m_rQuit.right  = m_rQuit.left + 63;
					m_rQuit.bottom = m_rQuit.top + 27;
				}
			}
			break;
		case 4:
			{
				if(!head.CompareNoCase(_T("Position")))
				{
					_stscanf(tail, _T("%d,%d"), &m_rReadme.left, &m_rReadme.top);
					m_rReadme.right  = m_rReadme.left + 63;
					m_rReadme.bottom = m_rReadme.top + 27;
				}
				else if(!head.CompareNoCase(_T("ReadmeName")))
					m_rReadmeName = tail;
				else if(!head.CompareNoCase(_T("ReadmePath")))
					m_rReadmeFile = tail;
			}
			break;
		case 5:
			{
				if(!head.CompareNoCase(_T("Position")))
				{
					_stscanf(tail, _T("%d,%d,%d,%d"), &m_rCredit.left, &m_rCredit.top, &m_rCredit.right, &m_rCredit.bottom);
					m_rCredit.right  += m_rCredit.left;
					m_rCredit.bottom += m_rCredit.top;
				}
				else if(!head.CompareNoCase(_T("CreditName")))
					m_credit.m_rCreditArray.Add(tail);
				else if(!head.CompareNoCase(_T("CreditHeadColor")))
				{
					int red, green, blue;
					_stscanf(tail, _T("%d,%d,%d"), &red, &green, &blue);
					m_credit.m_colCreditHeadColor = RGB(red, green, blue);
				}
				else if(!head.CompareNoCase(_T("CreditSecondColor")))
				{
					int red, green, blue;
					_stscanf(tail, _T("%d,%d,%d"), &red, &green, &blue);
					m_credit.m_colCreditSecondColor = RGB(red, green, blue);
				}
			}
			break;
		}
	}
	file.Close();

	rFilePath = rFile + _T("setup.bmp");
	m_hDIB = LoadDIB(rFilePath);
	if(!m_hDIB)
		return FALSE;
	return TRUE;
}

BOOL CSetupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_credit.SubclassWindow(GetDlgItem(IDC_SETUP_CREDIT)->GetSafeHwnd());
	if(!LoadInitialFile())
	{
		OnOK();
		return FALSE;
	}

	// �����Ʒ�����������У��򱾳������С�
	HWND hWnd = nullptr;
	if(m_rClassName.GetLength())
	{
		if(m_rWindowTitle.GetLength())
			hWnd = ::FindWindowEx(nullptr, nullptr, m_rClassName, m_rWindowTitle);
		else
			hWnd = ::FindWindowEx(nullptr, nullptr, m_rClassName, nullptr);
	}
	else
	{
		if(m_rWindowTitle.GetLength())
			hWnd = ::FindWindowEx(nullptr, nullptr, nullptr, m_rWindowTitle);
	}

	if(hWnd != nullptr)
	{
		OnOK();
		return FALSE;
	}

	// �ı䰴ť���ڵ�λ��
	if(!m_rSetup.IsRectNull())
		m_setup.MoveWindow(m_rSetup);
	if(!m_rRun.IsRectNull())
		m_run.MoveWindow(m_rRun);
	if(!m_rUninstall.IsRectNull())
		m_uninstall.MoveWindow(m_rUninstall);
	if(!m_rQuit.IsRectNull())
		m_exit.MoveWindow(m_rQuit);
	if(!m_rReadme.IsRectNull())
		m_readme.MoveWindow(m_rReadme);
	if(!m_rCredit.IsRectNull())
		m_credit.MoveWindow(m_rCredit);
	
	if(m_rReadmeFile.IsEmpty())
		m_readme.ShowWindow(SW_HIDE);
	else
	{
		if(m_rReadmeName.GetLength())
			m_readme.SetWindowText(m_rReadmeName);
	}

	StateChange();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	if(m_rSetupName.GetLength())
	{
		CString strTitile;
		strTitile.LoadString(IDS_INSTALL);
		CString r = strTitile + _T(" - ") + m_rSetupName;
		SetWindowText(r);
	}

	return TRUE;
}

// �ı�״̬
void CSetupDlg::StateChange()
{
	if(CanUninstall())
	{
		CString str;
		str.LoadString(IDS_REINSTALL);
		m_setup.SetWindowText(str);

		m_run.EnableWindow(TRUE);
		m_uninstall.EnableWindow(TRUE);
	}
	else
	{
		CString str;
		str.LoadString(IDS_INSTALL);
		m_setup.SetWindowText(str);

		// ����ܱ�������
		if(m_rLocalPath.GetLength())
			m_run.EnableWindow(TRUE);
		else
			m_run.EnableWindow(FALSE);

		m_uninstall.EnableWindow(FALSE);
	}

	if(!CanInstall())
		m_setup.EnableWindow(FALSE);
}

void CSetupDlg::OnPaint()
{
	if(IsIconic())
	{
		CPaintDC dc(this);
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
		if(m_hDIB)
		{
			CPaintDC dc(this);
			CRect rcClient;
			GetClientRect(&rcClient);
			PaintDIB(dc.m_hDC, rcClient, m_hDIB,CRect(0, 29, 398, 299), nullptr);
		}
		else
			CDialog::OnPaint();
	}
}

HCURSOR CSetupDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// ��װ����
BOOL CSetupDlg::CanInstall()
{
	if(!m_rSetupDir.GetLength())
		return FALSE;
	return TRUE;
}

// ж�ز���
static char regkey[] =	_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
BOOL CSetupDlg::CanUninstall()
{
	CString rKey;
	rKey = regkey;

	HKEY hKey = nullptr;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, rKey + m_rUninstallName, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwSize = 0;
		if(RegQueryValueEx(hKey, m_rUninstallKey, nullptr, nullptr,nullptr, &dwSize) == ERROR_SUCCESS)
		{
			char buffer[_MAX_PATH];
			LONG lRet = RegQueryValueEx(hKey, m_rUninstallKey, nullptr, nullptr,(LPBYTE)buffer, &dwSize);
			RegCloseKey(hKey);
			if(lRet == ERROR_SUCCESS) 
				return TRUE;
			else return FALSE;
		}
		RegCloseKey(hKey);
	}
	return FALSE;
}

// ��װ
void CSetupDlg::OnSetupSetup() 
{
	// Now try to open the setup.inf file
	CString rFilePath;
	LPSTR p = rFilePath.GetBuffer(MAX_PATH);
	::GetModuleFileName(AfxGetApp()->m_hInstance, p, MAX_PATH);
	rFilePath.ReleaseBuffer();
	rFilePath  = rFilePath.Left(rFilePath.ReverseFind('\\') + 1);
	rFilePath += m_rSetupDir;

	BOOL bOk;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	//ShowWindow(SW_SHOWMINIMIZED);
	char lpcmdline[MAX_PATH];
	wsprintf(lpcmdline, rFilePath);
	memset(&si,0,sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.wShowWindow = SW_SHOW;
	bOk = ::CreateProcess(nullptr, 
		lpcmdline,
		nullptr, nullptr, FALSE, 0, nullptr, nullptr,
		&si, &pi);
	if(!bOk)
		ReportError();
	StateChange();
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

// ����
void CSetupDlg::OnSetupRun() 
{
	// ����Ѿ���װ
	if(CanUninstall())
	{	
		HKEY hKey = nullptr;
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, m_rRunName, 
			0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			DWORD dwSize = 0;
			if(RegQueryValueEx(hKey, m_rRunKey, nullptr, nullptr,
				nullptr, &dwSize) == ERROR_SUCCESS)
			{
				char buffer[_MAX_PATH];
				LONG lRet = RegQueryValueEx(hKey, m_rRunKey, nullptr, nullptr,
					(LPBYTE)buffer, &dwSize);
				if(lRet == ERROR_SUCCESS)
				{
					BOOL bOk;
					STARTUPINFO si;
					PROCESS_INFORMATION pi;
					
					ShowWindow(SW_SHOWMINIMIZED);
					memset(&si,0,sizeof(STARTUPINFO));
					si.cb = sizeof(STARTUPINFO);
					si.wShowWindow = SW_SHOW;
					bOk = ::CreateProcess(nullptr, 
						buffer,
						nullptr, nullptr, FALSE, 0, nullptr, nullptr,
						&si, &pi);
					if(!bOk)
					{
						RegCloseKey(hKey);
						ReportError();
						ShowWindow(SW_RESTORE);
						return;
					}
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);
				}
			}
			RegCloseKey(hKey);
		}
	}
	// ��������
	else if(m_rLocalPath.GetLength())
	{
		CString rFilePath;
		LPSTR p = rFilePath.GetBuffer(MAX_PATH);
		::GetModuleFileName(AfxGetApp()->m_hInstance, p, MAX_PATH);
		rFilePath.ReleaseBuffer();
		rFilePath  = rFilePath.Left(rFilePath.ReverseFind('\\') + 1);
		rFilePath += m_rLocalPath;

		BOOL bOk;
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		
		ShowWindow(SW_SHOWMINIMIZED);
		memset(&si,0,sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.wShowWindow = SW_SHOW;
		bOk = ::CreateProcess(nullptr, 
			rFilePath.GetBuffer(1),
			nullptr, nullptr, FALSE, 0, nullptr, nullptr,
			&si, &pi);
		rFilePath.ReleaseBuffer();
		if(!bOk)
		{
			ReportError();
			ShowWindow(SW_RESTORE);
			return;
		}
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
}
// ж��
void CSetupDlg::OnSetupUninstall() 
{
	CString rKey;
	rKey = regkey;

	HKEY hKey = nullptr;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, rKey + m_rUninstallName, 
		0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwSize = 0;
		if(RegQueryValueEx(hKey, m_rUninstallKey, nullptr, nullptr,
			nullptr, &dwSize) == ERROR_SUCCESS)
		{
			char buffer[_MAX_PATH];
			LONG lRet = RegQueryValueEx(hKey, m_rUninstallKey, nullptr, nullptr,
				(LPBYTE)buffer, &dwSize);
			if(lRet == ERROR_SUCCESS)
			{
				RegCloseKey(hKey);

				BOOL bOk;
				STARTUPINFO si;
				PROCESS_INFORMATION pi;
				
				ShowWindow(SW_SHOWMINIMIZED);
				memset(&si,0,sizeof(STARTUPINFO));
				si.cb = sizeof(STARTUPINFO);
				si.wShowWindow = SW_SHOW;
				bOk = ::CreateProcess(nullptr, 
					buffer,
					nullptr, nullptr, FALSE, 0, nullptr, nullptr,
					&si, &pi);
				if(!bOk)
				{
					ReportError();
					StateChange();
					ShowWindow(SW_RESTORE);
					return;
				}

				while(WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_OBJECT_0)
				{
					StateChange();
					ShowWindow(SW_RESTORE);
					break;
				}
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
				return;
			}
		}
		RegCloseKey(hKey);
	}
	StateChange();
}

// �˳�
void CSetupDlg::OnSetupExit() 
{
	OnOK();
}

// ˵��
void CSetupDlg::OnSetupReadme() 
{
	// Now try to open the setup.inf file
	CString rFilePath;
	LPSTR p = rFilePath.GetBuffer(MAX_PATH);
	::GetModuleFileName(AfxGetApp()->m_hInstance, p, MAX_PATH);
	rFilePath.ReleaseBuffer();
	rFilePath  = rFilePath.Left(rFilePath.ReverseFind('\\') + 1);
	rFilePath += m_rReadmeFile;

	HINSTANCE hinst = 
		::ShellExecute(m_hWnd, "open", rFilePath, nullptr, nullptr, SW_SHOWNORMAL);
	if((int)hinst <= 32)
		ReportError();
/*
	BOOL bOk;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	char lpcmdline[MAX_PATH];
	wsprintf(lpcmdline, _T("notepad.exe %s"), rFilePath);
	memset(&si,0,sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.wShowWindow = SW_SHOW;
	bOk = ::CreateProcess(nullptr, 
		lpcmdline,
		nullptr, nullptr, FALSE, 0, nullptr, nullptr,
		&si, &pi);
	
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
*/
}

void CSetupDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	
	StateChange();
}

void TransparentBlt(HDC hDC, HBITMAP hBitmap, 
                    int xStart, int yStart, COLORREF cTransparentColor)
{
	BITMAP		bm;
	COLORREF	cColor;
	HBITMAP		bmAndBack, bmAndObject, bmAndMem, bmSave;
	HBITMAP		bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
	HDC			hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
	POINT		ptSize;
	int			nSave;

	ASSERT(hBitmap);

	nSave = SaveDC(hDC);
	hdcTemp = CreateCompatibleDC(hDC);
	SelectObject(hdcTemp, hBitmap);
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
	ptSize.x = bm.bmWidth;
	ptSize.y = bm.bmHeight;
	DPtoLP(hdcTemp, &ptSize, 1);

	hdcBack = CreateCompatibleDC(hDC);
	hdcObject = CreateCompatibleDC(hDC);
	hdcMem = CreateCompatibleDC(hDC);
	hdcSave = CreateCompatibleDC(hDC);

	bmAndBack = CreateBitmap(ptSize.x, ptSize.y, 1, 1, nullptr);
	bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, nullptr);
	bmAndMem = CreateCompatibleBitmap(hDC, ptSize.x, ptSize.y);
	bmSave = CreateCompatibleBitmap(hDC, ptSize.x, ptSize.y);

	bmBackOld = (HBITMAP)SelectObject(hdcBack, bmAndBack);
	bmObjectOld = (HBITMAP)SelectObject(hdcObject, bmAndObject);
	bmMemOld = (HBITMAP)SelectObject(hdcMem, bmAndMem);
	bmSaveOld = (HBITMAP)SelectObject(hdcSave, bmSave);

	SetMapMode(hdcTemp, GetMapMode(hDC));

	BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

	cColor = SetBkColor(hdcTemp, cTransparentColor);
	BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

	SetBkColor(hdcTemp, cColor);
	BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, NOTSRCCOPY);

	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hDC, xStart, yStart, SRCCOPY);

	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);

	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);

	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);

	BitBlt(hDC, xStart, yStart, ptSize.x, ptSize.y, hdcMem, 0, 0, SRCCOPY);

	BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

	DeleteObject(SelectObject(hdcBack, bmBackOld));
	DeleteObject(SelectObject(hdcObject, bmObjectOld));
	DeleteObject(SelectObject(hdcMem, bmMemOld));
	DeleteObject(SelectObject(hdcSave, bmSaveOld));

	DeleteDC(hdcMem);
	DeleteDC(hdcBack);
	DeleteDC(hdcObject);
	DeleteDC(hdcSave);
	DeleteDC(hdcTemp);

	RestoreDC(hDC, nSave);
}

void CSetupDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if(nIDCtl == IDC_SETUP_SETUP ||
		nIDCtl == IDC_SETUP_RUN ||
		nIDCtl == IDC_SETUP_UNINSTALL ||
		nIDCtl == IDC_SETUP_EXIT ||
		nIDCtl == IDC_SETUP_README)
	{
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		int nSaveDC = pDC->SaveDC();

		if(lpDrawItemStruct->itemState & ODS_DISABLED)
		{
			pDC->SetTextColor(m_colButtonDisableColor);
			if(m_hDIB)
				PaintDIB(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, m_hDIB,CRect(CPoint(126, 0), CSize(63, 29)), nullptr);
		}
		else if(lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			pDC->SetTextColor(m_colButtonSelectColor);
			if(m_hDIB)
				PaintDIB(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, m_hDIB,	CRect(CPoint(63, 0), CSize(63, 29)), nullptr);	
		}
		else
		{
			pDC->SetTextColor(m_colButtonNormalColor);
			if(m_hDIB)
				PaintDIB(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, m_hDIB,CRect(CPoint(0, 0), CSize(63, 29)), nullptr);
		}

		CString rText;
		GetDlgItem(nIDCtl)->GetWindowText(rText);
		pDC->SetBkMode(TRANSPARENT);

		pDC->DrawText(rText, &lpDrawItemStruct->rcItem, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		pDC->RestoreDC(nSaveDC);
	}
	else
		CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

// �������!
void CSetupDlg::ReportError()
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf, 0, nullptr);// Process any inserts in lpMsgBuf.
	
	// ...// Display the string.
	MessageBox((LPCTSTR)lpMsgBuf, _T("Install"), MB_OK | MB_ICONINFORMATION);
	// Free the buffer.
	LocalFree(lpMsgBuf);
}