
#include "msflexgrid.h"





#pragma once


/////////////////////////////////////////////////////////////////////////////
// CMapsvrDlg dialog
#include "Resource.h"
#include "Mapdcom.h"

class CListenSocket;
class CMapSinkSocket;
class CClientDlg;

class CMapsvrDlg : public CDialog
{
// Construction
public:
	CMapsvrDlg(CWnd* pParent = nullptr);	// standard constructor

public:
	CListenSocket* m_pListenSocket;
	CPtrList m_MapSinkList;    

	CClientDlg* m_ClientDlg;

public:
	static IMapdcom m_mapdocm;

public:	
	void InsertClient(CMapSinkSocket *pSocket);
	void RemoveClient(CMapSinkSocket *pSocket);

public:
	CRITICAL_SECTION csClientlist;
	CRITICAL_SECTION csClientDlg;

	
// Dialog Data
	//{{AFX_DATA(CMapsvrDlg)
	enum { IDD = IDD_MAPSVR };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapsvrDlg)
	public:
	virtual BOOL DestroyWindow() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMapsvrDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};





