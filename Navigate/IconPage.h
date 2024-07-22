#pragma once
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CIconPage dialog

class CIconPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CIconPage)

	public:
	CIconPage();

	~CIconPage() override;

	// Dialog Data
	//{{AFX_DATA(CIconPage)
	enum
	{
		IDD = IDD_ICON
	};

	CStatic m_Icon1;
	CStatic m_Icon2;
	CStatic m_Icon3;
	CStatic m_Icon4;
	CListCtrl m_ImageCtrl1;
	CListCtrl m_ImageCtrl2;
	CListCtrl m_ImageCtrl3;
	CListCtrl m_ImageCtrl4;
	//}}AFX_DATA

	CImageList m_imagelist;

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CIconPage)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:
	// Generated message map functions
	//{{AFX_MSG(CIconPage)
	BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg void OnItemchangedImagelist1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedImagelist2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedImagelist3(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedImagelist4(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
