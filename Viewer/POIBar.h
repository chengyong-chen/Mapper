#pragma once

#include "Resource.h"

#include "../Style/Library.h"

#include "../Layer/Laypp.h"
#include "../Layer/LayerTree.h"

class __declspec(dllexport) CPOIBar : public CDialog
{
	DECLARE_DYNAMIC(CPOIBar)
public:
	CPOIBar(CWnd* pParent = nullptr);

	~CPOIBar() override;

	// Dialog Data
	//{{AFX_DATA(CPOIBar)
	enum
	{
		IDD = IDD_POIBAR
	};

	CComboBox m_POITypeCombo1;
	CComboBox m_POITypeCombo2;
	CComboBox m_POITypeCombo3;
	//}}AFX_DATA

public:
	CDatabase* m_pDatabase;
	CLayerTree m_layertree;
	CLaypp* m_pLaypp;
public:
	void Draw(CWnd* pWnd, Gdiplus::Graphics& g, const CRectF& geoRect, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRect& invalidRect, const CLibrary& library) const;

	void InvalidateView() const;

	void SetDatabase(CDatabase* pDatabase);
	bool GetWhere(CString& strWhere) const;

public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPOIBar)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPOIBar)
	afx_msg void OnTypeCom1SelChanged();
	afx_msg void OnTypeCom2SelChanged();
	afx_msg void OnTypeCom3SelChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
