#pragma once
#include "Resource.h"

#include "../Public/ListCtrlEx.h"

class __declspec(dllexport) CFieldDlg : public CDialog
{
	DECLARE_DYNAMIC(CFieldDlg)
public:
	static long fieldtype_size[];
public:
	CFieldDlg(CDatabase* pDatabase, LPCTSTR strTable, CWnd* pParent = nullptr); // 标准构造函数
	~CFieldDlg() override;

	// 对话框数据
	enum
	{
		IDD = IDD_FIELD
	};

public:
	CDatabase* m_pDatabase;
	CString m_strTable;

	bool m_bModified;

public:
	void OnTableChanged(CString strTable);

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CListCtrlEx m_fieldlist;
	BOOL OnInitDialog() override;
	afx_msg void OnNew();
	afx_msg void OnDelete();
	afx_msg void OnMoveup();
	afx_msg void OnMovedown();
	afx_msg void OnDeleteitemFieldlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedFieldlist(NMHDR* pNMHDR, LRESULT* pResult);
};
