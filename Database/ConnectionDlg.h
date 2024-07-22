#pragma once

#include "Resource.h"

using namespace std;
#include <map>
#include <list>
class CODBCDatabase;

class __declspec(dllexport) CConnectionDlg : public CDialogEx
{
public:
	CConnectionDlg(CWnd* pParent, bool bSupport, std::map<CString, CODBCDatabase*>& databaselist, std::list<CString*>& activenames, CString& strDatabase);

	~CConnectionDlg() override;

	// Dialog Data
	enum
	{
		IDD = IDD_CONNECTION
	};

public:
	bool m_bSupport;

	std::list<CString*>& m_activenames;

	std::map<CString, CODBCDatabase*>& m_databaselist;

	CString& m_strDatabase;

public:
	CODBCDatabase* GetSelectedDatabase() const;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	afx_msg void OnOk();
	afx_msg void OnAdd();
	afx_msg void OnConfigure();
	afx_msg void OnRemove();
	afx_msg void OnConnect();
	afx_msg void OnRename();
	afx_msg void OnSelchangeDatabase();
	afx_msg void OnCheckedSupport();
	DECLARE_MESSAGE_MAP()

public:
};
