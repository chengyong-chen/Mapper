#pragma once



class CMonitorEar;
class CMessageEar;
class COutputWnd;
class CATTTDBGridCtrl;
class CPOUTDBGridCtrl;
class CAATDBGridCtrl ;
class CPOUDBGridCtrl ;
class CMonitorEar;
class CMessageEar;



/////////////////////////////////////////////////////////////////////////////
// CMyBar window

class __declspec(dllexport) CTablePane : public CDockablePane
{
	public:
	CTablePane(bool bEditable);

	public:
	CMFCTabCtrl m_wndTabs;

	public:
	CATTTDBGridCtrl* m_pATTTDBGridCtrl;
	CPOUTDBGridCtrl* m_pPOUTDBGridCtrl;
	CAATDBGridCtrl* m_pATTDBGridCtrl;
	CPOUDBGridCtrl* m_pPOUDBGridCtrl;
	CMonitorEar* m_pMonitorCtrl;
	CMessageEar* m_pMessageCtrl;

	public:
	static CString GetTabLabel(char cTab);
	CWnd* GetTabWnd(char cTab) const;
	void ActivateTab(char cTab);

	void SQLQuery(char type, CDatabase* pDatabase, CString strSQL, CString strIdField);

	public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTablePane)	
	//	virtual BOOL DestroyWindow() override;
	//}}AFX_VIRTUAL

	// Implementation
	~CTablePane() override;

	// Generated message map functions
	protected:
	//{{AFX_MSG(CTablePane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
