#pragma once

#include "StatusCombo.h"

class CDatainfo;
const int indexVIEWLEVEL = 1;
const int indexVIEWSCALE = 2;
const int indexGEOMNAME = 3;
const int indexGEOCODE = 4;
const int indexGEOMID = 5;
const int indexPOINTCOUNT = 6;
const int indexHANDLEXY = 7;
const int indexCOORDINATE = 8;
const int indexUNITCOMBO = 9;

#include "LevelBar.h"
#include "ScaleBar.h"

class __declspec(dllexport) CStatusBarEx : public CMFCStatusBar
{
public:
	CStatusBarEx();

	~CStatusBarEx() override;

public:
	CLevelBar m_LevelBar;
	CScaleBar m_ScaleBar;
	CStatusCombo m_StatusCombo;

public:
	void ViewChanged(const CDatainfo& datainfo);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusBarEx)
public:
	virtual BOOL Create(CWnd* wnd);
	//}}AFX_VIRTUAL
	// Implementation

	virtual void SetViewBackground(int nProvider, int nVariety);

	//{{AFX_MSG(CStatusBarEx)	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
