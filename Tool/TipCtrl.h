#if !defined(_TIPCTRL_H__)
#define _TIPCTRL_H__


#pragma once


class CHyperlinkStatic;

class __declspec(dllexport) CTagCtrl : public CWnd
{
// Construction
public:
	CTagCtrl();

// Operations
public:
    BOOL Create(CWnd* pParentWnd);
	void RelayEvent(MSG* pMsg);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTagCtrl)
	//}}AFX_VIRTUAL
public:
    void DisplayTag(const CPoint& pt,CString strTag);

protected:
	HWND m_hOwnerWnd;
	CString m_strTag;
public:
	CHyperlinkStatic* m_pHyperlinkStatic;
private:
	CFont m_font;

	// Generated message map functions
protected:
	//{{AFX_MSG(CTagCtrl)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow() override;
};

/////////////////////////////////////////////////////////////////////////////




#endif // !defined(_OXTOOLTIPCTRL_H__)
