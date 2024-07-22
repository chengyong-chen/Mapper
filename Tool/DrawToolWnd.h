#pragma once

class CTool;

class __declspec(dllexport) CDrawToolWnd : public CControlBar
{
	DECLARE_DYNAMIC(CDrawToolWnd)
	// Constructor
public:
	CDrawToolWnd();
	BOOL Create(CWnd* pParentWnd = nullptr, DWORD dwStyle = WS_CHILD|WS_VISIBLE|CBRS_SIZE_FIXED, UINT nId = AFX_IDW_TOOLBAR);

private:
	CBitmap m_bitmap;
	CWnd* m_wndParent;
	CTool* m_Tools[10][2];
	CTool** m_ppTool;
public:
	~CDrawToolWnd() override;

public:
	void SwitchSource(CTool** ppTool);
	void SwitchToolTo(CTool* pTool);

#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportDIBDlg)
protected:
	CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz) override;
	void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler) override;
public:
	BOOL DestroyWindow() override;
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CDrawToolWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint docPoint);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
