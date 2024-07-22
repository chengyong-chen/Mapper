#pragma once

#include "Pie.h"
#include "ChartDlg.h"
#include "Resource.h"

#include "../GridCtrl/GridCtrl.h"

using namespace std;

class CPieDlg : public CChartDlg
{
	DECLARE_DYNAMIC(CPieDlg)

public:
	CPieDlg(CWnd* pParent, CPie& pie, CDatabase& database, CATTDatasource& datasource);

	~CPieDlg() override;

	// Dialog Data
	enum
	{
		IDD = IDD_PIE
	};

public:
	CGridCtrl m_StepsListCtrl;

public:
	CPie& m_pie;

public:
	void OnRemoveAllFields() override;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	afx_msg void OnPaint();
	BOOL OnInitDialog() override;
	afx_msg void OnContinuous() override;
	afx_msg void OnDivisional() override;
	//	afx_msg void OnColorchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

class CSymbolCell2 : public CGridCell
{
	DECLARE_DYNCREATE(CSymbolCell2)
public:
	CSymbolCell2();

	~CSymbolCell2() override;
public:
	CPieDlg* m_pChartDlg;
	// Attributes
public:
	BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE) override;
};
