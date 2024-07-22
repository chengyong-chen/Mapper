#pragma once

/////////////////////////////////////////////////////////////////////////////
// CellEdit.h : header file

// Written by Chris Maunder (Chris.Maunder@cbr.clw.csiro.au)
// Copyright (c) 1998.

// The code contained in this file is based on the original
// CCellEdit from http://www.codeguru.com/listview/edit_subitems.shtml

// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then acknowledgement must be made to the author of this file 
// (in whatever form you wish).

// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.

// Expect bugs!
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 

/////////////////////////////////////////////////////////////////////////////

#define IDC_IPEDIT 8

/////////////////////////////////////////////////////////////////////////////
// CCellEdit window

class __declspec(dllexport) CCellEdit : public CEdit
{
public:
	CCellEdit();
	CCellEdit(int iItem, int iSubItem, CString sInitText);

	// Attributes
public:
	int m_iItem;
	int m_iSubItem;
	CString m_sInitText;
	BOOL m_bESC; // To indicate whether ESC key was pressed

	// Operations
public:
	void EndEdit();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCellEdit)
	BOOL PreTranslateMessage(MSG* pMsg) override;
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CCellEdit() override;

	// Generated message map functions
protected:
	//{{AFX_MSG(CCellEdit)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
