#pragma once

#include "Resource.h"

class CColorSpot;

class CStyleDlg : public CDialog
{
	public:
	CStyleDlg(CWnd* pParent = nullptr, COLORREF colorNode = RGB(0,0,0), COLORREF colorEdge = RGB(255,0,255));

	// Dialog Data
	//{{AFX_DATA(CStyleDlg)
	enum
	{
		IDD = IDD_STYLE
	};

	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStyleDlg)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	public:
	COLORREF m_colorNode;
	COLORREF m_colorEdge;

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CStyleDlg)
	afx_msg void OnPaint();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDCEdgeColor();
	afx_msg void OnDCNodeColor();
};
