#pragma once

/////////////////////////////////////////////////////////////////////////////
// CLED window

class __declspec(dllexport) CLED : public CStatic
{
	public:
	CLED();

	// Attributes
	protected:
	static CImageList m_SignalImage;

	BYTE m_nMode;
	BOOL m_bBright;

	// Operations
	public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLED)m_bBright
	//}}AFX_VIRTUAL

	// Implementation
	public:
	void SetMode(int nMode);

	~CLED() override;

	private:
	// This function cannot be called directly from your dialog application.
	// Its used internally by this class to reset the LED again.

	// Generated message map functions
	protected:
	//{{AFX_MSG(CLED)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
