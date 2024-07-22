// ChildView.h : interface of the CChildView class



#pragma once


#include "VehicleVect.h" 
#include "..\Socket\ListenSocket.h"
#include "..\Socket\GpsSinkSocket.h" 
#include "..\Socket\XmlSinkSocket.h"


class CSMSThread;
class CUDPThread;
class CTCPThread;
class CHTTPThread;

class CGpsSinkSocket;
class CXmlSinkSocket;
class CSecurityManager;
class CMessageReceived;
class CVehicleForm;
class CMessageForm;

#include <map>
#include <list>
#include <queue>
using namespace std;

const UINT STATE_WARNING = 0X01;
const UINT STATE_STATE   = 0X02;
const UINT STATE_TIME    = 0X04;

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:
	CRITICAL_SECTION csClientlist;

public:
	CSMSThread*  m_pSMSThread;
	CUDPThread*  m_pUDPThread;
	CTCPThread*  m_pTCPThread;
	CHTTPThread* m_pHTTPThread;

public:
	CWnd* m_pActiveWnd;
	void ChangeContent(const DWORD& dwId,DWORD wParam=0);

public:
	ADOCG::_ConnectionPtr m_connectPtr;
	CVehicleVect m_vectVehicle;
	CVehicleMap m_mapOrphanVehicle;

public:
	CListenSocket m_sockGpsListen;
	CListenSocket m_sockXmlListen;
	CListenSocket m_sockWebListen;
	
	CPtrList m_SinkSocketList;     // 已连接监控员列表

public:
	CGpsSinkSocket* GetClientSocketByUser(LPCTSTR lpszUser);
	CGpsSinkSocket* GetClientSocketByVehicle(const DWORD& dwId);
	CGpsSinkSocket* GetEasiestClient(DWORD dwVehicle,bool bForce);

public:
	static CMessageQueue g_SendMsgSaveQueue;
	static CMessageQueue g_RecvMsgSaveQueue;

public:
	BOOL Start();
	void Stop();

public:
	bool PutSendMsgToSaveQueue(CMessageWillSend* pMsg);
	bool PutRecvMsgToSaveQueue(CMessageReceived* pMsg);

	bool PutMsgToSMSSendQueue(CMessageWillSend* msg, DWORD dwTimeOut = 1000);
	bool PutMsgToUDPSendQueue(CMessageWillSend* msg, DWORD dwTimeOut = 1000);
	bool PutMsgToTCPSendQueue(CMessageWillSend* msg, DWORD dwTimeOut = 1000);

public:
	void ProcessInMsg(CMessageReceived* pMsg,int nMsgCount);
	void PutMsgToMessageWnd(LPCTSTR lpMsg);
	bool BroadCastMsgToEveryClient(CMessageReceived& msg, BOOL bForceBroad=FALSE);


	void FreshVectVehicle();

public:	
	BOOL AskInspectOneVehicle(const DWORD& dwId, CGpsSinkSocket* pSocket);
	BOOL AddOrphanVehicle(DWORD dwVehicle);

	bool VerifyUser(const CString &strUser,const CString &strPassword,DWORD* dwErr, ADOCG::_ConnectionPtr &pDBConnect, DWORD* dwUserID=nullptr);

public:
	void DeleteClient(CGpsSinkSocket *pSocket);
	void AddClient(CGpsSinkSocket *pSocket);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	public:
	virtual BOOL DestroyWindow() override;
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG	
	afx_msg LONG OnDeleteClient(UINT WPARAM, LONG LPARAM);
	DECLARE_MESSAGE_MAP()
};