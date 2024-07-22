#pragma once


#include "ClassificationDlg.h"
#include "EditableListCtrl.h"

using namespace std;
class CChart;

class CChartDlg : public CClassificationDlg
{
	DECLARE_DYNAMIC(CChartDlg)

public:
	CChartDlg(UINT IDD, CWnd* pParent, CChart& chart, CDatabase& database, CATTDatasource& datasource);

	~CChartDlg() override;

public:
	CEditableListCtrl m_FieldListCtrl;
	CMFCPropertyGridCtrl m_PropertyGrid;

public:
	CChart& m_chart;

public:
	void OnFieldAdded(const CString& strField) override;
	void OnFieldRemoved(const CString& strField) override;
	void OnRemoveAllFields() override;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	afx_msg void OnColorchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam) const;
	DECLARE_MESSAGE_MAP()
};
