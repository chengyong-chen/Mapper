#pragma once

#include "afxwin.h"

#include "ClassificationDlg.h"
#include "Graduated.h"
#include "Resource.h"

#include "../GridCtrl/GridCtrl.h"

class CGridCtrl;

class CGraduatedDlg : public CClassificationDlg
{
	DECLARE_DYNAMIC(CGraduatedDlg)

public:
	CGraduatedDlg(CWnd* pParent, CGraduated& graduated, CDatabase& database, CATTDatasource& datasource);

	~CGraduatedDlg() override;

	// Dialog Data
	enum
	{
		IDD = IDD_GRADUATED
	};

public:
	CGridCtrl m_StepsListCtrl;
	CMFCPropertyGridCtrl m_PropertyGrid;

public:
	CGraduated& m_graduated;
	CFont m_font;
	void OnFieldAdded(const CString& strField) override;
	void OnFieldRemoved(const CString& strField) override;
	void OnRemoveAllFields() override;

	static void DrawShape(CWnd* pWnd, int dwId, int nSize, COLORREF rgbStroke, COLORREF rgbFill);

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg void OnContinuous() override;
	afx_msg void OnDivisional() override;
	//	afx_msg void OnCustomdrawStepsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnGridEndEdit(NMHDR* pNotifyStruct, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_SymbolCombo;
	afx_msg void OnCbnSelchangeCombo();
};

class CSymbolCell : public CGridCell
{
	DECLARE_DYNCREATE(CSymbolCell)
public:
	CSymbolCell();

	~CSymbolCell() override;
public:
	CGraduatedDlg* m_pGraduatedDlg;
	// Attributes
public:
	BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE) override;
};
