#pragma once

#include "Resource.h"
class  CDatainfo;
class CProjection;
/////////////////////////////////////////////////////////////////////////////
// CDatainfoDlg dialog

class __declspec(dllexport) CDatainfoDlg : public CDialog
{
	typedef struct tagPAPERSIZE
	{
		TCHAR Name[10];
		long width;
		long height;
	} PAPERSIZE;

	static PAPERSIZE g_paper[];

public:
	CDatainfoDlg(CWnd* pParent, CDatainfo& datainfo);

	// Dialog Data
	//{{AFX_DATA(CDatainfoDlg)
	enum
	{
		IDD = IDD_DATAINFO
	};

	CString m_strName;
	CTime m_TimeLine;
	//}}AFX_DATA

public:
	CDatainfo& m_Datainfo;

	CProjection* m_pProjection;
	double m_xOrigin;
	double m_yOrigin;
	double m_cxCanvas;
	double m_cyCanvas;

	float m_scaleMinimum;
	float m_scaleSource;
	float m_scaleMaximum;

public:
	void UpdateHAndW() const;
	void ResetProject(CProjection* pProjection);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDatainfoDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDatainfoDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
	BOOL DestroyWindow() override;
	//}}AFX_MSG

protected:
	// Generated message map functions
	//{{AFX_MSG(CPaperDlg)
	afx_msg void OnSelchangePapercombo();
	afx_msg void OnPortraitradio();
	afx_msg void OnLandscaperadio();
	afx_msg void OnProjectionTypeChange();
	afx_msg void OnGCSSelchange();
	afx_msg void OnSetupProjection();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
