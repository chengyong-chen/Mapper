#include "MSCommCtrl.h"

#pragma once

#include <comutil.h>

#include "Resource.h"
#include "Frame.h"

class CMSCommDlg : public CDialog
{
	DECLARE_DYNAMIC(CMSCommDlg)

public:
	CMSCommDlg(CWnd* pParent = nullptr);

	~CMSCommDlg() override;

	// Dialog Data
	//{{AFX_DATA(CMSCommDlg)
	enum
	{
		IDD = IDD_MSCOMM
	};

	CMscommCtrl m_comm;
	//}}AFX_DATA

public:
	_variant_t m_vtRecvMsg; // 接收到的消息

public:
	static MSCommFrame m_sendFrame; // 发送消息缓冲区地址
	static MSCommFrame m_recvFrame; // 接收消息缓冲区地址

	void OnMsgReceived() const;

public:
	void Close();
	BOOL Open();

protected:
	BOOL NormalRespProc(); // 常见发送指令应答处理
	BOOL ATCmdRespProc(); // AT指令发送应答处理

protected:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMSCommDlg)
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CMSCommDlg)
	afx_msg void OnCommMscomm();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//以下为需要在队列中执行的操作
	afx_msg LONG OnOpenPort(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnTimeOut(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnSendMsg(WPARAM wParam, LPARAM lParam);

public:
	static UINT ProcessRecvSMSThread(LPVOID lParam);
};
