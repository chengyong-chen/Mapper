#pragma once

class __declspec(dllexport) CEagleWnd : public CWnd
{
public:
	CEagleWnd();

	~CEagleWnd() override;

public:
	Gdiplus::Image* m_Image = nullptr;

	CRect   m_eyeRect;
	double  m_fHScale;
	double  m_fVScale;

	void OpenEagleBmp(CView* pView, CString strBmp, CRect docRect);
	void SetRect(CView* pView, CRect cliRect);
	void Clear();

public:
	CView* m_pView;
	CString m_strBmp;
	CRect m_docRect;

public:
	CPoint m_ptDown;
	bool m_bDown;

public:
	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEagleWnd)
		//}}AFX_VIRTUAL


	// Generated message map functions
protected:
	//{{AFX_MSG(CEagleWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};