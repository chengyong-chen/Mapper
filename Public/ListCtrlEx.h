#pragma once

class __declspec(dllexport) CListCtrlEx : public CListCtrl
{
public:
	CListCtrlEx();

	// Attributes
public:
	bool m_bEditble;

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlEx)
	void PreSubclassWindow() override;
	int OnToolHitTest(CPoint point, TOOLINFO* pTI) const override;

	//}}AFX_VIRTUAL

	// Implementation
public:
	~CListCtrlEx() override;

public:
	int HitTestEx(const CPoint& point, int* col) const;
	int CellRectFromPoint(const CPoint& point, RECT* cellrect, int* col) const;

	virtual CEdit* EditSubLabel(int nItem, int nCol);

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlEx)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	afx_msg BOOL OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
