#include "Message.h"
#include "Resource.h"

#pragma once

// NavinfoPane.h : header file

/////////////////////////////////////////////////////////////////////////////
// CNavinfoDlg dialog

class __declspec(dllexport) CNavinfoDlg : public CDialog
{
	public:
	CNavinfoDlg(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CNavinfoDlg)
	enum
	{
		IDD = IDD_NAVINFO
	};

	double m_fSpeed;
	long m_nTime;
	long m_nAltitude;
	long m_nSatelites;
	//}}AFX_DATA

	public:
	int m_nAngle;
	int m_bAvailable;

	DWORD m_dwId;
	public:
	void SetID(const DWORD& dwId);
	void SetInfo(const DWORD& dwId, SMessage& message);
	void SetRouteLength(float fLength) const;
	void SetLeftLength(float fLength) const;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNavinfoDlg)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CNavinfoDlg)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
