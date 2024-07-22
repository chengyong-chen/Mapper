#pragma once

#include "Resource.h"

using namespace std;
class CTheme;
class CClassificationDlg;
class CGeom;
class CATTDatasource;
class CDatabase;

class __declspec(dllexport) CThemeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CThemeDlg)

public:
	CThemeDlg(CWnd* pParent, CDatabase& database, CATTDatasource& datasource, CTheme* pTheme);

	~CThemeDlg() override;

	// Dialog Data
	enum
	{
		IDD = IDD_THEME
	};

public:
	CTheme* m_pTheme;
	CDatabase& m_database;
	CATTDatasource& m_datasource;

	CClassificationDlg* m_pClassificationDlg;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	void OnOK() override;
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChangeType();
	afx_msg void OnFieldGoRight();
	afx_msg void OnFieldGoLeft();
};
