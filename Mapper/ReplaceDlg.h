#pragma once

#include "../Layer/Layer.h"
#include "resource.h"

class CGeom;

class CReplaceDlg : public CDialog
{
public:
	CReplaceDlg(CWnd* pParent, CDocument* pDocument, CLayerTree& layertree);

	// Dialog Data
	//{{AFX_DATA(CReplaceDlg)
	enum
	{
		IDD = IDD_REPLACE
	};

	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CDocument* m_pDocument;
	CGeom* m_pGeom;
	CLayerTree& m_layertree;
	CTree<CLayer>::forwiterator m_itLayertree;
	POSITION m_posGeomlist;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReplaceDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CReplaceDlg)
	afx_msg void OnReplace();
	afx_msg void OnReplaceAll();
	afx_msg void OnNext();
	afx_msg void OnChangeEdit1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
