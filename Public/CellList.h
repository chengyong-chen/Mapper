#pragma once

// InPlaceList.h : header file

#define IDC_IPLIST 7

class __declspec(dllexport) CCellList : public CListBox
{
public:
	CCellList();

	// Attributes
public:
	int m_nRow;
	int m_nCol;
	BOOL m_bLButtonDown;

	void EndSelect() const;
	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCellList)
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CCellList() override;

	// Generated message map functions
protected:
	//{{AFX_MSG(CCellList)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlag, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
