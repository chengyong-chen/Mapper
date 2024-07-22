#pragma once

class CTool;

class __declspec(dllexport) CInfoToolWnd : public CControlBar
{
	DECLARE_DYNAMIC(CInfoToolWnd)
	// Constructor
public:
	CInfoToolWnd();
	BOOL Create(CWnd* pParentWnd = nullptr, DWORD dwStyle = WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED, UINT nId = AFX_IDW_TOOLBAR);

public:
	BOOL m_bVisible;
private:
	unsigned __int8 row, col;
	CTool** m_pTool;

	// Implementation
public:
	~CInfoToolWnd() override;
	CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz) override;
	void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler) override;

#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

public:
	CWnd* m_wndParent;

	// Generated message map functions
protected:
	//{{AFX_MSG(CInfoToolWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
