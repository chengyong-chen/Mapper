#pragma once

#include "Resource.h"

using namespace std;
class CODBCDatabase;
class CPOUHeaderProfile;

class __declspec(dllexport) CPOUDatasourceDlg : public CDialogEx
{
public:
	CPOUDatasourceDlg(CWnd* pParent, CODBCDatabase* database, CPOUHeaderProfile& headerprofile);

	~CPOUDatasourceDlg() override;

	// Dialog Data
	enum
	{
		IDD = IDD_POUDATASOURCE
	};

public:
	CODBCDatabase* m_pDatabase;

	CPOUHeaderProfile& m_headerprofile;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;

	afx_msg void OnOk();
	afx_msg void OnSelchangeTable();
	afx_msg void OnSelchangeKey();
	DECLARE_MESSAGE_MAP()
};
