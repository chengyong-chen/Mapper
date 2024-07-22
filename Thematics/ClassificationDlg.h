#pragma once
#include "../Database/Datasource.h"

// CClassificationDlg command target
using namespace std;
#include <list>
#include <vector>

class CClassificationDlg abstract : public CDialogEx
{
public:
	CClassificationDlg(UINT IDD, CWnd* pParent, CDatabase& database, CATTDatasource& datasource);

	~CClassificationDlg() override;

public:
	CListCtrl m_ColorListCtrl;
	CDatabase& m_database;
	CATTDatasource& m_datasource;

public:
	std::vector<std::vector<Gdiplus::ARGB>> m_favcolors;

public:
	virtual void OnFieldAdded(const CString& strField)
	{
	};

	virtual void OnFieldRemoved(const CString& strField)
	{
	};

	virtual void OnRemoveAllFields()
	{
	};
	virtual void AddFields(std::list<CString> fields);

	virtual void SetSteps(int steps)
	{
	};
	virtual void LoadFavoriteColors(CString strType, CListCtrl* pListCtrl, int subs, std::vector<std::vector<Gdiplus::ARGB>>& favcolors);
protected:
	CMFCPropertyGridProperty* GetPropertyByName(CMFCPropertyGridCtrl* pPropertyGrid, CString strName);
	CMFCPropertyGridProperty* GetPropertyByName(CMFCPropertyGridProperty* pProperty, CString strName) const;
	COleVariant GetPropertyValue(CMFCPropertyGridCtrl* pPropertyGrid, CString strName);

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	afx_msg virtual void OnContinuous();
	afx_msg virtual void OnDivisional();
	afx_msg void OnCustomdrawColorList(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};
