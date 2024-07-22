#pragma once

#include "Resource.h"

class CMSCommThread;
class CMSCommCtrl;

class AFX_EXT_CLASS CWavecomDlg : public CDialog
{
	DECLARE_DYNAMIC(CWavecomDlg)

public:
	CWavecomDlg(CWnd* pParent = nullptr);

	~CWavecomDlg() override;

	// Dialog Data
	//{{AFX_DATA(CAtlasAttDlg)
	enum
	{
		IDD = IDD_WAVECOM
	};

	//}}AFX_DATA

protected:
	CMSCommThread* m_pMSCommThread; // 通讯线程

public:
	static HANDLE m_hEvent; // 通讯事件句柄

public:
	BOOL Open();
	void Close();

public:
	bool SendATCmd(CStringA atCmd) const; // 串口控制指令,使用AT指令操作 [COMM_CMG_AT]
	bool SendNormalMsg(CStringA strPhoneNo, CStringA strMsg, BOOL bUseUnicode1) const; // 发送正常消息
	bool SendMsg(CStringA strPhoneNo, CStringA strMsg, BOOL bUseUnicode);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWavecomDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CWavecomDlg)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
