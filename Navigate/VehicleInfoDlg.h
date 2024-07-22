#pragma once

#include "Resource.h"
#include "Message.h"

class __declspec(dllexport) CVehicleInfoDlg : public CDialog
{
	public:
	CVehicleInfoDlg(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CVehicleInfoDlg)
	enum
	{
		IDD = IDD_VEHICLEINFO
	};

	double m_fSpeed;
	CString m_strRoad;
	long m_nTime;
	long m_nAltitude;
	long m_nSatelites;
	//}}AFX_DATA
	public:
	int m_nBear;

	public:
	void SetRoad(CString strRoad);
	void SetInfo(SMessage& message);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVehicleInfoDlg)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CVehicleInfoDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
