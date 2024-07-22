
#include "msflexgrid.h"





#pragma once

// RoadAttDlg.h : header file


/////////////////////////////////////////////////////////////////////////////
// CRoadAttDlg dialog

#include "Topo.h"
#include "EditFlexGrid.h"

class CRoadAttDlg : public CDialog
{
// Construction
public:
	CRoadAttDlg(CWnd* pParent = nullptr,CTypedPtrMap<CMapWordToPtr,WORD,CTopology::ROADATT*>* mapRoadAtt = nullptr);  

// Dialog Data
	//{{AFX_DATA(CRoadAttDlg)
	enum { IDD = IDD_ROADATT };
	CEditFlexGrid	m_AttGrid;
	//}}AFX_DATA
	
	CTypedPtrMap<CMapWordToPtr,WORD,CTopology::ROADATT*>* m_mapRoadAtt; 
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRoadAttDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRoadAttDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnClickAttGrid();
	afx_msg void OnDblClickAttGrid();
	afx_msg void OnKeyPressAttGrid(short FAR* KeyAscii);
	virtual void OnOK() override;
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};





