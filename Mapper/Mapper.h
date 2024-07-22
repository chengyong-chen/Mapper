#pragma once


#include "../Public/IPlugin.h"

using namespace std;
#include <list>

class CMapperApp : public CWinAppEx
{
	ULONG_PTR m_gdiplusToken;

public:
	CMapperApp() noexcept;

private:
	std::list<Plugin> m_plugins;
	void AddPlugins();
	void ConvertFile(CString input, CString output, BYTE action);

public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapperApp)
	BOOL InitInstance() override;
	int Run() override;
	int ExitInstance() override;
	void PreLoadState() override;
	void LoadCustomState() override;
	void SaveCustomState() override;
	//}}AFX_VIRTUAL

public:
	//{{AFX_MSG(CMapperApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileImport();
	afx_msg void OnCloseAll();
	afx_msg void OnPlugin(UINT nId);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//extern CMapperApp theApp;
