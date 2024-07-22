#pragma once
#include "resource.h"
#include "afxwin.h"

#include "EditableListCtrl.h"
#include "ClassificationDlg.h"

class CChoropleth;
class CChoroplethDlg : public CClassificationDlg
{
	DECLARE_DYNAMIC(CChoroplethDlg)

public:
	CChoroplethDlg(CWnd* pParent, CChoropleth& choropleth, CDatabase& database, CATTDatasource& datasource);

	~CChoroplethDlg() override;

	// Dialog Data
	enum
	{
		IDD = IDD_CHOROPLETH
	};

public:
	CEditableListCtrl m_StepsListCtrl;
	CEdit m_celledit;
public:
	CChoropleth& m_choropleth;

	//	static BOOL InitEditor(CWnd** pWnd, int nRow, int nColumn, CString& strSubItemText, DWORD_PTR dwItemData, void* pThis, BOOL bUpdate);
	static BOOL OnEndCellEdit(CWnd** pWnd, int nRow, int nColumn, CString& strSubItemText, DWORD_PTR dwItemData, void* pThis, BOOL bUpdate);

public:
	void OnFieldAdded(const CString& strField) override;
	void OnFieldRemoved(const CString& strField) override;
	void OnRemoveAllFields() override;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg void OnContinuous() override;
	afx_msg void OnDivisional() override;
	afx_msg void OnColorchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedMfccolorbutton();
	afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};
