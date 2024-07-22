#include "msflexgrid.h"

#pragma once

// MonitorDlg.h : header file

#include "Resource.h"

class CPort;

class __declspec(dllexport) CMonitorEar : public CDialog
{
	public:
	CMonitorEar(CWnd* pParent = nullptr);

	// Dialog Data
	//{{AFX_DATA(CMonitorEar)
	enum
	{
		IDD = IDD_MONITOR
	};

	CMSFlexGrid m_VehGrid;
	//}}AFX_DATA

	public:
	//	CMarkup xmlParser;

	public:
	static BOOL ActivateVehicle(const DWORD& dwId);
	BOOL VehicleControl(UINT WPARAM, LONG LPARAM);
	BOOL VehicleMove(UINT WPARAM, LONG LPARAM);
	BOOL VehicleStatus(UINT WPARAM, LONG LPARAM);

	void MoveFirst(DWORD dwVehicle);
	void MoveGridItem(int idx1, int idx2);
	void DoSort(UINT uType);
	public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonitorEar)
	public:
	BOOL DestroyWindow() override;
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CMonitorEar)
	BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelChangeGrid();
	afx_msg void OnCall();
	afx_msg void OnErase();
	afx_msg void OnTrack();
	afx_msg void OnTrace();
	afx_msg void OnTile();
	afx_msg void OnMessage();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMyCommand();
	afx_msg void OnSetup();
	afx_msg void OnMouseUpMsflexgrid(short Button, short Shift, long x, long y);
	afx_msg void OnMenuitemTrace();
	afx_msg void OnMenuitemTraceStop();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMouseDownMsflexgrid(short Button, short Shift, long x, long y);
	afx_msg void OnMenuitemSortAesc();
	afx_msg void OnMenuitemSortDesc();
	afx_msg void OnMenuitemDispatch();
	afx_msg void OnMenuitemSearch();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
