// Viewer.h : main header file for the Viewer application

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

// CViewerApp:
// See Viewer.cpp for the implementation of this class

class CViewerApp : public CWinAppEx
{
	ULONG_PTR m_gdiplusToken;
public:
	CViewerApp();

private:
	BOOL InstallActiveX() const;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewerApp)
public:
	BOOL InitInstance() override;
	int ExitInstance() override;
	CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = nullptr) override;
	BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue) override;
	UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault) override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	void PreLoadState() override;
	void LoadCustomState() override;
	void SaveCustomState() override;
	//}}AFX_VIRTUAL

	// Implementation

	//{{AFX_MSG(CViewerApp)
	afx_msg void OnAppAbout();
	afx_msg void OnAppExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CViewerApp theApp;
