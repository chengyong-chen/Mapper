#pragma once

#include "Resource.h"

using namespace std;
class CODBCDatabase;
class CGEOHeaderProfile;
class __declspec(dllexport) CGEODatasourceDlg : public CDialogEx
{
public:
	CGEODatasourceDlg(CWnd* pParent, CODBCDatabase* database, CGEOHeaderProfile& headerfile);

	~CGEODatasourceDlg() override;

	// Dialog Data
	enum
	{
		IDD = IDD_GEODATASOURCE
	};

public:
	CODBCDatabase* m_pDatabase;

	CGEOHeaderProfile& m_headerfile;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	afx_msg void OnOk();
	afx_msg void OnSelchangeTableB();
	DECLARE_MESSAGE_MAP()
};
