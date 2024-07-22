#pragma once
#include "afxcmn.h"

#include "..\Coding\Instruction.h"

class CChildView;
class CGpsSinkSocket;
struct VehicleStatus;
struct SVehicle;

// CVehicleForm 对话框

class CVehicleForm : public CDialog
{
	DECLARE_DYNAMIC(CVehicleForm)

public:
	CVehicleForm(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CVehicleForm();

// 对话框数据
	enum { IDD = IDD_VEHICLEFORM };

public:
	CComboBoxEx m_comInspector;
	CListCtrl m_listCtrl;

public:
	CChildView* m_pParentView;

public:
	void AddClient(CGpsSinkSocket* pSocket);
	void RemoveClient(CGpsSinkSocket* pSocket);


	void AddVehicle(CGpsSinkSocket* pSocket,VehicleStatus* pVhcStatus);
	void RemoveVehicle(CGpsSinkSocket* pSocket,VehicleStatus* pVhcStatus);
	
	void AddOrphan(VehicleStatus* pVhcStatus);
	void RemoveOrphan(VehicleStatus* pVhcStatus);

	void ShowInspectVehicles(CGpsSinkSocket* pSocket);

public:
	void SetInspectedVehicleTrapMark(const DWORD& dwId, BOOL bMark, CGpsSinkSocket* pSocket=nullptr); // 显示监控车辆的跟踪状态
	void SetInspectedVehicleWarningMark(const DWORD& dwId, BOOL bMark, DWORD dwStatus=0, double time=0, DWORD mask=0); // 显示监控车辆的警告状态

public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageWnd)
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL


protected:
	//{{AFX_MSG(CVehicleForm)
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 支持
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	afx_msg void OnDisInspect();
	afx_msg void OnChangeUser();
	afx_msg void OnAssignUser();
	//}}AFX_MSG

	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRdblclkList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeCombobox();

	DECLARE_MESSAGE_MAP()
};
