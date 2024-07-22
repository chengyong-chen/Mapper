#pragma once

#include "Resource.h"
#include "Datasource.h"

using namespace std;
class CODBCDatabase;

class __declspec(dllexport) CATTDatasourceDlg : public CDialogEx
{
public:
	CATTDatasourceDlg(CWnd* pParent, CODBCDatabase* database, CATTHeaderProfile& headerprofile);

	~CATTDatasourceDlg() override;

	// Dialog Data
	enum
	{
		IDD = IDD_ATTDATASOURCE
	};

public:
	CODBCDatabase* m_pDatabase;
	CATTHeaderProfile& m_headerprofile;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;

	afx_msg void OnSelchangeTable();
	afx_msg void OnSelchangeKey();
	DECLARE_MESSAGE_MAP()
};
