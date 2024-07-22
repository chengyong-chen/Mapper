#pragma once

#include "Resource.h"

class __declspec(dllexport) CGeomQueryEar : public CDialog
{
	DECLARE_DYNCREATE(CGeomQueryEar)

	public:
	CGeomQueryEar();

	// Dialog Data
	//{{AFX_DATA(CGeomQueryEar)
	enum
	{
		IDD = IDD_GEOMQUERY
	};

	CListCtrl m_listCtrl;

	CComboBox m_TypeCombo;
	CComboBox m_MarkCombo;
	CComboBox m_RoadCombo;
	CComboBox m_POITypeCombo1;
	CComboBox m_POITypeCombo2;
	CComboBox m_POITypeCombo3;

	//}}AFX_DATA
	public:
	CDatabase* m_pDatabase;

	void OnSetComboDatabase(CDatabase* pDatabase);
	void AddMapCombo(CMap<WORD, WORD&, CString, CString&>& mapIdMap) const;

	public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeomQueryEar)
	public:
	BOOL PreTranslateMessage(MSG* pMsg) override;
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL
	static void DDX_CBData(CDataExchange* pDX, int nIDC, DWORD& wData);
	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CGeomQueryEar)
	afx_msg void OnOK() override;
	public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	protected:
	afx_msg void OnQuery();
	BOOL OnInitDialog() override;
	afx_msg void OnDeleteItemListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClass1Selchange();
	afx_msg void OnPOICom1SelChanged();
	afx_msg void OnPOICom2SelChanged();
	afx_msg void OnCbnEditupdateMapcombo();
	afx_msg void OnSelchangeType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	public:
};
