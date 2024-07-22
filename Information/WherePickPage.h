#pragma once
#include "resource.h"

#include "../Public/ListCtrlEx.h"

class CWherePickPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CWherePickPage)

	public:
	CWherePickPage();

	// Dialog Data
	//{{AFX_DATA(CWherePickPage)
	enum
	{
		IDD = IDD_WHEREPICK
	};

	CListCtrlEx m_ListCtrl;
	//}}AFX_DATA

	CDatabase* m_pDatabase;
	CString m_strTable;
	CString* m_strWhere;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWherePickPage)
	public:
	BOOL PreTranslateMessage(MSG* pMsg) override;
	BOOL OnSetActive() override;
	BOOL OnKillActive() override;
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CWherePickPage)
	BOOL OnInitDialog() override;
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	//}}AFX_MSG
	afx_msg BOOL OnBeginLabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};
