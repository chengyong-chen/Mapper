
#include "msflexgrid.h"





#pragma once

// ClientDlg.h : header file


/////////////////////////////////////////////////////////////////////////////
// CClientDlg dialog

class CMapSinkSocket;

class CClientDlg : public CDialog
{
// Construction
public:
	CClientDlg(CWnd* pParent = nullptr);  

// Dialog Data
	//{{AFX_DATA(CClientDlg)
	enum { IDD = IDD_CLIENT };
	CMSFlexGrid	m_ClientGrid;
	//}}AFX_DATA

public:
	void InsertClient(CMapSinkSocket* pSocket);
	void RemoveClient(CMapSinkSocket* pSocket);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CClientDlg)
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};





