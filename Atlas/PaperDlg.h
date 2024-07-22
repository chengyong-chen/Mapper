#pragma once

#include "resource.h"



class __declspec(dllexport) CPaperDlg : public CDialog
{
	typedef struct tagPAPERSIZE
	{
		TCHAR papername[10];
		long  width;
		long  height;
	} PAPERSIZE;
	static PAPERSIZE g_paper[] = 
	{
		{_T("Custom"), 0, 0},
		{_T("A4"), 595, 842},
		{_T("A3"), 842, 1190},
		{_T("A2"), 1191, 1684},
		{_T("A1"), 1684, 2384},
		{_T("A0"), 2384, 3370},
		{_T("B4"), 729, 1032},
		{_T("B3"), 1032, 1460},
		{_T("B2"), 1460, 2064},
		{_T("B1"), 2064, 2920},
		{_T("B0"), 2920, 4127},
	};

// Construction
public:
	CPaperDlg(CWnd* pParent = nullptr,long nWidth=0,long nHeight=0,long nXMargin=0,long nYMargin=0,long viewLower=10,long viewUpper=800,long nXGrid=0,long nYGrid=0);  

// Dialog Data
	//{{AFX_DATA(CPaperDlg)
	enum { IDD = IDD_PAPER };
	CComboBox m_papercombo;
	long  m_viewLower;
	long  m_viewUpper;
	UINT  m_cxGrid;
	UINT  m_cyGrid;
	//}}AFX_DATA

private:
	BOOL    m_portrait;	//TRUE	Vertical //FALSE	Horizontal
	void    UpdateHAndW();
public:
	long     m_nWidth;
	long     m_nHeight;
		
	long	 m_nXMargin;
	long	 m_nYMargin;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaperDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPaperDlg)
	afx_msg void OnSelchangePapercombo();
	afx_msg void OnPortraitradio();
	afx_msg void OnLandscaperadio();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnChangePaperwidth();
	afx_msg void OnChangePaperheight();
	afx_msg void OnChangeXMargin();
	afx_msg void OnChangeYMargin();
	afx_msg void OnStandadradio();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};